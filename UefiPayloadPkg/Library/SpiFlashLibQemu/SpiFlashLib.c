/** @file
  QEMU flash device support.

  Copyright (c) 2009 - 2013, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2025, 9elements GmbH. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SpiFlashLib.h>
#include <Guid/FlashRegionMapInfoGuid.h>

#define WRITE_BYTE_CMD           0x10
#define BLOCK_ERASE_CMD          0x20
#define CLEAR_STATUS_CMD         0x50
#define READ_STATUS_CMD          0x70
#define READ_DEVID_CMD           0x90
#define BLOCK_ERASE_CONFIRM_CMD  0xd0
#define READ_ARRAY_CMD           0xff

#define CLEARED_ARRAY_STATUS  0x00

#define QEMU_FLASH_BLOCK_SIZE	 0x1000

STATIC UINTN  mFlashDeviceBase;
STATIC UINTN  mFdBlockCount = 0;

STATIC
UINTN
QemuFlashPtr (
  IN  UINTN    Address
  )
{
  return mFlashDeviceBase + Address;
}

/**
  Read data from the flash part.

  @param[in] FlashRegionType      The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.
  @param[out] Buffer              The Pointer to caller-allocated buffer containing the data received.
                                  It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiFlashRead (
  IN   FLASH_REGION_TYPE  FlashRegionType,
  IN   UINT32             Address,
  IN   UINT32             ByteCount,
  OUT  UINT8              *Buffer
  )
{
  if ((FlashRegionType != FlashRegionAll) && (FlashRegionType != FlashRegionBios)) {
    return EFI_INVALID_PARAMETER;
  }

  if (ByteCount > mFdBlockCount * QEMU_FLASH_BLOCK_SIZE) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem (Buffer, (VOID *)QemuFlashPtr (Address), ByteCount);
  return EFI_SUCCESS;
}

/**
  Write data to the flash part.

  @param[in] FlashRegionType      The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.
  @param[in] Buffer               Pointer to caller-allocated buffer containing the data sent during the SPI cycle.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiFlashWrite (
  IN  FLASH_REGION_TYPE  FlashRegionType,
  IN  UINT32             Address,
  IN  UINT32             ByteCount,
  IN  UINT8              *Buffer
  )
{
  UINTN  PhysicalAddress;
  UINTN  Index;

  if ((FlashRegionType != FlashRegionAll) && (FlashRegionType != FlashRegionBios)) {
    return EFI_INVALID_PARAMETER;
  }

  if (ByteCount > mFdBlockCount * QEMU_FLASH_BLOCK_SIZE) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Program flash
  //
  PhysicalAddress = QemuFlashPtr (Address);
  for (Index = 0; Index < ByteCount; Index++) {
    MmioWrite8 (PhysicalAddress, WRITE_BYTE_CMD);
    MmioWrite8 (PhysicalAddress, Buffer[Index]);

    PhysicalAddress++;
  }

  //
  // Restore flash to read mode
  //
  if (ByteCount > 0) {
    MmioWrite8 (PhysicalAddress - 1, READ_ARRAY_CMD);
  }

  return EFI_SUCCESS;
}

/**
  Erase some area on the flash part.

  @param[in] FlashRegionType      The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiFlashErase (
  IN  FLASH_REGION_TYPE  FlashRegionType,
  IN  UINT32             Address,
  IN  UINT32             ByteCount
  )
{
  UINTN       RemainingBytes;
  UINT8       *BackupBuffer;
  EFI_STATUS  Status;
  UINTN       WholeBlocks;
  UINTN       Index;

  if ((FlashRegionType != FlashRegionAll) && (FlashRegionType != FlashRegionBios)) {
    return EFI_INVALID_PARAMETER;
  }

  if (ByteCount > mFdBlockCount * QEMU_FLASH_BLOCK_SIZE) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // If the address is not aligned, we have to erase the head too.
  // Erase it entirely and restore leading bytes.
  //
  if (!IS_ALIGNED (Address, QEMU_FLASH_BLOCK_SIZE)) {
    RemainingBytes = Address % QEMU_FLASH_BLOCK_SIZE;
    Address -= RemainingBytes;
    ByteCount -= (QEMU_FLASH_BLOCK_SIZE - RemainingBytes);

    BackupBuffer = AllocatePool (QEMU_FLASH_BLOCK_SIZE);
    ASSERT (BackupBuffer != NULL);
    Status = SpiFlashRead (FlashRegionType, Address, QEMU_FLASH_BLOCK_SIZE, BackupBuffer);
    ASSERT_EFI_ERROR (Status);

    MmioWrite8 (QemuFlashPtr (Address), BLOCK_ERASE_CMD);
    MmioWrite8 (QemuFlashPtr (Address), BLOCK_ERASE_CONFIRM_CMD);

    Status = SpiFlashWrite (FlashRegionType, Address, RemainingBytes, BackupBuffer);
    ASSERT_EFI_ERROR (Status);
    FreePool (BackupBuffer);
  }

  WholeBlocks = ByteCount / QEMU_FLASH_BLOCK_SIZE;
  for (Index = 0; Index < WholeBlocks; Index++) {
    MmioWrite8 (QemuFlashPtr (Address), BLOCK_ERASE_CMD);
    MmioWrite8 (QemuFlashPtr (Address), BLOCK_ERASE_CONFIRM_CMD);

    Address += QEMU_FLASH_BLOCK_SIZE;
  }

  //
  // We must erase another block, but the tail wasn't meant to be erased.
  // Erase it entirely and restore the trailing bytes.
  //
  RemainingBytes = ByteCount % QEMU_FLASH_BLOCK_SIZE;
  if (RemainingBytes != 0) {
    BackupBuffer = AllocatePool (QEMU_FLASH_BLOCK_SIZE);
    ASSERT (BackupBuffer != NULL);
    Status = SpiFlashRead (FlashRegionType, Address, QEMU_FLASH_BLOCK_SIZE, BackupBuffer);
    ASSERT_EFI_ERROR (Status);

    MmioWrite8 (QemuFlashPtr (Address), BLOCK_ERASE_CMD);
    MmioWrite8 (QemuFlashPtr (Address), BLOCK_ERASE_CONFIRM_CMD);

    Status = SpiFlashWrite (FlashRegionType, Address + RemainingBytes, QEMU_FLASH_BLOCK_SIZE - RemainingBytes, BackupBuffer);
    ASSERT_EFI_ERROR (Status);
    FreePool (BackupBuffer);
  }

  return EFI_SUCCESS;
}

/**
  Get the SPI region base and size, based on the enum type

  @param[in] FlashRegionType      The Flash Region type for for the base address which is listed in the Descriptor.
  @param[out] BaseAddress         The Flash Linear Address for the Region 'n' Base
  @param[out] RegionSize          The size for the Region 'n'

  @retval EFI_SUCCESS             Read success
  @retval EFI_INVALID_PARAMETER   Invalid region type given
  @retval EFI_DEVICE_ERROR        The region is not used
**/
EFI_STATUS
EFIAPI
SpiGetRegionAddress (
  IN   FLASH_REGION_TYPE  FlashRegionType,
  OUT  UINT32             *BaseAddress  OPTIONAL,
  OUT  UINT32             *RegionSize OPTIONAL
  )
{
  //
  // TODO: Determine if QEMU flash devices can be divided into regions.
  // - And if so, how to support it.
  //

  if ((FlashRegionType == FlashRegionAll) || (FlashRegionType == FlashRegionBios)) {
    if (BaseAddress != NULL) {
      *BaseAddress = 0;
    }

    if (RegionSize != NULL) {
      *RegionSize = mFdBlockCount * QEMU_FLASH_BLOCK_SIZE;
    }

    return EFI_SUCCESS;
  }

  return EFI_INVALID_PARAMETER;
}

