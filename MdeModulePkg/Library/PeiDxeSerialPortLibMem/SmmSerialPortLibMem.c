/** @file
  Serial Port library functions for in-memory debug logging

  Copyright (c) 2021, Baruch Binyamin Doron
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <PiSmm.h>
#include <Library/PcdLib.h>
#include <Library/SmmMemLib.h>
#include <Library/UefiLib.h>
#include "SerialPortLibMem.h"

// NOTE:
// - User must ensure that SMM phase buffer size is equal to the DXE instance.
// - Across any phase, all instances must be the same size.
#define BUFFER_SIZE  FixedPcdGet16 (PcdStatusCodeMemorySize)

DEBUG_LOG_RINGBUFFER  *mDebugLogsRingbuffer = NULL;

/**
  Initialize the serial device hardware.

  If no initialization is required, then return RETURN_SUCCESS.
  If the serial device was successfully initialized, then return RETURN_SUCCESS.
  If the serial device could not be initialized, then return RETURN_DEVICE_ERROR.

  @retval RETURN_SUCCESS        The serial device was initialized.
  @retval RETURN_DEVICE_ERROR   The serial device could not be initialized.

**/
RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{
  EfiGetSystemConfigurationTable (&gDebugLogRingbufferGuid, (VOID **) &mDebugLogsRingbuffer);
  if (mDebugLogsRingbuffer == NULL) {
    return RETURN_DEVICE_ERROR;
  }

  // Only use the memory buffer if it passes security checks
  // - TODO/NB: Still, some memory ranges aren't safe (consider sideband in MMIO). Need to confirm more checks
  if (SmmIsBufferOutsideSmmValid ((EFI_PHYSICAL_ADDRESS) mDebugLogsRingbuffer, BUFFER_SIZE*SIZE_1KB)) {
    // TODO: Can we assert that DXE allocation == SMM allocation and phase uses same size buffer?
    return RETURN_SUCCESS;
  } else {
    // Invalidate the ringbuffer pointer and return an error
    mDebugLogsRingbuffer = NULL;
    return RETURN_DEVICE_ERROR;
  }
}

/**
  Phase-specific implementation to retrieve a pointer to the ringbuffer.

  @return  The pointer to the ringbuffer, NULL on failure.

**/
DEBUG_LOG_RINGBUFFER *
GetDebugLogsRingbuffer (
  VOID
  )
{
  return mDebugLogsRingbuffer;
}
