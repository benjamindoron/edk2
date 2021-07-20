/** @file
  Serial Port library functions for in-memory debug logging

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/SerialPortLib.h>

///
/// A structure that defines the layout of the in-memory ringbuffer containing the logs.
///
typedef struct {
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
  /// PEI indicates that it switched buffer.
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
