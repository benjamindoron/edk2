/** @file
  This file parses the coreboot table in memory to extract
  the required information.

  Copyright (c) 2025, 9elements GmbH.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PAYLOAD_MM_PARSE_H_
#define _PAYLOAD_MM_PARSE_H_

#include <PiPei.h>

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

#endif // _PAYLOAD_MM_PARSE_H_