/**
  Initialize the SPI library.

  @retval EFI_SUCCESS             The protocol instance was properly initialized.
  @retval EFI_WRITE_PROTECTED     The flash device is read-only.
  @retval EFI_NO_MEDIA            The flash device is not present.
  @retval EFI_NOT_FOUND           The needed SPI info could not be found.
**/
EFI_STATUS
EFIAPI
SpiConstructor (
  VOID
  )
{
  EFI_HOB_GUID_TYPE      *GuidHob;
  FLASH_REGION_MAP_INFO  *Fmap;
  BOOLEAN                FlashDetected;
  UINTN                  PhysicalAddress;
  UINTN                  Offset;
  UINT8                  OriginalUint8;
  UINT8                  ProbeUint8;

  //
  // Determine the flash device's base address and size.
  // The QEMU boards don't use flash windowing, so infer this from the FMAP.
  //
  GuidHob = GetFirstGuidHob (&gEfiFlashRegionMapInfoHobGuid);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }

  Fmap = GET_GUID_HOB_DATA (GuidHob);
  ASSERT (Fmap->Size % QEMU_FLASH_BLOCK_SIZE == 0);

  mFlashDeviceBase = (0x100000000ULL - Fmap->Size);
  mFdBlockCount = Fmap->Size / QEMU_FLASH_BLOCK_SIZE;

  //
  // Determine if a QEMU flash device is present here.
  //
  FlashDetected = FALSE;
  for (Offset = 0; Offset < QEMU_FLASH_BLOCK_SIZE; Offset++) {
    PhysicalAddress        = QemuFlashPtr (Offset);
    ProbeUint8 = MmioRead8 (PhysicalAddress);
    if ((ProbeUint8 != CLEAR_STATUS_CMD) &&
        (ProbeUint8 != READ_STATUS_CMD) &&
        (ProbeUint8 != CLEARED_ARRAY_STATUS))
    {
      break;
    }
  }

  if (Offset >= QEMU_FLASH_BLOCK_SIZE) {
    DEBUG ((DEBUG_INFO, "QEMU Flash: Failed to find probe location\n"));
    return EFI_NO_MEDIA;
  }

  DEBUG ((DEBUG_INFO, "QEMU Flash: Attempting flash detection at %p\n", PhysicalAddress));

  //
  // Detect what type of flash is present. For a pflash device, this refreshes the status.
  //
  OriginalUint8 = MmioRead8 (PhysicalAddress);
  MmioWrite8 (PhysicalAddress, CLEAR_STATUS_CMD);
  ProbeUint8    = MmioRead8 (PhysicalAddress);
  if ((OriginalUint8 != CLEAR_STATUS_CMD) &&
      (ProbeUint8 == CLEAR_STATUS_CMD))
  {
    DEBUG ((DEBUG_INFO, "QemuFlashDetected => FD behaves as RAM\n"));
    MmioWrite8 (PhysicalAddress, OriginalUint8);
  } else {
    MmioWrite8 (PhysicalAddress, READ_STATUS_CMD);
    ProbeUint8 = MmioRead8 (PhysicalAddress);
    if (ProbeUint8 == OriginalUint8) {
      DEBUG ((DEBUG_INFO, "QemuFlashDetected => FD behaves as ROM\n"));
    } else if (ProbeUint8 == READ_STATUS_CMD) {
      DEBUG ((DEBUG_INFO, "QemuFlashDetected => FD behaves as RAM\n"));
      MmioWrite8 (PhysicalAddress, OriginalUint8);
    } else if (ProbeUint8 == CLEARED_ARRAY_STATUS) {
      MmioWrite8 (PhysicalAddress, WRITE_BYTE_CMD);
      MmioWrite8 (PhysicalAddress, OriginalUint8);
      MmioWrite8 (PhysicalAddress, READ_STATUS_CMD);
      ProbeUint8 = MmioRead8 (PhysicalAddress);
      if (ProbeUint8 & 0x10 /* programming error */) {
        DEBUG ((DEBUG_INFO, "QemuFlashDetected => FD behaves as FLASH, write-protected\n"));
      } else {
        DEBUG ((DEBUG_INFO, "QemuFlashDetected => FD behaves as FLASH, writable\n"));
        FlashDetected = TRUE;
      }

      // Restore flash device to read mode.
      MmioWrite8 (PhysicalAddress, READ_ARRAY_CMD);
    }
  }

  DEBUG ((DEBUG_INFO, "QemuFlashDetected = %d\n", FlashDetected));

  return FlashDetected ? EFI_SUCCESS : EFI_WRITE_PROTECTED;
}
