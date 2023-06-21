/** @file
  This file defines the structure of the FMAP HOB.

  Copyright (c) 2025, 9elements GmbH<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef FLASH_REGION_MAP_INFO_GUID_H_
#define FLASH_REGION_MAP_INFO_GUID_H_

#include <Base.h>

///
/// coreboot FMAP HOB GUID
///
extern EFI_GUID  gEfiFlashRegionMapInfoHobGuid;

#define FMAP_STRLEN  32         /* includes null-terminator */
typedef struct {
  UINT32  Offset;                /* offset relative to base */
  UINT32  Size;                  /* size in bytes */
  UINT8   Name[FMAP_STRLEN];     /* descriptive name */
  UINT16  Flags;                 /* flags for this area */
} FMAP_AREA;

typedef struct {
  UINT8   Signature[8];       /* "__FMAP__" (0x5F5F464D41505F5F) */
  UINT8   MajorVersion;       /* major version */
  UINT8   MinorVersion;       /* minor version */
  UINT64  Base;               /* address of the firmware binary */
  UINT32  Size;               /* size of firmware binary in bytes */
  UINT8   Name[FMAP_STRLEN];  /* name of this firmware binary */
  UINT16  NumberOfAreas;      /* number of areas described by
                                fmap_areas[] below */
  FMAP_AREA  Areas[];
} FLASH_REGION_MAP_INFO;

#endif
