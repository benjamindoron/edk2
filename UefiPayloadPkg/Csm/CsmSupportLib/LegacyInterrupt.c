/** @file
  Legacy Interrupt Support

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "CsmSupportLib.h"
#include "LegacyInterrupt.h"

//
// Handle for the Legacy Interrupt Protocol instance produced by this driver
//
STATIC EFI_HANDLE  mLegacyInterruptHandle = NULL;

//
// The Legacy Interrupt Protocol instance produced by this driver
//
STATIC EFI_LEGACY_INTERRUPT_PROTOCOL  mLegacyInterrupt = {
  GetNumberPirqs,
  GetLocation,
  ReadPirq,
  WritePirq
};

//
// Legacy Interrupt Device and Function number
//
UINT8  mLegacyInterruptDevice = 0xFF;
UINT8  mLegacyInterruptDeviceFunction = 0xFF;

/**
  Return the number of PIRQs supported by chipset.

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
  *Function = mLegacyInterruptDeviceFunction;

  return EFI_SUCCESS;
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

  if (mChipsetVendor == CHIPSET_IS_INTEL) {
    if (PirqsOnSideband) {
      *PirqData = ReadPirqSideband (PirqNumber);
    } else {
      *PirqData = PciRead8 (GetAddress (PirqNumber)) & 0x7F;
    }
    return EFI_SUCCESS;
  }

  if (mChipsetVendor == CHIPSET_IS_AMD) {
    *PirqData = ReadPciIntIndex (PirqNumber) & 0x1F;
    /* Check if unused or not programmed */
    if (*PirqData == 0x1F) {
      *PirqData = 0;
    }
    return EFI_SUCCESS;
  }

  return EFI_DEVICE_ERROR;
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

  if (mChipsetVendor == CHIPSET_IS_INTEL) {
    if (PirqsOnSideband) {
      WritePirqSideband (PirqNumber, PirqData);
    } else {
      PciWrite8 (GetAddress (PirqNumber), PirqData);
    }
    return EFI_SUCCESS;
  }

  if (mChipsetVendor == CHIPSET_IS_AMD) {
    WritePciIntIndex (PirqNumber, PirqData);
    return EFI_SUCCESS;
  }

  return EFI_DEVICE_ERROR;
}

/**
  Initialize Legacy Interrupt support

  @retval EFI_SUCCESS   Successfully initialized

**/
EFI_STATUS
EFIAPI
LegacyInterruptInstall (
  VOID
  )
{
  EFI_STATUS  Status;

  //
  // Make sure the Legacy Interrupt Protocol is not already installed in the system
  //
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiLegacyInterruptProtocolGuid);

  if (mChipsetVendor == CHIPSET_IS_INTEL) {
    LegacyInterruptInitialiseIntel ();
  } else if (mChipsetVendor == CHIPSET_IS_AMD) {
    LegacyInterruptInitialiseAmd ();
  }

  //
  // Install the Legacy Interrupt Protocol on a new handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mLegacyInterruptHandle,
                  &gEfiLegacyInterruptProtocolGuid,
                  &mLegacyInterrupt,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
