/** @file
  Legacy Interrupt Support for Intel platforms

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include "LegacyInterrupt.h"

BOOLEAN  PirqsOnSideband = FALSE;

/*
 * Sideband-related definitions.
 */
STATIC UINTN  SbregBase;
#define PCR_ADDRESS(Pid, Offset)  (SbregBase | (Pid << 16) | Offset)

STATIC UINT8  PidItss;
#define PCR_ITSS_PIRQA_ROUT       0x3100

UINT8  PirqReg[MAX_PIRQ_NUMBER] = { PIRQA, PIRQB, PIRQC, PIRQD, PIRQE, PIRQF, PIRQG, PIRQH };

/**
  Builds the PCI configuration address for the register specified by PirqNumber

  @param[in]  PirqNumber - The PIRQ number to build the PCI configuration address for

  @return  The PCI Configuration address for the PIRQ
**/
UINTN
EFIAPI
GetAddress (
  IN  UINT8  PirqNumber
  )
{
  return PCI_LIB_ADDRESS (
           LEGACY_INT_BUS,
           mLegacyInterruptDevice,
           mLegacyInterruptDeviceFunction,
           PirqReg[PirqNumber]
           );
}

/**
  Read one PIRQ from the sideband.

**/
UINT8
EFIAPI
ReadPirqSideband (
  IN  UINT8  PirqNumber
  )
{
  /*
   * Inline an implementation of `PcrRead8(), specific to
   * ITSS registers in the sideband's MMIO space.
   * - PCR offsets must be size-aligned, byte-wise access naturally is.
   */
  return MmioRead8 (PCR_ADDRESS (PidItss, (PCR_ITSS_PIRQA_ROUT + PirqNumber))) & 0x7F;
}

/**
  Write one PIRQ on the sideband.

**/
VOID
EFIAPI
WritePirqSideband (
  IN  UINT8  PirqNumber,
  IN  UINT8  PirqData
  )
{
  /*
   * Inline an implementation of `PcrWrite8(), specific to
   * ITSS registers in the sideband's MMIO space.
   * - PCR offsets must be size-aligned, byte-wise access naturally is.
   */
  MmioWrite8 (PCR_ADDRESS (PidItss, (PCR_ITSS_PIRQA_ROUT + PirqNumber)), PirqData);

  /*
   * For certain ports, one needs to read a register to
   * ensure the writes are completed. This is done for
   * all ports so that per-port knowledge is not required.
   */
  MmioRead8 (PCR_ADDRESS (PidItss, (PCR_ITSS_PIRQA_ROUT + PirqNumber)));
}

STATIC
BOOLEAN
EFIAPI
CheckPirqsOnSideband (
  VOID
  )
{
  UINT8  Pirq;
  UINT8  PirqNumber;
  UINT8  PirqAllZeros = 0;

  for (PirqNumber = 0; PirqNumber < ARRAY_SIZE (PirqReg); PirqNumber++) {
    Pirq = PciRead8 (GetAddress (PirqNumber));
    if (Pirq == 0) {
      PirqAllZeros |= (1 << PirqNumber);
    }
  }

  /* All PIRQ registers on LPC device are zeros, probably they are located on sideband */
  if (PirqAllZeros == 0xFF) {
    PirqsOnSideband = TRUE;
  }

  return PirqsOnSideband;
}

/**
  Initialise the Legacy Interrupt protocol for Intel platforms.

**/
VOID
EFIAPI
LegacyInterruptInitialiseIntel (
  VOID
  )
{
  mLegacyInterruptDevice = 0x1F;
  mLegacyInterruptDeviceFunction = 0;

  //
  // Determine PIRQ routing
  //
  if (CheckPirqsOnSideband () == FALSE) {
    return;
  }

  // BUGBUG: Don't hardcode this!
  SbregBase = 0xE0000000;
  PidItss = 0xC4;
}
