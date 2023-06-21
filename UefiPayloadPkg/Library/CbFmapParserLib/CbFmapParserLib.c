/** @file
  Contains helper functions for parsing FMAP.

  Copyright (c) 2023, 9elements GmbH.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BlParseLib.h>
#include <Library/DebugLib.h>
#include <Library/FmapParserLib.h>
#include <Coreboot.h>

/**
  Find coreboot record with given Tag.
  NOTE: This coreboot-specific function definition is absent
        from the common BlParseLib header.

  @param  Tag                The tag id to be found

  @retval NULL              The Tag is not found.
  @retval Others            The pointer to the record found.

**/
VOID *
FindCbTag (
  IN  UINT32  Tag
  );

/**
  Find a requested FMAP area's address and size.

  @param[in]   FmapAreaName     Name string of FMAP area to find
  @param[out]  FmapAreaAddress  Pointer to return of FMAP area memory address
  @param[out]  FmapAreaSize     Pointer to return of FMAP area size

  @retval EFI_SUCCESS            Successfully found the FMAP area information.
  @retval EFI_INVALID_PARAMETER  Input arguments are invalid.
  @retval EFI_NOT_FOUND          Failed to find the FMAP area information.

**/
EFI_STATUS
EFIAPI
FmapLocateArea (
  IN     CHAR8                 *FmapAreaName,
  IN OUT EFI_PHYSICAL_ADDRESS  *FmapAreaAddress,
  IN OUT UINT32                *FmapAreaSize
  )
{
  struct cb_cbmem_ref  *CbMemRef;
  struct fmap          *Fmap;
  UINTN                Index;

  //
  // Perform basic validation
  //
  if ((FmapAreaName == NULL) || (FmapAreaAddress == NULL) || (FmapAreaSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  // FMAP_STRLEN includes sizeof NULL-terminator, so this also too long
  if (AsciiStrLen (FmapAreaName) >= FMAP_STRLEN) {
    return EFI_INVALID_PARAMETER;
  }

  // The coreboot table contains large structures as references
  CbMemRef = FindCbTag (CB_TAG_FMAP);
  if (CbMemRef == NULL) {
    return EFI_NOT_FOUND;
  }

  Fmap = (VOID *)(UINTN)CbMemRef->cbmem_addr;  // Support PEI and DXE
  if (Fmap == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Perform FMAP validation
  //
  if ((AsciiStrnCmp ((CHAR8 *)Fmap->signature, FMAP_SIGNATURE, 8) != 0)
      || (Fmap->ver_major != FMAP_VER_MAJOR) || (Fmap->ver_minor != FMAP_VER_MINOR)) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < Fmap->nareas; Index++) {
    if (AsciiStrCmp ((CHAR8 *)Fmap->areas[Index].name, FmapAreaName) == 0) {
      DEBUG ((
        DEBUG_INFO,
        "FMAP: Found area \"%a\" at offset 0x%x (size 0x%0x)\n",
        FmapAreaName,
        Fmap->areas[Index].offset,
        Fmap->areas[Index].size
      ));

      *FmapAreaAddress = Fmap->base + Fmap->areas[Index].offset;
      *FmapAreaSize = Fmap->areas[Index].size;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}
