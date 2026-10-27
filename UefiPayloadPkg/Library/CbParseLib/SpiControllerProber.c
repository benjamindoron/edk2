/** @file
  This file determines where the SPI flash controller device
  is located and generates a SPI_FLASH_INFO HOB accordingly.

  Copyright (c) 2026, 9elements GmbH.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <IndustryStandard/Pci30.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Guid/PayloadMmInterfaceInfoGuid.h>
#include <Guid/SpiFlashInfoGuid.h>
#include "PayloadMmParser.h"

#define FLAGS_SPI_DISABLE_SMM_WRITE_PROTECT     BIT0

#define PCI_VENDOR_ID_INTEL                     0x8086

#define PCI_DEVICE_INTEL_APL_NB                 0x5af0
#define PCI_DEVICE_INTEL_GLK_NB                 0x31f0

#define SA_PCI_ADDR(Offset)                     (PCI_LIB_ADDRESS (0, 0x00, 0, Offset))
#define PCH_LPC_PCI_ADDR(Offset)                (PCI_LIB_ADDRESS (0, 0x1F, 0, Offset))

STATIC
BOOLEAN
IsIntelPlatform (
  VOID
  )
{
  if (PciRead16 (SA_PCI_ADDR (PCI_VENDOR_ID_OFFSET)) == PCI_VENDOR_ID_INTEL) {
    return PciRead16 (PCH_LPC_PCI_ADDR (PCI_VENDOR_ID_OFFSET)) == PCI_VENDOR_ID_INTEL;
  }
  return FALSE;
}

STATIC
UINT64
CheckDevAndReturnAddr (
  UINT8 Bus,
  UINT8 Dev,
  UINT8 Func,
  UINT16 ExpectedDevId
  )
{
  if (PciRead16 (PCI_LIB_ADDRESS (Bus, Dev, Func, PCI_VENDOR_ID_OFFSET)) != ExpectedDevId) {
    return 0;
  }
  return PCI_LIB_ADDRESS (Bus, Dev, Func, 0);
}

STATIC
UINT64
GetSpiFlashCtrlrAddr_Intel (
  VOID
  )
{
  switch (PciRead16 (SA_PCI_ADDR (PCI_DEVICE_ID_OFFSET))) {
    case PCI_DEVICE_INTEL_APL_NB:
    case PCI_DEVICE_INTEL_GLK_NB:
      return CheckDevAndReturnAddr(0, 0x0D, 2, PCI_VENDOR_ID_INTEL);
    default:
      return CheckDevAndReturnAddr(0, 0x1F, 5, PCI_VENDOR_ID_INTEL);
  }
}

STATIC
EFI_STATUS
ProbeSpiController_Intel (
  OUT SPI_FLASH_INFO  **PldSpiFlashInfoPtr
  )
{
  SPI_FLASH_INFO *PldSpiFlashInfo;
  UINT64 SpiCtrlrAddr;

  SpiCtrlrAddr = GetSpiFlashCtrlrAddr_Intel ();
  if (SpiCtrlrAddr == 0) {
    return EFI_NOT_FOUND;
  }

  PldSpiFlashInfo = BuildGuidHob (&gSpiFlashInfoGuid, sizeof (SPI_FLASH_INFO));
  if (PldSpiFlashInfo != NULL) {
    PldSpiFlashInfo->Revision = 0;
    PldSpiFlashInfo->Flags = FLAGS_SPI_DISABLE_SMM_WRITE_PROTECT; /// @todo: Hardcoded

    PldSpiFlashInfo->SpiAddress.AddressSpaceId    = SPACE_ID_PCI_CONFIGURATION;
    PldSpiFlashInfo->SpiAddress.RegisterBitWidth  = 32;
    PldSpiFlashInfo->SpiAddress.RegisterBitOffset = 0;
    PldSpiFlashInfo->SpiAddress.AccessSize        = REGISTER_BIT_WIDTH_DWORD;
    PldSpiFlashInfo->SpiAddress.Address           = PcdGet64 (PcdPciExpressBaseAddress) + SpiCtrlrAddr;

    DEBUG ((DEBUG_INFO, "Created SPI flash info HOB for Intel\n"));
  }

  if (PldSpiFlashInfoPtr != NULL) {
    *PldSpiFlashInfoPtr = PldSpiFlashInfo;
  }

  return EFI_SUCCESS;
}

/**
  This API provides a way to build a SPI_FLASH_INFO HOB by
  finding the SPI flash controller at runtime via probing.

  @param  PldSpiFlashInfoPtr          Output parameter for HOB.

  @retval RETURN_SUCCESS              The HOB was generated successfully.
  @retval RETURN_NOT_FOUND            The SPI flash controller was not found.
**/
EFI_STATUS
EFIAPI
ProbeSpiController (
  OUT SPI_FLASH_INFO  **PldSpiFlashInfoPtr
  )
{
  if (IsIntelPlatform ()) {
    return ProbeSpiController_Intel (PldSpiFlashInfoPtr);
  }

  ASSERT (FALSE);
  return EFI_NOT_FOUND;
}
