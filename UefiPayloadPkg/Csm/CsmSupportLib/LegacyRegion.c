/** @file
  Legacy Region Support

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "LegacyRegion.h"

//
// Handle used to install the Legacy Region Protocol
//
STATIC EFI_HANDLE  mHandle = NULL;

//
// Instance of the Legacy Region Protocol to install into the handle database
//
STATIC EFI_LEGACY_REGION2_PROTOCOL  mLegacyRegion2 = {
  LegacyRegion2Decode,
  LegacyRegion2Lock,
  LegacyRegion2BootLock,
  LegacyRegion2Unlock,
  LegacyRegionGetInfo
};

//
// Current chipset's tables
//
UINT8                               mPamPciBus = 0;
UINT8                               mPamPciDev = 0;
UINT8                               mPamPciFunc = 0;
UINT8                               mPamRegStart = 0;

//
// PAM map.
//
// PAM Range          Offset    Bits  Operation
//                  440   Q35
// ===============  ====  ====  ====  ===============================================================
// 0xC0000-0xC3FFF  0x5a  0x91  1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xC4000-0xC7FFF  0x5a  0x91  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xC8000-0xCBFFF  0x5b  0x92  1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xCC000-0xCFFFF  0x5b  0x92  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xD0000-0xD3FFF  0x5c  0x93  1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xD4000-0xD7FFF  0x5c  0x93  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xD8000-0xDBFFF  0x5d  0x94  1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xDC000-0xDFFFF  0x5d  0x94  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xE0000-0xE3FFF  0x5e  0x95  1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xE4000-0xE7FFF  0x5e  0x95  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xE8000-0xEBFFF  0x5f  0x96  1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xEC000-0xEFFFF  0x5f  0x96  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xF0000-0xFFFFF  0x59  0x90  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
//
STATIC LEGACY_MEMORY_SECTION_INFO   mSectionArray[] = {
  {0xC0000, SIZE_16KB, FALSE, FALSE},
  {0xC4000, SIZE_16KB, FALSE, FALSE},
  {0xC8000, SIZE_16KB, FALSE, FALSE},
  {0xCC000, SIZE_16KB, FALSE, FALSE},
  {0xD0000, SIZE_16KB, FALSE, FALSE},
  {0xD4000, SIZE_16KB, FALSE, FALSE},
  {0xD8000, SIZE_16KB, FALSE, FALSE},
  {0xDC000, SIZE_16KB, FALSE, FALSE},
  {0xE0000, SIZE_16KB, FALSE, FALSE},
  {0xE4000, SIZE_16KB, FALSE, FALSE},
  {0xE8000, SIZE_16KB, FALSE, FALSE},
  {0xEC000, SIZE_16KB, FALSE, FALSE},
  {0xF0000, SIZE_64KB, FALSE, FALSE}
};

STATIC PAM_REGISTER_VALUE  mRegisterValues[] = {
  {PAM1_OFFSET, 0x01, 0x02},
  {PAM1_OFFSET, 0x10, 0x20},
  {PAM2_OFFSET, 0x01, 0x02},
  {PAM2_OFFSET, 0x10, 0x20},
  {PAM3_OFFSET, 0x01, 0x02},
  {PAM3_OFFSET, 0x10, 0x20},
  {PAM4_OFFSET, 0x01, 0x02},
  {PAM4_OFFSET, 0x10, 0x20},
  {PAM5_OFFSET, 0x01, 0x02},
  {PAM5_OFFSET, 0x10, 0x20},
  {PAM6_OFFSET, 0x01, 0x02},
  {PAM6_OFFSET, 0x10, 0x20},
  {PAM0_OFFSET, 0x10, 0x20}
};

STATIC
EFI_STATUS
LegacyRegionManipulationInternal (
  IN  UINT32                  Start,
  IN  UINT32                  Length,
  IN  BOOLEAN                 *ReadEnable,
  IN  BOOLEAN                 *WriteEnable,
  OUT UINT32                  *Granularity
  )
{
  UINT32                        EndAddress;
  UINTN                         Index;
  UINTN                         StartIndex;

  //
  // Validate input parameters.
  //
  if (Length == 0 || Granularity == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  EndAddress = Start + Length - 1;
  if ((Start < PAM_BASE_ADDRESS) || EndAddress > PAM_LIMIT_ADDRESS) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Loop to find the start PAM.
  //
  StartIndex = 0;
  for (Index = 0; Index < ARRAY_SIZE (mSectionArray); Index++) {
    if ((Start >= mSectionArray[Index].Start) && (Start < (mSectionArray[Index].Start + mSectionArray[Index].Length))) {
      StartIndex = Index;
      break;
    }
  }
  ASSERT (Index < ARRAY_SIZE (mSectionArray));

  //
  // Program PAM until end PAM is encountered
  //
  for (Index = StartIndex; Index < ARRAY_SIZE (mSectionArray); Index++) {
    if (ReadEnable != NULL) {
      if (*ReadEnable) {
        PciOr8 (
          (PCI_LIB_ADDRESS(mPamPciBus, mPamPciDev, mPamPciFunc, mPamRegStart +
                           mRegisterValues[Index].PAMRegOffset)),
          mRegisterValues[Index].ReadEnableData
          );
      } else {
        PciAnd8 (
          (PCI_LIB_ADDRESS(mPamPciBus, mPamPciDev, mPamPciFunc, mPamRegStart +
                           mRegisterValues[Index].PAMRegOffset)),
          (UINT8) (~mRegisterValues[Index].ReadEnableData)
          );
      }
    }
    if (WriteEnable != NULL) {
      if (*WriteEnable) {
        PciOr8 (
          (PCI_LIB_ADDRESS(mPamPciBus, mPamPciDev, mPamPciFunc, mPamRegStart +
                           mRegisterValues[Index].PAMRegOffset)),
          mRegisterValues[Index].WriteEnableData
          );
      } else {
        PciAnd8 (
          (PCI_LIB_ADDRESS(mPamPciBus, mPamPciDev, mPamPciFunc, mPamRegStart +
                           mRegisterValues[Index].PAMRegOffset)),
          (UINT8) (~mRegisterValues[Index].WriteEnableData)
          );
      }
    }

    //
    // If the end PAM is encountered, record its length as granularity and jump out.
    //
    if ((EndAddress >= mSectionArray[Index].Start) && (EndAddress < (mSectionArray[Index].Start + mSectionArray[Index].Length))) {
      *Granularity = mSectionArray[Index].Length;
      break;
    }
  }
  ASSERT (Index < ARRAY_SIZE (mSectionArray));

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
LegacyRegionGetInfoInternal (
  OUT UINT32                        *DescriptorCount,
  OUT LEGACY_MEMORY_SECTION_INFO    **Descriptor
  )
{
  UINTN    Index;
  UINT8    PamValue;

  //
  // Check input parameters
  //
  if (DescriptorCount == NULL || Descriptor == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Fill in current status of legacy region.
  //
  *DescriptorCount = ARRAY_SIZE (mSectionArray);
  for (Index = 0; Index < *DescriptorCount; Index++) {
    PamValue = PciRead8 (PCI_LIB_ADDRESS(mPamPciBus, mPamPciDev, mPamPciFunc, mPamRegStart +
                         mRegisterValues[Index].PAMRegOffset));
    mSectionArray[Index].ReadEnabled = FALSE;
    if ((PamValue & mRegisterValues[Index].ReadEnableData) != 0) {
      mSectionArray[Index].ReadEnabled = TRUE;
    }
    mSectionArray[Index].WriteEnabled = FALSE;
    if ((PamValue & mRegisterValues[Index].WriteEnableData) != 0) {
      mSectionArray[Index].WriteEnabled = TRUE;
    }
  }

  *Descriptor = mSectionArray;
  return EFI_SUCCESS;
}

/**
  Modify the hardware to allow (decode) or disallow (not decode) memory reads in a region.

  If the On parameter evaluates to TRUE, this function enables memory reads in the address range
  Start to (Start + Length - 1).
  If the On parameter evaluates to FALSE, this function disables memory reads in the address range
  Start to (Start + Length - 1).

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose attributes
                                should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address
                                was not aligned to a region's starting address or if the length
                                was greater than the number of bytes in the first region.
  @param  On[in]                Decode / Non-Decode flag.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2Decode (
  IN  EFI_LEGACY_REGION2_PROTOCOL  *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity,
  IN  BOOLEAN                      *On
  )
{
  return LegacyRegionManipulationInternal (Start, Length, On, NULL, Granularity);
}


/**
  Modify the hardware to disallow memory attribute changes in a region.

  This function makes the attributes of a region read only. Once a region is boot-locked with this
  function, the read and write attributes of that region cannot be changed until a power cycle has
  reset the boot-lock attribute. Calls to Decode(), Lock() and Unlock() will have no effect.

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose
                                attributes should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address was
                                not aligned to a region's starting address or if the length was
                                greater than the number of bytes in the first region.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.
  @retval EFI_UNSUPPORTED       The chipset does not support locking the configuration registers in
                                a way that will not affect memory regions outside the legacy memory
                                region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2BootLock (
  IN  EFI_LEGACY_REGION2_PROTOCOL         *This,
  IN  UINT32                              Start,
  IN  UINT32                              Length,
  OUT UINT32                              *Granularity
  )
{
  if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_UNSUPPORTED;
}


/**
  Modify the hardware to disallow memory writes in a region.

  This function changes the attributes of a memory range to not allow writes.

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose
                                attributes should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address was
                                not aligned to a region's starting address or if the length was
                                greater than the number of bytes in the first region.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2Lock (
  IN  EFI_LEGACY_REGION2_PROTOCOL *This,
  IN  UINT32                      Start,
  IN  UINT32                      Length,
  OUT UINT32                      *Granularity
  )
{
  BOOLEAN  WriteEnable;

  WriteEnable = FALSE;
  return LegacyRegionManipulationInternal (Start, Length, NULL, &WriteEnable, Granularity);
}


/**
  Modify the hardware to allow memory writes in a region.

  This function changes the attributes of a memory range to allow writes.

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose
                                attributes should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address was
                                not aligned to a region's starting address or if the length was
                                greater than the number of bytes in the first region.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2Unlock (
  IN  EFI_LEGACY_REGION2_PROTOCOL  *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity
  )
{
  BOOLEAN  WriteEnable;

  WriteEnable = TRUE;
  return LegacyRegionManipulationInternal (Start, Length, NULL, &WriteEnable, Granularity);
}

/**
  Get region information for the attributes of the Legacy Region.

  This function is used to discover the granularity of the attributes for the memory in the legacy
  region. Each attribute may have a different granularity and the granularity may not be the same
  for all memory ranges in the legacy region.

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  DescriptorCount[out]  The number of region descriptor entries returned in the Descriptor
                                buffer.
  @param  Descriptor[out]       A pointer to a pointer used to return a buffer where the legacy
                                region information is deposited. This buffer will contain a list of
                                DescriptorCount number of region descriptors.  This function will
                                provide the memory for the buffer.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegionGetInfo (
  IN  EFI_LEGACY_REGION2_PROTOCOL   *This,
  OUT UINT32                        *DescriptorCount,
  OUT EFI_LEGACY_REGION_DESCRIPTOR  **Descriptor
  )
{
  LEGACY_MEMORY_SECTION_INFO   *SectionInfo;
  UINT32                       SectionCount;
  EFI_LEGACY_REGION_DESCRIPTOR *DescriptorArray;
  UINTN                        Index;
  UINTN                        DescriptorIndex;

  //
  // Get section numbers and information
  //
  LegacyRegionGetInfoInternal (&SectionCount, &SectionInfo);

  //
  // Each section has 3 descriptors, corresponding to readability, writeability, and lock status.
  //
  DescriptorArray = AllocatePool (sizeof (EFI_LEGACY_REGION_DESCRIPTOR) * SectionCount * 3);
  if (DescriptorArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  DescriptorIndex = 0;
  for (Index = 0; Index < SectionCount; Index++) {
    DescriptorArray[DescriptorIndex].Start       = SectionInfo[Index].Start;
    DescriptorArray[DescriptorIndex].Length      = SectionInfo[Index].Length;
    DescriptorArray[DescriptorIndex].Granularity = SectionInfo[Index].Length;
    if (SectionInfo[Index].ReadEnabled) {
      DescriptorArray[DescriptorIndex].Attribute   = LegacyRegionDecoded;
    } else {
      DescriptorArray[DescriptorIndex].Attribute   = LegacyRegionNotDecoded;
    }
    DescriptorIndex++;

    //
    // Create descriptor for writeability, according to lock status
    //
    DescriptorArray[DescriptorIndex].Start       = SectionInfo[Index].Start;
    DescriptorArray[DescriptorIndex].Length      = SectionInfo[Index].Length;
    DescriptorArray[DescriptorIndex].Granularity = SectionInfo[Index].Length;
    if (SectionInfo[Index].WriteEnabled) {
      DescriptorArray[DescriptorIndex].Attribute = LegacyRegionWriteEnabled;
    } else {
      DescriptorArray[DescriptorIndex].Attribute = LegacyRegionWriteDisabled;
    }
    DescriptorIndex++;

    //
    // Chipset does not support bootlock.
    //
    DescriptorArray[DescriptorIndex].Start       = SectionInfo[Index].Start;
    DescriptorArray[DescriptorIndex].Length      = SectionInfo[Index].Length;
    DescriptorArray[DescriptorIndex].Granularity = SectionInfo[Index].Length;
    DescriptorArray[DescriptorIndex].Attribute   = LegacyRegionNotLocked;
    DescriptorIndex++;
  }

  *DescriptorCount = (UINT32) DescriptorIndex;
  *Descriptor      = DescriptorArray;

  return EFI_SUCCESS;
}

/**
  Detects chipset and initializes PAM support tables

  @retval EFI_SUCCESS   Successfully initialized

  OpenCore: Modified by dmazar with support for different chipsets and added newer ones.

**/
STATIC
EFI_STATUS
DetectChipset (
  VOID
  )
{
  UINT16  VendorId = 0;
  UINT16  DeviceId = 0;

  UINT16 mDeviceId = PciRead16 (PCI_LIB_ADDRESS(PAM_PCI_BUS, PAM_PCI_DEV, PAM_PCI_FUNC, 0x02));

  switch (mDeviceId) {

    //
    // Copied from 915 resolution created by steve tomljenovic,
    // Resolution module by Evan Lojewski
    //
    case 0x3575:  // 830
    case 0x3580:  // 855GM
      // Intel 830 and similar (PAM 0x59-0x5f)
      mPamRegStart = 0x59;
      break;

    case 0x25C0:  // 5000
    case 0x25D4:  // 5000V
    case 0x65C0:  // 5100
      // Intel 5000 and similar (PAM 0x59-0x5f)
      mPamRegStart = 0x59;
      mPamPciDev = 16;
      break;

    //
    // Copied from 915 resolution created by steve tomljenovic,
    // Resolution module by Evan Lojewski
    //
    case 0x2560:  // 845G
    case 0x2570:  // 865G
    case 0x2580:  // 915G
    case 0x2590:  // 915GM
    case 0x2770:  // 945G
    case 0x2774:  // 955X
    case 0x277c:  // 975X
    case 0x27a0:  // 945GM
    case 0x27ac:  // 945GME
    case 0x2920:  // G45
    case 0x2970:  // 946GZ
    case 0x2980:  // G965
    case 0x2990:  // Q965
    case 0x29a0:  // P965
    case 0x29b0:  // R845
    case 0x29c0:  // G31/P35
    case 0x29d0:  // Q33
    case 0x29e0:  // X38/X48
    case 0x2a00:  // 965GM
    case 0x2a10:  // GME965/GLE960
    case 0x2a40:  // PM/GM45/47
    case 0x2e00:  // Eaglelake
    case 0x2e10:  // B43
    case 0x2e20:  // P45
    case 0x2e30:  // G41
    case 0x2e40:  // B43 Base
    case 0x2e90:  // B43 Soft Sku
    case 0x8100:  // 500
    case 0xA000:  // 3150
      // Intel Series 4 and similar (PAM 0x90-0x96)
      mPamRegStart = 0x90;
      break;

    //
    // 1st gen i7 - Nehalem
    //
    case 0x0040:  // Core Processor DRAM Controller
    case 0x0044:  // Core Processor DRAM Controller - Arrandale
    case 0x0048:  // Core Processor DRAM Controller
    case 0x0069:  // Core Processor DRAM Controller
    case 0xD130:  // Xeon(R) CPU L3426 Processor DRAM Controller
    case 0xD131:  // Core-i Processor DRAM Controller
    case 0xD132:  // PM55 i7-720QM  DRAM Controller
    case 0x3400:  // Core-i Processor DRAM Controller
    case 0x3401:  // Core-i Processor DRAM Controller
    case 0x3402:  // Core-i Processor DRAM Controller
    case 0x3403:  // Core-i Processor DRAM Controller
    case 0x3404:  // Core-i Processor DRAM Controller
    case 0x3405:  // X58 Core-i Processor DRAM Controller
    case 0x3406:  // Core-i Processor DRAM Controller
    case 0x3407:  // Core-i Processor DRAM Controller
      // Core i7 processors (PAM 0x40-0x46)
      mPamRegStart = 0x40;
      mPamPciBus = 0xFF;
      for (mPamPciBus = 0xFF; mPamPciBus > 0x1F; mPamPciBus >>= 1) {
        VendorId = PciRead16 (PCI_LIB_ADDRESS(mPamPciBus, 0, 1, 0x00));
        if (VendorId != 0x8086) {
          continue;
        }
        DeviceId = PciRead16 (PCI_LIB_ADDRESS(mPamPciBus, 0, 1, 0x02));
        if (DeviceId > 0x2c00) {
          break;
        }
      }
      if ((VendorId != 0x8086) || (DeviceId < 0x2c00)) {
        //
        // Nehalem bus is not found, assume 0
        //
        mPamPciBus = 0;
      } else {
        mPamPciFunc = 1;
      }
      break;

    case 0x3C00:  // Xeon E5 Processor
      // Xeon E5 processors (PAM 0x40-0x46)
      mPamRegStart = 0x40;
      mPamPciBus = PciRead8 (PCI_LIB_ADDRESS(0, 5, 0, 0x109));
      mPamPciDev = 12;
      mPamPciFunc = 6;
      break;

    //
    // Core processors
    // http://pci-ids.ucw.cz/read/PC/8086
    //
    case 0x0100:  // 2nd Generation Core Processor Family DRAM Controller
    case 0x0104:  // 2nd Generation Core Processor Family DRAM Controller
    case 0x0108:  // Xeon E3-1200 2nd Generation Core Processor Family DRAM Controller
    case 0x010c:  // Xeon E3-1200 2nd Generation Core Processor Family DRAM Controller
    case 0x0150:  // 3rd Generation Core Processor Family DRAM Controller
    case 0x0154:  // 3rd Generation Core Processor Family DRAM Controller
    case 0x0158:  // 3rd Generation Core Processor Family DRAM Controller
    case 0x015c:  // 3rd Generation Core Processor Family DRAM Controller
    case 0x0160:  // 3rd Generation Core Processor Family DRAM Controller
    case 0x0164:  // 3rd Generation Core Processor Family DRAM Controller
    case 0x0A04:  // 4rd Generation U-Processor Series
    case 0x0C00:  // 4rd Generation Core Processor Family DRAM Controller
    case 0x0C04:  // 4rd Generation M-Processor Series
    case 0x0C08:  // 4rd Generation Haswell Xeon
    case 0x0D04:  // 4rd Generation H-Processor Series (BGA) with GT3 Graphics
    case 0x0F00:  // Bay Trail Family DRAM Controller
    case 0x1604:  // 5th Generation Core Processor Family DRAM Controller
    case 0x1904:  // 6th Generation (Skylake-U) DRAM Controller
    case 0x190f:  // 6th Generation (Skylake) DRAM Controller (Desktop)
    case 0x190c:  // 6th Generation (Skylake-Y) DRAM Controller
    case 0x1910:  // 6th Generation (Skylake-M) DRAM Controller
    case 0x1918:  // 6th Generation (Skylake) DRAM Controller (Workstation)
    case 0x191f:  // 6th Generation (Skylake) DRAM Controller (Z170X)
    case 0x3ed0:  // 8th Generation (Coffeelake-U) DRAM Controller
    case 0x3e34:  // 8th Generation (Whiskeylake-U) DRAM Controller
    case 0x5904:  // 7th Generation (Kabylake-U) DRAM Controller
    case 0x590c:  // 7th Generation (Kabylake-Y) DRAM Controller
    case 0x8a12:  // 10th Generation (Icelake-U) DRAM Controller
    case 0x9b51:  // 9th Generation (Cometlake-U) DRAM Controller
    case 0x9b61:  // 9th Generation (Cometlake-U) DRAM Controller
    case 0x9b71:  // 9th Generation (Cometlake-U) DRAM Controller
      // Next Generation Core processors (PAM 0x80-0x86)
      mPamRegStart = 0x80;
      break;

    /* Add back support for NVIDIA chipsets? */
    default:
      // Unknown chipset
      break;
  }

  return mPamRegStart != 0 ? EFI_SUCCESS : EFI_NOT_FOUND;
}

/**
  Initialize Legacy Region support

  @retval EFI_SUCCESS   Successfully initialized

**/
EFI_STATUS
LegacyRegionInit (
  VOID
  )
{
  EFI_STATUS  Status;
  UINT16      HostBridgeVenId;

  //
  // Query Host Bridge DeviceId to determine platform type, then set device number
  //
  HostBridgeVenId = PciRead16 (PCI_LIB_ADDRESS (0, 0, 0, PCI_VENDOR_ID_OFFSET));
  if (HostBridgeVenId != 0x8086) {
    DEBUG ((DEBUG_ERROR, "Unknown Host Bridge Vendor ID: 0x%04x\n", HostBridgeVenId));
    DEBUG ((DEBUG_ERROR, "Only Intel platforms are supported!\n"));
    return EFI_UNSUPPORTED;
  }

  Status = DetectChipset ();
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Install the Legacy Region Protocol on a new handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mHandle,
                  &gEfiLegacyRegion2ProtocolGuid, &mLegacyRegion2,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

