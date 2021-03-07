/** @file
  Legacy Region Support

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>
#include "CsmSupportLib.h"
#include "LegacyRegion.h"

//
// Handle for the Legacy Region Protocol instance produced by this driver
//
STATIC EFI_HANDLE  mLegacyRegion2Handle = NULL;

//
// The Legacy Region Protocol instance produced by this driver
//
STATIC EFI_LEGACY_REGION2_PROTOCOL  mLegacyRegion2 = {
  LegacyRegion2Decode,
  LegacyRegion2Lock,
  LegacyRegion2BootLock,
  LegacyRegion2Unlock,
  LegacyRegionGetInfo
};

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
  if (mChipsetVendor == CHIPSET_IS_INTEL) {
    return LegacyRegionManipulationIntel (Start, Length, On, NULL, Granularity);
  }

  if (mChipsetVendor == CHIPSET_IS_AMD) {
    return LegacyRegionManipulationAmd (Start, Length, On, NULL, Granularity);
  }

  return EFI_UNSUPPORTED;
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
  IN  EFI_LEGACY_REGION2_PROTOCOL  *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity
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
  IN  EFI_LEGACY_REGION2_PROTOCOL  *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity
  )
{
  BOOLEAN  WriteEnable;

  WriteEnable = FALSE;

  if (mChipsetVendor == CHIPSET_IS_INTEL) {
    return LegacyRegionManipulationIntel (Start, Length, NULL, &WriteEnable, Granularity);
  }

  if (mChipsetVendor == CHIPSET_IS_AMD) {
    return LegacyRegionManipulationAmd (Start, Length, NULL, &WriteEnable, Granularity);
  }

  return EFI_UNSUPPORTED;
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

  if (mChipsetVendor == CHIPSET_IS_INTEL) {
    return LegacyRegionManipulationIntel (Start, Length, NULL, &WriteEnable, Granularity);
  }

  if (mChipsetVendor == CHIPSET_IS_AMD) {
    return LegacyRegionManipulationAmd (Start, Length, NULL, &WriteEnable, Granularity);
  }

  return EFI_UNSUPPORTED;
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
  EFI_STATUS                   Status;
  LEGACY_MEMORY_SECTION_INFO   *SectionInfo;
  UINT32                       SectionCount;
  EFI_LEGACY_REGION_DESCRIPTOR *DescriptorArray;
  UINTN                        Index;
  UINTN                        DescriptorIndex;

  //
  // Get section numbers and information
  //
  if (mChipsetVendor == CHIPSET_IS_INTEL) {
    Status = LegacyRegionGetInfoIntel (&SectionCount, &SectionInfo);
  } else if (mChipsetVendor == CHIPSET_IS_AMD) {
    Status = LegacyRegionGetInfoAmd (&SectionCount, &SectionInfo);
  }

  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Each section has 3 descriptors, corresponding to readability, writeability, and lock status.
  //
  DescriptorArray = AllocatePool (sizeof (EFI_LEGACY_REGION_DESCRIPTOR) * SectionCount * 3);
  if (DescriptorArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  DescriptorIndex = 0;
  for (Index = 0; Index < SectionCount; Index++) {
    //
    // Create descriptor for readability, according to lock status
    //
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
  Initialize Legacy Region support

  @retval EFI_SUCCESS   Successfully initialized

**/
EFI_STATUS
EFIAPI
LegacyRegionInstall (
  VOID
  )
{
  EFI_STATUS  Status;

  //
  // Make sure the Legacy Region Protocol is not already installed in the system
  //
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiLegacyRegion2ProtocolGuid);

  if (mChipsetVendor == CHIPSET_IS_INTEL) {
    LegacyRegionInitialiseIntel ();
  } else if (mChipsetVendor == CHIPSET_IS_AMD) {
    LegacyRegionInitialiseAmd ();
  }

  //
  // Install the Legacy Region Protocol on a new handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mLegacyRegion2Handle,
                  &gEfiLegacyRegion2ProtocolGuid,
                  &mLegacyRegion2,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
