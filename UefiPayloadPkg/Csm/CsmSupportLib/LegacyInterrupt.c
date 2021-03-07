/** @file
  Legacy Interrupt Support

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "LegacyInterrupt.h"

//
// Handle for the Legacy Interrupt Protocol instance produced by this driver
//
STATIC EFI_HANDLE mLegacyInterruptHandle = NULL;

//
// Legacy Interrupt Device number (0x01 on piix4, 0x1f on q35/mch)
//
STATIC UINT8      mLegacyInterruptDevice = 0;

//
// The Legacy Interrupt Protocol instance produced by this driver
//
STATIC EFI_LEGACY_INTERRUPT_PROTOCOL mLegacyInterrupt = {
  GetNumberPirqs,
  GetLocation,
  ReadPirq,
  WritePirq
};

//
// Legacy Interrupt configuration
//
STATIC UINT8 IsPchWithP2Sb = 0;
STATIC UINT8 PirqReg[MAX_PIRQ_NUMBER] = { PIRQA, PIRQB, PIRQC, PIRQD, PIRQE, PIRQF, PIRQG, PIRQH };


/**
  Return the number of PIRQs supported by this chipset.

  @param[in]  This         Pointer to LegacyInterrupt Protocol
  @param[out] NumberPirqs  The pointer to return the max IRQ number supported

  @retval EFI_SUCCESS   Max PIRQs successfully returned

**/
EFI_STATUS
EFIAPI
GetNumberPirqs (
  IN  EFI_LEGACY_INTERRUPT_PROTOCOL  *This,
  OUT UINT8                          *NumberPirqs
  )
{
  *NumberPirqs = MAX_PIRQ_NUMBER;

  return EFI_SUCCESS;
}


/**
  Return PCI location of this device.
  $PIR table requires this info.

  @param[in]   This                - Protocol instance pointer.
  @param[out]  Bus                 - PCI Bus
  @param[out]  Device              - PCI Device
  @param[out]  Function            - PCI Function

  @retval  EFI_SUCCESS   Bus/Device/Function returned

**/
EFI_STATUS
EFIAPI
GetLocation (
  IN  EFI_LEGACY_INTERRUPT_PROTOCOL  *This,
  OUT UINT8                          *Bus,
  OUT UINT8                          *Device,
  OUT UINT8                          *Function
  )
{
  *Bus      = LEGACY_INT_BUS;
  *Device   = mLegacyInterruptDevice;
  *Function = LEGACY_INT_FUNC;

  return EFI_SUCCESS;
}


/**
  Builds the PCI configuration address for the register specified by PirqNumber

  @param[in]  PirqNumber - The PIRQ number to build the PCI configuration address for

  @return  The PCI Configuration address for the PIRQ
**/
UINTN
GetAddress (
  UINT8  PirqNumber
  )
{
  if (IsPchWithP2Sb) {
    return (PCR_ADDR | (APIC_PID << P2SB_PORTID_SHIFT) | (PIRQ_OFFSET + PirqNumber));
  } else {
    return PCI_LIB_ADDRESS(
            LEGACY_INT_BUS,
            mLegacyInterruptDevice,
            LEGACY_INT_FUNC,
            PirqReg[PirqNumber]
            );
  }
}

/**
  Read the given PIRQ register

  @param[in]  This        Protocol instance pointer
  @param[in]  PirqNumber  The Pirq register 0 = A, 1 = B etc
  @param[out] PirqData    Value read

  @retval EFI_SUCCESS   Decoding change affected.
  @retval EFI_INVALID_PARAMETER   Invalid PIRQ number

**/
EFI_STATUS
EFIAPI
ReadPirq (
  IN  EFI_LEGACY_INTERRUPT_PROTOCOL  *This,
  IN  UINT8                          PirqNumber,
  OUT UINT8                          *PirqData
  )
{
  if (PirqNumber >= MAX_PIRQ_NUMBER) {
    return EFI_INVALID_PARAMETER;
  }

  /* TODO: Unhide P2SB? */
  *PirqData = PciRead8 (GetAddress (PirqNumber));
  *PirqData = (UINT8) (*PirqData & 0x7f);

  return EFI_SUCCESS;
}


