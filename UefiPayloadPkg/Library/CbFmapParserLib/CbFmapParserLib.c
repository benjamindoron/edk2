/** @file
  Contains helper functions for parsing the FMAP.

  Copyright (c) 2025, 9elements GmbH.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/FmapParserLib.h>
#include <Library/HobLib.h>
#include <Guid/FlashRegionMapInfoGuid.h>
#include <Guid/SpiFlashWindowInfoGuid.h>

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
  )
{
  EFI_HOB_GUID_TYPE       *GuidHob;
  FLASH_REGION_MAP_INFO   *Fmap;
  SPI_FLASH_WINDOW_INFO   *SpiFlashWindows;
  FMAP_AREA               *FmapArea;
  UINTN                   FmapIndex;
  FLASH_MMAP_WINDOW       *SpiFlashWindow;
  UINTN                   WindowIndex;
  UINT32                  FlashWindowBase;
  UINT32                  SubtractFlashWindowOffset;

  //
  // Perform basic validation
  //
  if (FmapAreaName == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // FMAP_STRLEN includes the sizeof of the NULL-terminator, so that's also too long
  if (AsciiStrLen (FmapAreaName) >= FMAP_STRLEN) {
    return EFI_INVALID_PARAMETER;
  }

  SpiFlashWindows = NULL;

  GuidHob = GetFirstGuidHob (&gEfiSpiFlashWindowInfoHobGuid);
  if (GuidHob != NULL) {
    SpiFlashWindows = GET_GUID_HOB_DATA (GuidHob);
  }

  GuidHob = GetFirstGuidHob (&gEfiFlashRegionMapInfoHobGuid);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }

  Fmap = GET_GUID_HOB_DATA (GuidHob);

  //
  // Perform FMAP validation
  //
  if ((AsciiStrnCmp ((CHAR8 *)Fmap->Signature, FMAP_SIGNATURE, 8) != 0)
      || (Fmap->MajorVersion != FMAP_VER_MAJOR) || (Fmap->MinorVersion != FMAP_VER_MINOR)) {
    return EFI_UNSUPPORTED;
  }

  if (FmapAreaAddress != NULL) {
    *FmapAreaAddress = 0;
  }

  if (FmapAreaSize != NULL) {
    *FmapAreaSize = 0;
  }

  for (FmapIndex = 0; FmapIndex < Fmap->NumberOfAreas; FmapIndex++) {
    FmapArea = &Fmap->Areas[FmapIndex];
    if (AsciiStrCmp ((CHAR8 *)FmapArea->Name, FmapAreaName) == 0) {
      // Tell the user the offset plainly, it's more readable
      DEBUG ((
        DEBUG_INFO,
        "FMAP: Found area \"%a\" at offset 0x%x (size 0x%0x)\n",
        FmapAreaName,
        FmapArea->Offset,
        FmapArea->Size
      ));

      // Pass the memory-mapped address back to the caller
      SubtractFlashWindowOffset = 0;
      if (SpiFlashWindows != NULL) {
        for (WindowIndex = 0; WindowIndex < SpiFlashWindows->MmapCount; WindowIndex++) {
          SpiFlashWindow = &SpiFlashWindows->MmapTable[WindowIndex];

          // TODO? Assumes that flash windows are sorted and that an FMAP region is strictly within one SPI window.
          SubtractFlashWindowOffset += SpiFlashWindow->FlashBase;
          if ((FmapArea->Offset >= SpiFlashWindow->FlashBase)
              && (FmapArea->Offset + FmapArea->Size <= SpiFlashWindow->FlashBase + SpiFlashWindow->Size)) {
            FlashWindowBase = SpiFlashWindow->HostBase;
            break;
          }
        }

        if (WindowIndex >= SpiFlashWindows->MmapCount) {
          return EFI_NOT_FOUND;
        }
      } else {
        DEBUG ((DEBUG_WARN, "gEfiSpiFlashWindowInfoHobGuid not found, assuming SPI flash at top of address space\n"));
        FlashWindowBase = 0x100000000ULL - Fmap->Size;
      }

      if (FmapAreaAddress != NULL) {
        *FmapAreaAddress = FlashWindowBase + FmapArea->Offset - SubtractFlashWindowOffset;
      }

      if (FmapAreaSize != NULL) {
        *FmapAreaSize = FmapArea->Size;
      }

      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}
