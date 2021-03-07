/** @file
  Platform CSM Support Library

  Copyright (c) 2008 - 2011, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CSM_SUPPORT_LIB_H_
#define _CSM_SUPPORT_LIB_H_

#include <PiDxe.h>

//
// The processor/chipset vendor
//
#define CHIPSET_INVALID   0x00
#define CHIPSET_IS_AMD    0x01
#define CHIPSET_IS_INTEL  0x02

extern UINT8  mChipsetVendor;

/**
  Initialize Legacy Region support

  @retval EFI_SUCCESS   Successfully initialized

**/
EFI_STATUS
EFIAPI
LegacyRegionInstall (
  VOID
  );

/**
  Initialize Legacy Interrupt support

  @retval EFI_SUCCESS   Successfully initialized

**/
EFI_STATUS
EFIAPI
LegacyInterruptInstall (
  VOID
  );

/**
  Initialize Legacy Platform support

  @retval EFI_SUCCESS   Successfully initialized

**/
EFI_STATUS
EFIAPI
LegacyBiosPlatformInstall (
  VOID
  );

#endif
