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
#include <Guid/TcgPhysicalPresenceGuid.h>
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
  Find the Tcg Physical Presence store information

  @param  PPIInfo       Pointer to the TCG_PHYSICAL_PRESENCE_INFO structure

  @retval RETURN_SUCCESS     Successfully find the SMM store buffer information.
  @retval RETURN_NOT_FOUND   Failed to find the SMM store buffer information .

**/
EFI_STATUS
EFIAPI
ParseTPMPPIInfo (
  OUT TCG_PHYSICAL_PRESENCE_INFO  *PPIInfo
  )
{
  struct cb_tpm_physical_presence  *CbTPPRec;
  UINT8                            VersionMajor;
  UINT8                            VersionMinor;

  if (PPIInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CbTPPRec = FindCbTag (CB_TAG_TPM_PPI_HANDOFF);
  if (CbTPPRec == NULL) {
    return EFI_NOT_FOUND;
  }

  VersionMajor = CbTPPRec->ppi_version >> 4;
  VersionMinor = CbTPPRec->ppi_version & 0xF;

  DEBUG ((DEBUG_INFO, "Found Tcg Physical Presence information\n"));
  DEBUG ((DEBUG_INFO, "PpiAddress: 0x%x\n", CbTPPRec->ppi_address));
  DEBUG ((DEBUG_INFO, "TpmVersion: 0x%x\n", CbTPPRec->tpm_version));
  DEBUG ((DEBUG_INFO, "PpiVersion: %x.%x\n", VersionMajor, VersionMinor));

  PPIInfo->PpiAddress = CbTPPRec->ppi_address;
  if (CbTPPRec->tpm_version == LB_TPM_VERSION_TPM_VERSION_1_2) {
    PPIInfo->TpmVersion = UEFIPAYLOAD_TPM_VERSION_1_2;
  } else if (CbTPPRec->tpm_version == LB_TPM_VERSION_TPM_VERSION_2) {
    PPIInfo->TpmVersion = UEFIPAYLOAD_TPM_VERSION_2;
  }
  if (VersionMajor == 1 && VersionMinor >= 3) {
    PPIInfo->PpiVersion = UEFIPAYLOAD_TPM_PPI_VERSION_1_30;
  }

  return EFI_SUCCESS;
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
  EFI_STATUS                  Status;
  TCG_PHYSICAL_PRESENCE_INFO  PhysicalPresenceInfo;
  TCG_PHYSICAL_PRESENCE_INFO  *NewPhysicalPresenceInfo;

  //
  // Create guid hob for Tcg Physical Presence Interface
  //
  Status = ParseTPMPPIInfo (&PhysicalPresenceInfo);
  if (!EFI_ERROR (Status)) {
    NewPhysicalPresenceInfo = BuildGuidHob (&gEfiTcgPhysicalPresenceInfoHobGuid, sizeof (PhysicalPresenceInfo));
    ASSERT (NewPhysicalPresenceInfo != NULL);
    CopyMem (NewPhysicalPresenceInfo, &PhysicalPresenceInfo, sizeof (PhysicalPresenceInfo));
    DEBUG ((DEBUG_INFO, "Created Tcg Physical Presence info hob\n"));
  }

  return EFI_SUCCESS;
}
