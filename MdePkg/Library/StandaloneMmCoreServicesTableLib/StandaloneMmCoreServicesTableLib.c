/** @file
  MM Core MM Services Table Library.

  Copyright (c) 2023, 9elements GmbH<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiMm.h>
#include <Library/MmServicesTableLib.h>
#include <Library/DebugLib.h>

EFI_MM_SYSTEM_TABLE         *gMmst = NULL;
extern EFI_MM_SYSTEM_TABLE  gMmCoreMmst;

/**
  The constructor function caches the pointer of the MM Services Table.

  @param  ImageHandle     The firmware allocated handle for the EFI image.
  @param  MmSystemTable   A pointer to the MM System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
StandaloneMmCoreServicesTableLibConstructor (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_MM_SYSTEM_TABLE  *MmSystemTable
  )
{
  gMmst = &gMmCoreMmst;
  return EFI_SUCCESS;
}
