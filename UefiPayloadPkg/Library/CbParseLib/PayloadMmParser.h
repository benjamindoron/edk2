/** @file
  This file parses the coreboot table in memory to extract
  the required information.

  Copyright (c) 2025, 9elements GmbH.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PAYLOAD_MM_PARSE_H_
#define _PAYLOAD_MM_PARSE_H_

#include <PiPei.h>
#include <Guid/SpiFlashInfoGuid.h>

/**
  Find payload MM feature tables.

  @retval EFI_SUCCESS     Successfully found the payload MM feature tables.
  @retval EFI_NOT_FOUND   Failed to find the payload MM feature tables.
**/
EFI_STATUS
EFIAPI
ParsePayloadMmFeatureInfo (
  VOID
  );

/**
  This API provides a way to build a SPI_FLASH_INFO HOB by
  finding the SPI flash controller at runtime via probing.

  @param  PldSpiFlashInfoPtr          Output parameter for HOB.

  @retval RETURN_SUCCESS              The HOB was generated successfully.
  @retval RETURN_NOT_FOUND            The SPI flash controller was not found.
  @retval RETURN_INVALID_PARAMETER    Parameter is NULL pointer.
**/
EFI_STATUS
EFIAPI
ProbeSpiController (
  OUT SPI_FLASH_INFO  **PldSpiFlashInfoPtr
  );

#endif // _PAYLOAD_MM_PARSE_H_
