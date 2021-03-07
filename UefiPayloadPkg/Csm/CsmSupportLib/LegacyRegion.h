/** @file
  Legacy Region Support

  Copyright (c) 2008 - 2016, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _LEGACY_REGION_DXE_H_
#define _LEGACY_REGION_DXE_H_

#include <PiDxe.h>

#include <Protocol/LegacyRegion2.h>

#include <IndustryStandard/Pci.h>

#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

/*
  Various register numbers and value bits based on the following publications:
  - Intel(R) datasheet 316966-002
  - Intel(R) datasheet 316972-004

  Copyright (C) 2015, Red Hat, Inc.
  Copyright (c) 2014, Gabriel L. Somlo <somlo@cmu.edu>

 */

//
// B/D/F/Type: 0/0/0/PCI
//
#define DRAMC_REGISTER(Offset) PCI_LIB_ADDRESS (0, 0, 0, (Offset))


#define MCH_PAM0                  0x80
#define MCH_PAM1                  0x81
#define MCH_PAM2                  0x82
#define MCH_PAM3                  0x83
#define MCH_PAM4                  0x84
#define MCH_PAM5                  0x85
#define MCH_PAM6                  0x86

#define MSR_AMD_MTRR_FIX4K_C0000          0x00000268
#define MSR_AMD_MTRR_FIX4K_C8000          0x00000269
#define MSR_AMD_MTRR_FIX4K_D0000          0x0000026A
#define MSR_AMD_MTRR_FIX4K_D8000          0x0000026B
#define MSR_AMD_MTRR_FIX4K_E0000          0x0000026C
#define MSR_AMD_MTRR_FIX4K_E8000          0x0000026D
#define MSR_AMD_MTRR_FIX4K_F0000          0x0000026E
#define MSR_AMD_MTRR_FIX4K_F8000          0x0000026F

#define PAM_BASE_ADDRESS   0xc0000
#define PAM_LIMIT_ADDRESS  BASE_1MB

//
// Describes Legacy Region blocks and status.
//
typedef struct {
  UINT32  Start;
  UINT32  Length;
  BOOLEAN ReadEnabled;
  BOOLEAN WriteEnabled;
} LEGACY_MEMORY_SECTION_INFO;

//
// Provides a map of the registers and bits used to set Read/Write access.
//
typedef struct {
  UINTN   RegAddress;
  UINT8   ReadEnableData;
  UINT8   WriteEnableData;
} INTEL_SEGMENT_REGISTER_VALUE;

typedef struct {
  UINT32   RegAddress;
  UINT64   ReadEnableData;
  UINT64   WriteEnableData;
} AMD_SEGMENT_REGISTER_VALUE;

EFI_STATUS
LegacyRegionManipulationAmd (
  IN  UINT32                  Start,
  IN  UINT32                  Length,
  IN  BOOLEAN                 *ReadEnable,
  IN  BOOLEAN                 *WriteEnable,
  OUT UINT32                  *Granularity
  );

EFI_STATUS
LegacyRegionGetInfoAmd (
  OUT UINT32                        *DescriptorCount,
  OUT LEGACY_MEMORY_SECTION_INFO    **Descriptor
  );

/**
  Initialise the Legacy Region protocol for AMD platforms.

**/
VOID
EFIAPI
LegacyRegionInitialiseAmd (
  VOID
  );

EFI_STATUS
LegacyRegionManipulationIntel (
  IN  UINT32                  Start,
  IN  UINT32                  Length,
  IN  BOOLEAN                 *ReadEnable,
  IN  BOOLEAN                 *WriteEnable,
  OUT UINT32                  *Granularity
  );

EFI_STATUS
LegacyRegionGetInfoIntel (
  OUT UINT32                        *DescriptorCount,
  OUT LEGACY_MEMORY_SECTION_INFO    **Descriptor
  );

/**
  Initialise the Legacy Region protocol for Intel platforms.

**/
VOID
EFIAPI
LegacyRegionInitialiseIntel (
  VOID
  );

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
  );

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
  );

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
  IN EFI_LEGACY_REGION2_PROTOCOL          *This,
  IN  UINT32                              Start,
  IN  UINT32                              Length,
  OUT UINT32                              *Granularity
  );

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
  );

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
  );

#endif
