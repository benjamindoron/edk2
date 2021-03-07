/** @file
  Platform CSM Support Library

  Copyright (c) 2008 - 2011, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include "CsmSupportLib.h"

UINT8  mChipsetVendor = CHIPSET_INVALID;

/**
  The constructor function for the platform CSM support library

  @retval EFI_SUCCESS   The constructor always returns RETURN_SUCCESS.

**/
EFI_STATUS
EFIAPI
CsmSupportLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINT16  HostBridgeVenId;

  //
  // Query Host Bridge VID to determine platform type
  //
  HostBridgeVenId = PciRead16 (PCI_LIB_ADDRESS (0, 0, 0, 0));
  switch (HostBridgeVenId) {
    /* Intel */
    case 0x8086:
      DEBUG ((DEBUG_INFO, "%a(): Intel Host Bridge found\n", __FUNCTION__));
      mChipsetVendor = CHIPSET_IS_INTEL;
      break;
    /* AMD */
    case 0x1022:
      DEBUG ((DEBUG_INFO, "%a(): AMD Host Bridge found\n", __FUNCTION__));
      mChipsetVendor = CHIPSET_IS_AMD;
      break;
    default:
      DEBUG ((DEBUG_ERROR, "%a(): Unknown Host Bridge found. Vendor ID: 0x%04x\n",
              __FUNCTION__, HostBridgeVenId));
      ASSERT (FALSE);
      break;
  }

  LegacyRegionInstall ();

  LegacyInterruptInstall ();

  LegacyBiosPlatformInstall ();

  return EFI_SUCCESS;
}
