/** @file
  Serial Port library functions for in-memory debug logging

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include "SerialPortLibMem.h"

// NOTE:
// - Consider using module-scoped PCD overrides for a larger DXE buffer.
// - User must ensure that PEI buffer is not larger than maximum HOB size.
#define EARLY_BUFFER_SIZE  8  // TODO: Determine buffer size in CAR
#define LATE_BUFFER_SIZE   FixedPcdGet16 (PcdStatusCodeMemorySize)

/**
  Create larger debug log ringbuffer once there is permanent memory.

  @param[in]  PeiServices       General purpose services available to every PEIM.
  @param[in]  NotifyDescriptor  Notify that this module published.
  @param[in]  Ppi               PPI that was installed.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN VOID                        *Ppi
  )
{
  EFI_HOB_GUID_TYPE     *GuidHob;
  DEBUG_LOG_RINGBUFFER  *OldRingbufferData;
  DEBUG_LOG_RINGBUFFER  *NewRingbufferData;

  GuidHob = GetFirstGuidHob (&gDebugLogRingbufferGuid);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }

  OldRingbufferData                            = GET_GUID_HOB_DATA (GuidHob);
  OldRingbufferData->Header.PeiSwitchedBuffer  = 1;

  NewRingbufferData = BuildGuidHob (
                        &gDebugLogRingbufferGuid,
                        LATE_BUFFER_SIZE*SIZE_1KB + sizeof(DEBUG_LOG_RINGBUFFER_HEADER)
                        );
  if (NewRingbufferData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Copy the old data, if possible. If the buffers are equally sized,
  // copy but zero the cursor so that future writes succeed.
  if (LATE_BUFFER_SIZE*SIZE_1KB > OldRingbufferData->Header.Cursor) {
    CopyMem (&NewRingbufferData->Body, &OldRingbufferData->Body, OldRingbufferData->Header.Cursor);
    NewRingbufferData->Header.Cursor           = OldRingbufferData->Header.Cursor;
  } else if (LATE_BUFFER_SIZE*SIZE_1KB == OldRingbufferData->Header.Cursor) {
    CopyMem (&NewRingbufferData->Body, &OldRingbufferData->Body, OldRingbufferData->Header.Cursor);
    NewRingbufferData->Header.Cursor           = 0;
  } else {
    NewRingbufferData->Header.Cursor           = 0;
  }
  NewRingbufferData->Header.Size               = LATE_BUFFER_SIZE;
  NewRingbufferData->Header.PeiSwitchedBuffer  = 1;

  return EFI_SUCCESS;
}

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_NOTIFY_DESCRIPTOR mMemDiscoveredNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMemoryDiscoveredPpiGuid,
  (EFI_PEIM_NOTIFY_ENTRY_POINT) MemoryDiscoveredPpiNotifyCallback
};

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
  EFI_HOB_GUID_TYPE     *GuidHob;
  DEBUG_LOG_RINGBUFFER  *RingbufferHob;

  GuidHob = GetFirstGuidHob (&gDebugLogRingbufferGuid);
  if (GuidHob != NULL) {
    // An entire phase only requires one ringbuffer.
    // - GetDebugLogsRingbuffer() will track second ringbuffer.
    return RETURN_SUCCESS;
  }

  //
  // Build GUID'ed HOB with PCD defined size, plus the size of the metadata.
  // BUGBUG: PEI phase affected by possible size/alignment bug. Therefore, just allocate
  //         extra KiB instead.
  //
  // Allocating small buffer due to CAR restrictions.
  // User-defined buffer size may be allocated after memory is installed.
  RingbufferHob = BuildGuidHob (
                    &gDebugLogRingbufferGuid,
                    EARLY_BUFFER_SIZE*SIZE_1KB + sizeof(DEBUG_LOG_RINGBUFFER_HEADER)
                    );
  if (RingbufferHob == NULL) {
    return RETURN_OUT_OF_RESOURCES;
  }

  RingbufferHob->Header.Cursor             = 0;
  RingbufferHob->Header.Size               = EARLY_BUFFER_SIZE;
  RingbufferHob->Header.PeiSwitchedBuffer  = 0;

  PeiServicesNotifyPpi (&mMemDiscoveredNotifyList);

  return RETURN_SUCCESS;
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
  EFI_HOB_GUID_TYPE     *GuidHob;
  DEBUG_LOG_RINGBUFFER  *RingbufferPtr;

  GuidHob = GetFirstGuidHob (&gDebugLogRingbufferGuid);
  if (GuidHob == NULL) {
    return NULL;
  }

  RingbufferPtr = GET_GUID_HOB_DATA (GuidHob);
  if (RingbufferPtr->Header.PeiSwitchedBuffer) {
    GuidHob = GetNextGuidHob (&gDebugLogRingbufferGuid, GuidHob);
    if (GuidHob == NULL) {
      return NULL;
    }
    RingbufferPtr = GET_GUID_HOB_DATA (GuidHob);
  }
  return RingbufferPtr;
}
