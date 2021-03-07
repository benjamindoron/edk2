/** @file
  Legacy Interrupt Support for AMD platforms

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>
#include <Library/IoLib.h>
#include "LegacyInterrupt.h"

/* FCH-related definitions */
#define PCI_INTR_INDEX  0xC00
#define PCI_INTR_DATA   0xC01
#define MODE_PIC        0
#define MODE_IOAPIC     1

/**
  Read from the FCH PCI_INTR registers 0xC00/0xC01 at a
  given index and a given PIC (0) or IOAPIC (1) mode.

**/
UINT8
EFIAPI
ReadPciIntIndex (
  IN  UINT8  Index
  )
{
  // NOTE: Hardcoding PIC mode at the moment
  IoWrite8 (PCI_INTR_INDEX, (MODE_PIC << 7) | Index);
  return IoRead8 (PCI_INTR_DATA);
}

/**
  Write to the FCH PCI_INTR registers 0xC00/0xC01 at a
  given index and a given PIC (0) or IOAPIC (1) mode.

**/
VOID
EFIAPI
WritePciIntIndex (
  IN  UINT8  Index,
  IN  UINT8  Data
  )
{
  // NOTE: Hardcoding PIC mode at the moment
  IoWrite8 (PCI_INTR_INDEX, (MODE_PIC << 7) | Index);
  IoWrite8 (PCI_INTR_DATA, Data);
}

/**
  Initialise the Legacy Interrupt protocol for AMD platforms.

**/
VOID
EFIAPI
LegacyInterruptInitialiseAmd (
  VOID
  )
{
  mLegacyInterruptDevice = 0x14;
  mLegacyInterruptDeviceFunction = 3;
}
