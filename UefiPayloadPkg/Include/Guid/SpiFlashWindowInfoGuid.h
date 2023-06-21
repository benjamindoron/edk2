/** @file
  This file defines the structure of the SPI Flash Window info HOB.

  Copyright (c) 2025, 9elements GmbH<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef SPI_FLASH_WINDOW_INFO_GUID_H_
#define SPI_FLASH_WINDOW_INFO_GUID_H_

#include <Base.h>

///
/// coreboot SPI Flash Window info GUID
///
extern EFI_GUID  gEfiSpiFlashWindowInfoHobGuid;

/* Memory map windows to translate addresses between SPI flash space and host address space. */
typedef struct {
  UINT32  FlashBase;
  UINT32  HostBase;
  UINT32  Size;
} FLASH_MMAP_WINDOW;

typedef struct {
  UINT32  FlashSize;
  UINT32  SectorSize;
  /*
   * Note: `erase_cmd` was previously a uint32_t. It's now uint8_t because only
   * the lowest byte was used, ensuring backward compatibility with older coreboot
   * tables and allowing reuse of the remaining bytes.
   */
  UINT8   EraseCmd;
#define LB_SPI_FLASH_FLAG_IN_4BYTE_ADDR_MODE    (1 << 0)
  UINT8   Flags;
  UINT16  Reserved;
  /*
   * Number of mmap windows used by the platform to decode addresses between SPI flash
   * space and host address space. This determines the number of entries in mmap_table.
   */
  UINT32  MmapCount;
  FLASH_MMAP_WINDOW  MmapTable[];
} SPI_FLASH_WINDOW_INFO;

#endif
