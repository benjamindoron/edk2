/** @file
  Include all platform specific features which can be customized by IBV/OEM.

Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BlParseLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PlatformSupportLib.h>
#include <Guid/SmmStoreInfoGuid.h>
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
  Find the SmmStore HOB.

  @param  SmmStoreInfo       Pointer to the SMMSTORE_INFO structure

  @retval RETURN_SUCCESS     Successfully find the Smm store buffer information.
  @retval RETURN_NOT_FOUND   Failed to find the Smm store buffer information .
**/
STATIC
RETURN_STATUS
EFIAPI
ParseSmmStoreInfo (
  OUT SMMSTORE_INFO  *SmmStoreInfo
  )
{
  struct cb_smmstorev2  *CbSSRec;

  if (SmmStoreInfo == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  CbSSRec = FindCbTag (CB_TAG_SMMSTOREV2);
  if (CbSSRec == NULL) {
    return RETURN_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "Found Smm Store information\n"));
  DEBUG ((DEBUG_INFO, "block size: 0x%x\n", CbSSRec->block_size));
  DEBUG ((DEBUG_INFO, "number of blocks: 0x%x\n", CbSSRec->num_blocks));
  DEBUG ((DEBUG_INFO, "communication buffer: 0x%x\n", CbSSRec->com_buffer));
  DEBUG ((DEBUG_INFO, "communication buffer size: 0x%x\n", CbSSRec->com_buffer_size));
  DEBUG ((DEBUG_INFO, "MMIO address of store: 0x%x\n", CbSSRec->mmap_addr));

  SmmStoreInfo->ComBuffer     = CbSSRec->com_buffer;
  SmmStoreInfo->ComBufferSize = CbSSRec->com_buffer_size;
  SmmStoreInfo->BlockSize     = CbSSRec->block_size;
  SmmStoreInfo->NumBlocks     = CbSSRec->num_blocks;
  SmmStoreInfo->MmioAddress   = CbSSRec->mmap_addr;
  SmmStoreInfo->ApmCmd        = CbSSRec->apm_cmd;

  return RETURN_SUCCESS;
}

/**
  Parse platform specific information from coreboot.

  @retval RETURN_SUCCESS       The platform specific coreboot support succeeded.
  @retval RETURN_DEVICE_ERROR  The platform specific coreboot support could not be completed.

**/
EFI_STATUS
EFIAPI
ParsePlatformInfo (
  VOID
  )
{
  SMMSTORE_INFO  SmmStoreInfo;
  EFI_STATUS     Status;
  SMMSTORE_INFO  *NewSmmStoreInfo;

  //
  // Create guid hob for SmmStore
  //
  Status = ParseSmmStoreInfo (&SmmStoreInfo);
  if (!EFI_ERROR (Status)) {
    NewSmmStoreInfo = BuildGuidHob (&gEfiSmmStoreInfoHobGuid, sizeof (SmmStoreInfo));
    ASSERT (NewSmmStoreInfo != NULL);
    CopyMem (NewSmmStoreInfo, &SmmStoreInfo, sizeof (SmmStoreInfo));
    DEBUG ((DEBUG_INFO, "Created SmmStore info hob\n"));
  }

  return EFI_SUCCESS;
}
