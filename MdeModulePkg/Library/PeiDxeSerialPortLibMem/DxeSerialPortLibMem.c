/** @file
  Serial Port library functions for in-memory debug logging

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
// - Consider using module-scoped PCD overrides for a larger DXE buffer.
// - User must ensure that PEI buffer is not larger than maximum HOB size.
#define BUFFER_SIZE  FixedPcdGet16 (PcdStatusCodeMemorySize)

// Consider converting pointer?
// - DxeRuntimeDebugLibSerialPort and RuntimeDxeReportStatusCodeLib handle ExitBootServices()
// - However, CpuDxe can consume SerialPortLib through CpuExceptionHandlerLib
STATIC DEBUG_LOG_RINGBUFFER  *mDebugLogsRingbuffer = NULL;

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

  EfiGetSystemConfigurationTable (&gDebugLogRingbufferGuid, (VOID **) &mDebugLogsRingbuffer);
  if (mDebugLogsRingbuffer != NULL) {
    // An entire phase only requires one ringbuffer
    return RETURN_SUCCESS;
  }

  //
  // Allocate runtime memory ringbuffer pool, plus the size of the metadata.
  // BUGBUG: PEI phase affected by possible size/alignment bug. Therefore, just allocate
  //         extra KiB instead.
  // TODO: Is "allocate pages" more performant?
  //
  mDebugLogsRingbuffer = AllocateRuntimePool (
                           BUFFER_SIZE*SIZE_1KB + sizeof(DEBUG_LOG_RINGBUFFER_HEADER)
                           );
  if (mDebugLogsRingbuffer == NULL) {
    return RETURN_OUT_OF_RESOURCES;
  }

  // Copy the old data, if possible. If the buffers are equally sized,
  // copy but zero the cursor so that future writes succeed.
  GuidHob = GetFirstGuidHob (&gDebugLogRingbufferGuid);
  if (GuidHob != NULL) {
    PeiRingbufferData = GET_GUID_HOB_DATA (GuidHob);
    if (PeiRingbufferData->Header.PeiSwitchedBuffer) {
      GuidHob = GetNextGuidHob (&gDebugLogRingbufferGuid, GuidHob);
      if (GuidHob != NULL) {
        PeiRingbufferData = GET_GUID_HOB_DATA (GuidHob);
      }
    }
    if (BUFFER_SIZE*SIZE_1KB > PeiRingbufferData->Header.Cursor) {
      CopyMem(&mDebugLogsRingbuffer->Body, &PeiRingbufferData->Body, PeiRingbufferData->Header.Cursor);
      mDebugLogsRingbuffer->Header.Cursor  = PeiRingbufferData->Header.Cursor;
    } else if (BUFFER_SIZE*SIZE_1KB == PeiRingbufferData->Header.Cursor) {
      CopyMem (&mDebugLogsRingbuffer->Body, &PeiRingbufferData->Body, PeiRingbufferData->Header.Cursor);
      mDebugLogsRingbuffer->Header.Cursor  = 0;
    } else {
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
