/** @file
  Legacy Region Support for Intel platforms

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "LegacyRegion.h"

//
// Intel PAM map.
//
// PAM Range       Offset Bits  Operation
// ===============  ====  ====  ===============================================================
// 0xC0000-0xC3FFF  0x81  1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xC4000-0xC7FFF  0x81  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xC8000-0xCBFFF  0x82  1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xCC000-0xCFFFF  0x82  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xD0000-0xD3FFF  0x83  1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xD4000-0xD7FFF  0x83  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xD8000-0xDBFFF  0x84  1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xDC000-0xDFFFF  0x84  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xE0000-0xE3FFF  0x85  1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xE4000-0xE7FFF  0x85  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xE8000-0xEBFFF  0x86  1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xEC000-0xEFFFF  0x86  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xF0000-0xFFFFF  0x80  5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
//
STATIC LEGACY_MEMORY_SECTION_INFO  mSectionArrayIntel[] = {
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
  {0xF0000, SIZE_64KB, FALSE, FALSE},
};

STATIC INTEL_SEGMENT_REGISTER_VALUE  mRegisterValuesIntel[] = {
  {DRAMC_REGISTER (MCH_PAM1), 0x01, 0x02},
  {DRAMC_REGISTER (MCH_PAM1), 0x10, 0x20},
  {DRAMC_REGISTER (MCH_PAM2), 0x01, 0x02},
  {DRAMC_REGISTER (MCH_PAM2), 0x10, 0x20},
  {DRAMC_REGISTER (MCH_PAM3), 0x01, 0x02},
  {DRAMC_REGISTER (MCH_PAM3), 0x10, 0x20},
  {DRAMC_REGISTER (MCH_PAM4), 0x01, 0x02},
  {DRAMC_REGISTER (MCH_PAM4), 0x10, 0x20},
  {DRAMC_REGISTER (MCH_PAM5), 0x01, 0x02},
  {DRAMC_REGISTER (MCH_PAM5), 0x10, 0x20},
  {DRAMC_REGISTER (MCH_PAM6), 0x01, 0x02},
  {DRAMC_REGISTER (MCH_PAM6), 0x10, 0x20},
  {DRAMC_REGISTER (MCH_PAM0), 0x10, 0x20},
};

EFI_STATUS
LegacyRegionManipulationIntel (
  IN  UINT32   Start,
  IN  UINT32   Length,
  IN  BOOLEAN  *ReadEnable,
  IN  BOOLEAN  *WriteEnable,
  OUT UINT32   *Granularity
  )
{
  UINT32  EndAddress;
  UINTN   Index;
  UINTN   StartIndex;

  //
  // Validate input parameters.
  //
  if (Length == 0 || Granularity == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EndAddress = Start + Length - 1;
  if (Start < PAM_BASE_ADDRESS || EndAddress > PAM_LIMIT_ADDRESS) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Loop to find the start PAM.
  //
  StartIndex = 0;
  for (Index = 0; Index < ARRAY_SIZE (mSectionArrayIntel); Index++) {
    if ((Start >= mSectionArrayIntel[Index].Start) && (Start < (mSectionArrayIntel[Index].Start + mSectionArrayIntel[Index].Length))) {
      StartIndex = Index;
      break;
    }
  }

  ASSERT (Index < ARRAY_SIZE (mSectionArrayIntel));

  //
  // Program PAM until end PAM is encountered
  //
  for (Index = StartIndex; Index < ARRAY_SIZE (mSectionArrayIntel); Index++) {
    if (ReadEnable != NULL) {
      if (*ReadEnable) {
        PciOr8 (
          mRegisterValuesIntel[Index].RegAddress,
          mRegisterValuesIntel[Index].ReadEnableData
          );
      } else {
        PciAnd8 (
          mRegisterValuesIntel[Index].RegAddress,
          (UINT8) (~mRegisterValuesIntel[Index].ReadEnableData)
          );
      }
    }
    if (WriteEnable != NULL) {
      if (*WriteEnable) {
        PciOr8 (
          mRegisterValuesIntel[Index].RegAddress,
          mRegisterValuesIntel[Index].WriteEnableData
          );
      } else {
        PciAnd8 (
          mRegisterValuesIntel[Index].RegAddress,
          (UINT8) (~mRegisterValuesIntel[Index].WriteEnableData)
          );
      }
    }

    //
    // If the end PAM is encountered, record its length as granularity and jump out.
    //
    if ((EndAddress >= mSectionArrayIntel[Index].Start) && (EndAddress < (mSectionArrayIntel[Index].Start + mSectionArrayIntel[Index].Length))) {
      *Granularity = mSectionArrayIntel[Index].Length;
      break;
    }
  }

  ASSERT (Index < ARRAY_SIZE (mSectionArrayIntel));

  return EFI_SUCCESS;
}

EFI_STATUS
LegacyRegionGetInfoIntel (
  OUT UINT32                      *DescriptorCount,
  OUT LEGACY_MEMORY_SECTION_INFO  **Descriptor
  )
{
  UINTN  Index;
  UINT8  PamValue;

  //
  // Check input parameters
  //
  if (DescriptorCount == NULL || Descriptor == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Fill in current status of legacy region.
  //
  *DescriptorCount = sizeof (mSectionArrayIntel) / sizeof (mSectionArrayIntel[0]);
  for (Index = 0; Index < *DescriptorCount; Index++) {
    PamValue = PciRead8 (mRegisterValuesIntel[Index].RegAddress);
    mSectionArrayIntel[Index].ReadEnabled = FALSE;
    if ((PamValue & mRegisterValuesIntel[Index].ReadEnableData) != 0) {
      mSectionArrayIntel[Index].ReadEnabled = TRUE;
    }
    mSectionArrayIntel[Index].WriteEnabled = FALSE;
    if ((PamValue & mRegisterValuesIntel[Index].WriteEnableData) != 0) {
      mSectionArrayIntel[Index].WriteEnabled = TRUE;
    }
  }

  *Descriptor = mSectionArrayIntel;
  return EFI_SUCCESS;
}

/**
  Initialise the Legacy Region protocol for Intel platforms.

**/
VOID
EFIAPI
LegacyRegionInitialiseIntel (
  VOID
  )
{

}
