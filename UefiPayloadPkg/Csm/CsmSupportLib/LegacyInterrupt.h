/** @file
  Legacy Interrupt Support

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _LEGACY_INTERRUPT_H_
#define _LEGACY_INTERRUPT_H_

#include <PiDxe.h>
#include <Protocol/LegacyInterrupt.h>

//
// Legacy Interrupt Device and Function number
//
#define LEGACY_INT_BUS   0
extern UINT8  mLegacyInterruptDevice;
extern UINT8  mLegacyInterruptDeviceFunction;

extern BOOLEAN  PirqsOnSideband;

//
// PIRQs
//
#define PIRQN  0x00  // PIRQ Null
#define PIRQA  0x60
#define PIRQB  0x61
#define PIRQC  0x62
#define PIRQD  0x63
#define PIRQE  0x68
#define PIRQF  0x69
#define PIRQG  0x6A
#define PIRQH  0x6B

#define MAX_PIRQ_NUMBER  8

/**
  Read from the FCH PCI_INTR registers 0xC00/0xC01 at a
  given index and a given PIC (0) or IOAPIC (1) mode.

**/
UINT8
EFIAPI
ReadPciIntIndex (
  IN  UINT8  Index
  );

/**
  Write to the FCH PCI_INTR registers 0xC00/0xC01 at a
  given index and a given PIC (0) or IOAPIC (1) mode.

**/
VOID
EFIAPI
WritePciIntIndex (
  IN  UINT8  Index,
  IN  UINT8  Data
  );

/**
  Initialise the Legacy Interrupt protocol for AMD platforms.

**/
VOID
EFIAPI
LegacyInterruptInitialiseAmd (
  VOID
  );

/**
  Builds the PCI configuration address for the register specified by PirqNumber

  @param[in]  PirqNumber - The PIRQ number to build the PCI configuration address for

  @return  The PCI Configuration address for the PIRQ
**/
UINTN
EFIAPI
GetAddress (
  IN  UINT8  PirqNumber
  );

/**
  Read one PIRQ from the sideband.

**/
UINT8
EFIAPI
ReadPirqSideband (
  IN  UINT8  PirqNumber
  );

/**
  Write one PIRQ on the sideband.

**/
VOID
EFIAPI
WritePirqSideband (
  IN  UINT8  PirqNumber,
  IN  UINT8  PirqData
  );

/**
  Initialise the Legacy Interrupt protocol for Intel platforms.

**/
VOID
EFIAPI
LegacyInterruptInitialiseIntel (
  VOID
  );

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
  );

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
  );

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
  );

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
  );

#endif