/**
  Write the given PIRQ register

  @param[in]  This        Protocol instance pointer
  @param[in]  PirqNumber  The Pirq register 0 = A, 1 = B etc
  @param[out] PirqData    Value to write

  @retval EFI_SUCCESS   Decoding change affected.
  @retval EFI_INVALID_PARAMETER   Invalid PIRQ number

**/
EFI_STATUS
EFIAPI
WritePirq (
  IN  EFI_LEGACY_INTERRUPT_PROTOCOL  *This,
  IN  UINT8                          PirqNumber,
  IN  UINT8                          PirqData
  )
{
  if (PirqNumber >= MAX_PIRQ_NUMBER) {
    return EFI_INVALID_PARAMETER;
  }

  /* TODO: Unhide P2SB? */
  PciWrite8 (GetAddress (PirqNumber), PirqData);
  return EFI_SUCCESS;
}

/**
  Detects chipset and sets interrupt device

  @retval EFI_SUCCESS   Successfully initialized

**/
STATIC
EFI_STATUS
DetectChipset (
  VOID
  )
{
  UINT16 mDeviceId = PciRead16 (PCI_LIB_ADDRESS(0, 0, 0, PCI_DEVICE_ID_OFFSET));

  switch (mDeviceId) {

    //
    // Copied from 915 resolution created by steve tomljenovic,
    // Resolution module by Evan Lojewski
    //
    case 0x3575:  // 830
    case 0x3580:  // 855GM
      // Intel 830 and similar
//    mLegacyInterruptDevice
      break;

    case 0x25C0:  // 5000
    case 0x25D4:  // 5000V
    case 0x65C0:  // 5100
      // Intel 5000 and similar
//    mLegacyInterruptDevice
      break;

    //
    // Copied from 915 resolution created by steve tomljenovic,
    // Resolution module by Evan Lojewski
    //
    case 0x2560:  // 845G
    case 0x2570:  // 865G
    case 0x2580:  // 915G
    case 0x2590:  // 915GM
    case 0x2770:  // 945G
    case 0x2774:  // 955X
    case 0x277c:  // 975X
    case 0x27a0:  // 945GM
    case 0x27ac:  // 945GME
    case 0x2920:  // G45
    case 0x2970:  // 946GZ
    case 0x2980:  // G965
    case 0x2990:  // Q965
    case 0x29a0:  // P965
    case 0x29b0:  // R845
    case 0x29c0:  // G31/P35
    case 0x29d0:  // Q33
    case 0x29e0:  // X38/X48
    case 0x2a00:  // 965GM
    case 0x2a10:  // GME965/GLE960
    case 0x2a40:  // PM/GM45/47
    case 0x2e00:  // Eaglelake
    case 0x2e10:  // B43
    case 0x2e20:  // P45
    case 0x2e30:  // G41
    case 0x2e40:  // B43 Base
    case 0x2e90:  // B43 Soft Sku
    case 0x8100:  // 500
    case 0xA000:  // 3150
      // Intel Series 4 and similar
      mLegacyInterruptDevice = LEGACY_INT_DEV_Q35;
      break;

    //
    // 1st gen i7 - Nehalem
    //
    case 0x0040:  // Core Processor DRAM Controller
    case 0x0044:  // Core Processor DRAM Controller - Arrandale
    case 0x0048:  // Core Processor DRAM Controller
    case 0x0069:  // Core Processor DRAM Controller
    case 0xD130:  // Xeon(R) CPU L3426 Processor DRAM Controller
    case 0xD131:  // Core-i Processor DRAM Controller
    case 0xD132:  // PM55 i7-720QM  DRAM Controller
    case 0x3400:  // Core-i Processor DRAM Controller
    case 0x3401:  // Core-i Processor DRAM Controller
    case 0x3402:  // Core-i Processor DRAM Controller
    case 0x3403:  // Core-i Processor DRAM Controller
    case 0x3404:  // Core-i Processor DRAM Controller
    case 0x3405:  // X58 Core-i Processor DRAM Controller
    case 0x3406:  // Core-i Processor DRAM Controller
    case 0x3407:  // Core-i Processor DRAM Controller
    case 0x3C00:  // Xeon E5 Processor
      // Core i7 processors
      mLegacyInterruptDevice = LEGACY_INT_DEV_Q35;
      break;

    //
    // Core processors
    // http://pci-ids.ucw.cz/read/PC/8086
    //
    case 0x0100:  // 2nd Generation Core Processor Family DRAM Controller
    case 0x0104:  // 2nd Generation Core Processor Family DRAM Controller
    case 0x0108:  // Xeon E3-1200 2nd Generation Core Processor Family DRAM Controller
    case 0x010c:  // Xeon E3-1200 2nd Generation Core Processor Family DRAM Controller
    case 0x0150:  // 3rd Generation Core Processor Family DRAM Controller
    case 0x0154:  // 3rd Generation Core Processor Family DRAM Controller
    case 0x0158:  // 3rd Generation Core Processor Family DRAM Controller
    case 0x015c:  // 3rd Generation Core Processor Family DRAM Controller
    case 0x0160:  // 3rd Generation Core Processor Family DRAM Controller
    case 0x0164:  // 3rd Generation Core Processor Family DRAM Controller
    case 0x0A04:  // 4rd Generation U-Processor Series
    case 0x0C00:  // 4rd Generation Core Processor Family DRAM Controller
    case 0x0C04:  // 4rd Generation M-Processor Series
    case 0x0C08:  // 4rd Generation Haswell Xeon
    case 0x0D04:  // 4rd Generation H-Processor Series (BGA) with GT3 Graphics
    case 0x0F00:  // Bay Trail Family DRAM Controller
    case 0x1604:  // 5th Generation Core Processor Family DRAM Controller
      // Next Generation Core processors
      mLegacyInterruptDevice = LEGACY_INT_DEV_Q35;
      break;

    //
    // Core processors
    // http://pci-ids.ucw.cz/read/PC/8086
    //
    case 0x1904:  // 6th Generation (Skylake-U) DRAM Controller
    case 0x190f:  // 6th Generation (Skylake) DRAM Controller (Desktop)
    case 0x190c:  // 6th Generation (Skylake-Y) DRAM Controller
    case 0x1910:  // 6th Generation (Skylake-M) DRAM Controller
    case 0x1918:  // 6th Generation (Skylake) DRAM Controller (Workstation)
    case 0x191f:  // 6th Generation (Skylake) DRAM Controller (Z170X)
    case 0x3ed0:  // 8th Generation (Coffeelake-U) DRAM Controller
    case 0x3e34:  // 8th Generation (Whiskeylake-U) DRAM Controller
    case 0x5904:  // 7th Generation (Kabylake-U) DRAM Controller
    case 0x590c:  // 7th Generation (Kabylake-Y) DRAM Controller
    case 0x8a12:  // 10th Generation (Icelake-U) DRAM Controller
    case 0x9b51:  // 9th Generation (Cometlake-U) DRAM Controller
    case 0x9b61:  // 9th Generation (Cometlake-U) DRAM Controller
    case 0x9b71:  // 9th Generation (Cometlake-U) DRAM Controller
      // Next Generation Core processors
      mLegacyInterruptDevice = LEGACY_INT_DEV_Q35;
      IsPchWithP2Sb = 1;
      break;

    default:
      // Unknown chipset
      break;
  }

  return mLegacyInterruptDevice != 0 ? EFI_SUCCESS : EFI_NOT_FOUND;
}


/**
  Initialize Legacy Interrupt support

  @retval EFI_SUCCESS   Successfully initialized

**/
EFI_STATUS
LegacyInterruptInstall (
  VOID
  )
{
  EFI_STATUS  Status;
  UINT16      HostBridgeVenId;

  //
  // Make sure the Legacy Interrupt Protocol is not already installed in the system
  //
  ASSERT_PROTOCOL_ALREADY_INSTALLED(NULL, &gEfiLegacyInterruptProtocolGuid);

  //
  // Query Host Bridge DeviceId to determine platform type, then set device number
  //
  HostBridgeVenId = PciRead16 (PCI_LIB_ADDRESS (0, 0, 0, PCI_VENDOR_ID_OFFSET));
  if (HostBridgeVenId != 0x8086) {
    DEBUG ((DEBUG_ERROR, "Unknown Host Bridge Vendor ID: 0x%04x\n", HostBridgeVenId));
    DEBUG ((DEBUG_ERROR, "Only Intel platforms are supported!\n"));
    return EFI_UNSUPPORTED;
  }

  Status = DetectChipset ();
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Make a new handle and install the protocol
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mLegacyInterruptHandle,
                  &gEfiLegacyInterruptProtocolGuid, &mLegacyInterrupt,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

