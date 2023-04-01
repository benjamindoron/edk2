/** @file
  Serial Port library functions for in-memory debug logging

  Copyright (c) 2021, Baruch Binyamin Doron
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiMultiPhase.h>
#include <Pi/PiMultiPhase.h>
#include <Library/SerialPortLib.h>

///
/// This structure defines the ringbuffer header.
///
typedef struct {
  ///
  /// Reserve space for PEI phase HOB's GUID. In DXE/SMM, this is unused.
  /// Perhaps slightly wasteful, but simpler than defining more structs.
  /// TODO/TEST: It's unclear to me whether this is strictly necessary.
  ///
  EFI_HOB_GUID_TYPE      GuidHob;

  ///
  /// Cursor location inside the buffer.
  ///
  UINT32                 Cursor;

  ///
  /// Size of the present buffer in KiB.
  ///
  UINT16                 Size;

  ///
  /// PEI indicates that it switched buffer.
  ///
  UINT16                 PeiSwitchedBuffer;
} DEBUG_LOG_RINGBUFFER_HEADER;

///
/// A structure that defines the layout of the in-memory ringbuffer containing the logs.
///
typedef struct {
  ///
  /// Ringbuffer header, containing metadata.
  ///
  DEBUG_LOG_RINGBUFFER_HEADER  Header;

  ///
  /// Ringbuffer body.
  ///
  UINT8                        Body[];
} DEBUG_LOG_RINGBUFFER;

/**
  Phase-specific implementation to retrieve a pointer to the ringbuffer.

  @return  The pointer to the ringbuffer, NULL on failure.

**/
DEBUG_LOG_RINGBUFFER *
GetDebugLogsRingbuffer (
  VOID
  );
