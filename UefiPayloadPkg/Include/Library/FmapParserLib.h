/** @file
  Contains helper functions for parsing the FMAP.

  Copyright (c) 2023, 9elements GmbH.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiMultiPhase.h>
#include <Pi/PiMultiPhase.h>

// TODO: Copy in flags.
#define FMAP_SIGNATURE  "__FMAP__"
#define FMAP_VER_MAJOR  1
#define FMAP_VER_MINOR  1

/**
  Find an FMAP area's address and size.

  @param[in]   FmapAreaName     Name string of FMAP area to find
  @param[out]  FmapAreaAddress  Pointer to return of FMAP area memory address
  @param[out]  FmapAreaSize     Pointer to return of FMAP area size

  @retval EFI_SUCCESS            Successfully found the FMAP area information.
  @retval EFI_INVALID_PARAMETER  Input arguments are invalid.
  @retval EFI_UNSUPPORTED        The FMAP found is unsupported by this library.
  @retval EFI_NOT_FOUND          Failed to find the FMAP area information.

**/
EFI_STATUS
EFIAPI
FmapLocateArea (
  IN  CHAR8                 *FmapAreaName,
  OUT EFI_PHYSICAL_ADDRESS  *FmapAreaAddress,
  OUT UINT32                *FmapAreaSize
  );
