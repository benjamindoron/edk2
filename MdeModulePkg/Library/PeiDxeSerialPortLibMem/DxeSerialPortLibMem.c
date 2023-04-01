/** @file
  Serial Port library functions for in-memory debug logging

  Copyright (c) 2021, Baruch Binyamin Doron
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <PiDxe.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include "SerialPortLibMem.h"

// NOTE:
// - User must ensure that SMM phase buffer size is equal to the DXE instance.
// - Across any phase, all instances must declare the same size.
#define BUFFER_SIZE  FixedPcdGet16 (PcdStatusCodeMemorySize)

DEBUG_LOG_RINGBUFFER  *mDebugLogsRingbuffer = NULL;

/**
  Exit Boot Services Event notification handler.

  Store ringbuffer details for use at runtime by user.

  @param[in]  Event     Event whose notification function is being invoked
  @param[in]  Context   Pointer to the notification function's context

**/
VOID
EFIAPI
OnExitBootServices (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  UINTN  Data;

  Data = (UINTN) &mDebugLogsRingbuffer->Body[0];
  gRT->SetVariable (
         L"DebugLogsAddress",
         &gDebugLogRingbufferGuid,
         (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS),
         sizeof(UINTN),
         &Data
         );

  Data = mDebugLogsRingbuffer->Header.Cursor;
  gRT->SetVariable (
         L"DebugLogsCursor",
         &gDebugLogRingbufferGuid,
         (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS),
         sizeof(Data),
         &Data
         );

  Data = BUFFER_SIZE*SIZE_1KB;
  gRT->SetVariable (
         L"DebugLogsSize",
         &gDebugLogRingbufferGuid,
         (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS),
         sizeof(Data),
         &Data
         );

  gBS->CloseEvent (Event);
}

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
  RETURN_STATUS         Status;
  EFI_HOB_GUID_TYPE     *GuidHob;
  DEBUG_LOG_RINGBUFFER  *PeiRingbufferData;
  EFI_EVENT             Event;

  // TODO: Consider special S3 resume handling
  // - DebugLibSerialPort risks imposing gBS. Unlike in DxeCore presentation,
  //  this can be avoided, because BootScriptExecutorDxe is late.
  // - However, UefiLib here uses gST
  // - Consider bail-out and SerialPortWrite() calls SerialPortInitialize()
  EfiGetSystemConfigurationTable (&gDebugLogRingbufferGuid, (VOID **) &mDebugLogsRingbuffer);
  if (mDebugLogsRingbuffer != NULL) {
    // TODO: Can we assert that phase uses same size buffer?
    // An entire phase only requires one ringbuffer.
    return RETURN_SUCCESS;
  }

  //
  // Allocate runtime memory ringbuffer pool, plus the size of the metadata.
  //
  mDebugLogsRingbuffer = AllocateRuntimePool (
                           BUFFER_SIZE*SIZE_1KB + sizeof(DEBUG_LOG_RINGBUFFER_HEADER)
                           );
  if (mDebugLogsRingbuffer == NULL) {
    return RETURN_OUT_OF_RESOURCES;
  }

  //
  // Initialise buffer to lower case 'x'.
  //
  SetMem ((VOID *) mDebugLogsRingbuffer->Body, BUFFER_SIZE*SIZE_1KB, 0x78);

  // Copy the old data, if possible. If the buffers are equally sized,
  // copy but zero the cursor so that future writes succeed.
  GuidHob = GetFirstGuidHob (&gDebugLogRingbufferGuid);
  if (GuidHob != NULL) {
    PeiRingbufferData = GET_GUID_HOB_DATA (GuidHob);
    if (PeiRingbufferData->Header.PeiSwitchedBuffer) {
      GuidHob = GetNextGuidHob (&gDebugLogRingbufferGuid, GET_NEXT_HOB (GuidHob));
      if (GuidHob != NULL) {
        PeiRingbufferData = GET_GUID_HOB_DATA (GuidHob);
      }
    }
    if (BUFFER_SIZE*SIZE_1KB > PeiRingbufferData->Header.Cursor) {
      CopyMem (&mDebugLogsRingbuffer->Body, &PeiRingbufferData->Body, PeiRingbufferData->Header.Cursor);
      mDebugLogsRingbuffer->Header.Cursor  = PeiRingbufferData->Header.Cursor;
    } else if (BUFFER_SIZE*SIZE_1KB == PeiRingbufferData->Header.Cursor) {
      CopyMem (&mDebugLogsRingbuffer->Body, &PeiRingbufferData->Body, PeiRingbufferData->Header.Cursor);
      mDebugLogsRingbuffer->Header.Cursor  = 0;
    } else {
      // Copy ringbuffer tail
      CopyMem (&mDebugLogsRingbuffer->Body, PeiRingbufferData->Body + (PeiRingbufferData->Header.Cursor - BUFFER_SIZE*SIZE_1KB), BUFFER_SIZE*SIZE_1KB);
      mDebugLogsRingbuffer->Header.Cursor  = 0;
    }
  } else {
    mDebugLogsRingbuffer->Header.Cursor    = 0;
  }

  mDebugLogsRingbuffer->Header.Size        = BUFFER_SIZE;

  Status = gBS->InstallConfigurationTable (&gDebugLogRingbufferGuid, mDebugLogsRingbuffer);
  if (EFI_ERROR (Status)) {
    return RETURN_DEVICE_ERROR;
  }

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  OnExitBootServices,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &Event
                  );

  return Status;
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
