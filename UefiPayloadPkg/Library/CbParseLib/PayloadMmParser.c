/** @file
  This file parses the coreboot table in memory to extract
  the required information.

  Copyright (c) 2025, 9elements GmbH.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BlParseLib.h>
#include <Library/DebugLib.h>
#include <Library/FmapParserLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Guid/MmCommBuffer.h>
#include <Guid/MmUnblockRegion.h>
#include <Guid/PayloadMmInterfaceInfoGuid.h>
#include <Guid/SmmS3CommunicationInfoGuid.h>
#include <Guid/SmramMemoryReserve.h>
#include <Guid/SpiFlashInfoGuid.h>
#include <Guid/VariableFlashInfo.h>
#include <Guid/VariableRuntimeCacheInfo.h>
#include <UniversalPayload/SerialPortInfo.h>
#include <Coreboot.h>
#include "PayloadMmParser.h"

#define R_SPI_BASE       0x10    ///< 32-bit Memory Base Address Register
#define B_SPI_BAR0_MASK  0x0FFF

/**
  Allocates one or more pages .

  Allocates the number of pages of MemoryType and returns a pointer to the
  allocated buffer.  The buffer returned is aligned on a 4KB boundary.
  If Pages is 0, then NULL is returned.
  If there is not enough memory availble to satisfy the request, then NULL
  is returned.

  @param   Pages                 The number of 4 KB pages to allocate.
  @param   MemoryType            The Memorytype
  @return  A pointer to the allocated buffer or NULL if allocation fails.
**/
VOID *
EFIAPI
PayloadAllocatePages (
  IN UINTN            Pages,
  IN EFI_MEMORY_TYPE  MemoryType
  );

/**
  Convert a packed value from cbuint64 to a UINT64 value.

  @param  val      The pointer to packed data.

  @return          the UNIT64 value after conversion.

**/
UINT64
cb_unpack64 (
  IN struct cbuint64  val
  );

/**
  Find coreboot record with given Tag.

  @param  Tag                The tag id to be found

  @retval NULL              The Tag is not found.
  @retval Others            The pointer to the record found.

**/
VOID *
FindCbTag (
  IN  UINT32  Tag
  );

/**
  This API provides a way to unblock certain data pages to be accessible inside MM environment.

  @param  UnblockAddress              The address of buffer caller requests to unblock, the address
                                      has to be page aligned.
  @param  NumberOfPages               The number of pages requested to be unblocked from MM
                                      environment.
  @retval RETURN_SUCCESS              The request goes through successfully.
  @retval RETURN_NOT_AVAILABLE_YET    The requested functionality is not produced yet.
  @retval RETURN_UNSUPPORTED          The requested functionality is not supported on current platform.
  @retval RETURN_SECURITY_VIOLATION   The requested address failed to pass security check for
                                      unblocking.
  @retval RETURN_INVALID_PARAMETER    Input address either NULL pointer or not page aligned.
  @retval RETURN_INVALID_PARAMETER    Input range to unblock contains invalid types memory other than
                                      EfiRuntimeServicesData, EfiACPIMemoryNVS, and EfiReservedMemory.
  @retval RETURN_INVALID_PARAMETER    Input range to unblock contains memory that doesn't belong to
                                      any memory allocation HOB.
  @retval RETURN_ACCESS_DENIED        The request is rejected due to system has passed certain boot
                                      phase.
**/
STATIC
EFI_STATUS
EFIAPI
MmUnblockMemoryRequest (
  IN EFI_PHYSICAL_ADDRESS  UnblockAddress,
  IN UINT64                NumberOfPages
  )
{
  MM_UNBLOCK_REGION  *MmUnblockMemoryHob;

  if (!IS_ALIGNED (UnblockAddress, SIZE_4KB)) {
    DEBUG ((DEBUG_ERROR, "Error: UnblockAddress is not 4KB aligned: %p\n", UnblockAddress));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Build the GUID'd HOB for MmCore
  //
  MmUnblockMemoryHob = BuildGuidHob (&gMmUnblockRegionHobGuid, sizeof (MM_UNBLOCK_REGION));
  if (MmUnblockMemoryHob == NULL) {
    DEBUG ((DEBUG_ERROR, "MmUnblockMemoryRequest: Failed to allocate hob for unblocked data parameter!!\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (MmUnblockMemoryHob, sizeof (MM_UNBLOCK_REGION));

  //
  // Caller ID is filled in.
  //
  CopyGuid (&MmUnblockMemoryHob->IdentifierGuid, &gEfiCallerIdGuid);
  MmUnblockMemoryHob->PhysicalStart = UnblockAddress;
  MmUnblockMemoryHob->NumberOfPages = NumberOfPages;
  return EFI_SUCCESS;
}

/**
  Build communication buffer HOB.

**/
STATIC
VOID
MmIplBuildCommBufferHob (
  VOID
  )
{
  EFI_STATUS      Status;
  MM_COMM_BUFFER  *MmCommBuffer;
  UINT64          MmCommBufferPages;

  MmCommBufferPages = PcdGet32 (PcdMmCommBufferPages);

  MmCommBuffer = BuildGuidHob (&gMmCommBufferHobGuid, sizeof (MM_COMM_BUFFER));
  ASSERT (MmCommBuffer != NULL);

  //
  // Set MM communicate buffer size
  //
  MmCommBuffer->NumberOfPages = MmCommBufferPages;

  //
  // Allocate runtime memory for MM communicate buffer
  //
  MmCommBuffer->PhysicalStart = (EFI_PHYSICAL_ADDRESS)(UINTN)PayloadAllocatePages (MmCommBufferPages, EfiRuntimeServicesData);
  if (MmCommBuffer->PhysicalStart == 0) {
    DEBUG ((DEBUG_ERROR, "Fail to allocate MM communication buffer\n"));
    ASSERT (MmCommBuffer->PhysicalStart != 0);
  }

  //
  // Build MM unblock memory region HOB for MM communication buffer
  //
  Status = MmUnblockMemoryRequest (MmCommBuffer->PhysicalStart, MmCommBufferPages);
  ASSERT_EFI_ERROR (Status);

  //
  // Allocate runtime memory for MM communication status parameters :
  // ReturnStatus, ReturnBufferSize, IsCommBufferValid
  //
  MmCommBuffer->Status = (EFI_PHYSICAL_ADDRESS)(UINTN)PayloadAllocatePages (EFI_SIZE_TO_PAGES (sizeof (MM_COMM_BUFFER_STATUS)), EfiRuntimeServicesData);
  if (MmCommBuffer->Status == 0) {
    DEBUG ((DEBUG_ERROR, "Fail to allocate memory for MM communication status\n"));
    ASSERT (MmCommBuffer->Status != 0);
  }

  //
  // Build MM unblock memory region HOB for MM communication status
  //
  Status = MmUnblockMemoryRequest (MmCommBuffer->Status, EFI_SIZE_TO_PAGES (sizeof (MM_COMM_BUFFER_STATUS)));
  ASSERT_EFI_ERROR (Status);
}

/**
  Initialize the variable store

  @retval     EFI_SUCCESS if initialize the store success.

**/
STATIC
EFI_STATUS
InitVariableStore (
  UINT32  NvStorageBase,
  UINT32  NvStorageSize
  )
{
  UINT32               NvVariableSize;
  UINT32               FtwWorkingSize;
  UINT32               FtwSpareSize;
  VARIABLE_FLASH_INFO  *VariableFlashInfo;

  //
  // NvStorageBase needs to be 4KB aligned, NvStorageSize needs to be 8KB * n
  //
  if (((NvStorageBase & (SIZE_4KB - 1)) != 0) || ((NvStorageSize & (SIZE_8KB - 1)) != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  FtwSpareSize   = NvStorageSize / 2;
  FtwWorkingSize = 0x2000;
  NvVariableSize = NvStorageSize / 2 - FtwWorkingSize;
  DEBUG ((DEBUG_INFO, "NvStorageBase:0x%x, NvStorageSize:0x%x\n", NvStorageBase, NvStorageSize));

  if (NvVariableSize >= 0x80000000) {
    return EFI_INVALID_PARAMETER;
  }

  VariableFlashInfo = BuildGuidHob (&gVariableFlashInfoHobGuid, sizeof (VARIABLE_FLASH_INFO));
  if (VariableFlashInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  VariableFlashInfo->Version = VARIABLE_FLASH_INFO_HOB_VERSION;

  VariableFlashInfo->NvVariableBaseAddress = NvStorageBase;
  VariableFlashInfo->NvVariableLength      = NvVariableSize;
  VariableFlashInfo->FtwSpareBaseAddress   = NvStorageBase + FtwSpareSize;
  VariableFlashInfo->FtwSpareLength        = FtwSpareSize;
  VariableFlashInfo->FtwWorkingBaseAddress = NvStorageBase + NvVariableSize;
  VariableFlashInfo->FtwWorkingLength      = FtwWorkingSize;

  return EFI_SUCCESS;
}

/**
  Build gEdkiiVariableRuntimeCacheInfoHobGuid.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval others                  Failed to build VariableRuntimeCacheInfo Hob.

**/
STATIC
EFI_STATUS
BuildVariableRuntimeCacheInfoHob (
  VOID
  )
{
  EFI_HOB_GUID_TYPE            *GuidHob;
  VARIABLE_FLASH_INFO          *VariableFlashInfo;
  VARIABLE_RUNTIME_CACHE_INFO  *VariableRuntimeCacheInfo;
  UINTN                        Pages;
  VOID                         *Buffer;
  EFI_STATUS                   Status;

  GuidHob = GetFirstGuidHob (&gVariableFlashInfoHobGuid);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }

  VariableFlashInfo = GET_GUID_HOB_DATA (GuidHob);

  VariableRuntimeCacheInfo = BuildGuidHob (&gEdkiiVariableRuntimeCacheInfoHobGuid, sizeof (VARIABLE_RUNTIME_CACHE_INFO));
  if (VariableRuntimeCacheInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // AllocateRuntimePages for CACHE_INFO_FLAG and unblock it.
  //
  Pages  = EFI_SIZE_TO_PAGES (sizeof (CACHE_INFO_FLAG));
  Buffer = PayloadAllocatePages (Pages, EfiRuntimeServicesData);
  ASSERT (Buffer != NULL);
  Status = MmUnblockMemoryRequest ((EFI_PHYSICAL_ADDRESS)(UINTN)Buffer, Pages);
  ASSERT_EFI_ERROR (Status);

  VariableRuntimeCacheInfo->CacheInfoFlagBuffer = (UINTN)Buffer;

  DEBUG ((
    DEBUG_INFO,
    "PeiVariable: CACHE_INFO_FLAG Buffer is: 0x%lx, number of pages is: 0x%x\n",
    VariableRuntimeCacheInfo->CacheInfoFlagBuffer,
    Pages
    ));

  //
  // AllocateRuntimePages for VolatileCache and unblock it.
  //
  Pages  = EFI_SIZE_TO_PAGES (VariableFlashInfo->NvVariableLength);
  Buffer = PayloadAllocatePages (Pages, EfiRuntimeServicesData);
  ASSERT (Buffer != NULL);
  Status = MmUnblockMemoryRequest ((EFI_PHYSICAL_ADDRESS)(UINTN)Buffer, Pages);
  ASSERT_EFI_ERROR (Status);

  VariableRuntimeCacheInfo->RuntimeVolatileCacheBuffer = (UINTN)Buffer;
  VariableRuntimeCacheInfo->RuntimeVolatileCachePages  = Pages;

  DEBUG ((
    DEBUG_INFO,
    "PeiVariable: Volatile cache Buffer is: 0x%lx, number of pages is: 0x%lx\n",
    VariableRuntimeCacheInfo->RuntimeVolatileCacheBuffer,
    VariableRuntimeCacheInfo->RuntimeVolatileCachePages
    ));

  //
  // AllocateRuntimePages for NVCache and unblock it.
  //
  Pages  = EFI_SIZE_TO_PAGES (VariableFlashInfo->NvVariableLength);
  Buffer = PayloadAllocatePages (Pages, EfiRuntimeServicesData);
  ASSERT (Buffer != NULL);
  Status = MmUnblockMemoryRequest ((EFI_PHYSICAL_ADDRESS)(UINTN)Buffer, Pages);
  ASSERT_EFI_ERROR (Status);

  VariableRuntimeCacheInfo->RuntimeNvCacheBuffer = (UINTN)Buffer;
  VariableRuntimeCacheInfo->RuntimeNvCachePages  = Pages;

  DEBUG ((
    DEBUG_INFO,
    "PeiVariable: NV cache Buffer is: 0x%lx, number of pages is: 0x%lx\n",
    VariableRuntimeCacheInfo->RuntimeNvCacheBuffer,
    VariableRuntimeCacheInfo->RuntimeNvCachePages
    ));

  //
  // There is no HobCache.
  //
  VariableRuntimeCacheInfo->RuntimeHobCacheBuffer = 0;
  VariableRuntimeCacheInfo->RuntimeHobCachePages  = 0;

  return EFI_SUCCESS;
}

/**
  Find payload MM feature tables.

  @retval EFI_SUCCESS     Successfully found the payload MM feature tables.
  @retval EFI_NOT_FOUND   Failed to find the payload MM feature tables.
**/
EFI_STATUS
EFIAPI
ParsePayloadMmFeatureInfo (
  VOID
  )
{
  struct cb_payload_mm_interface_info   *BlMmInfo;
  PAYLOAD_MM_INTERFACE_INFO             *PldMmInfo;
  struct cb_payload_mm_smram_region     *BlSmramInfo;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK        *PldSmramHob;
  struct cb_payload_mm_shared_mem       *BlMmSharedMem;
  PLD_S3_COMMUNICATION                  *PldMmSharedMem;
  struct cb_pld_mm_spi_controller_info  *BlSpiFlashInfo;
  SPI_FLASH_INFO                        *PldSpiFlashInfo;
  EFI_STATUS                            Status;
  EFI_HOB_GUID_TYPE                     *GuidHob;
  UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO    *UniversalSerialPort;
  struct cb_cbmem_ref                   *CbMemConsole;
  EFI_PHYSICAL_ADDRESS                  SmmStoreFmapRegionAddress;
  UINT32                                SmmStoreFmapRegionSize;

  BlMmInfo = FindCbTag (CB_TAG_PAYLOAD_MM_INTERFACE_INFO);
  if (BlMmInfo != NULL) {
    PldMmInfo = BuildGuidHob (&gPayloadMmInterfaceInfoGuid, sizeof (PAYLOAD_MM_INTERFACE_INFO));
    if (PldMmInfo != NULL) {
      PldMmInfo->Revision = BlMmInfo->revision;
      PldMmInfo->BootloaderSmmIs64Bit = BlMmInfo->bootloader_smm_is_64bit;
      PldMmInfo->ApmCmd = BlMmInfo->apm_cmd;

      DEBUG ((DEBUG_INFO, "Created payload MM interface info HOB\n"));
    }
  }

  BlMmSharedMem = FindCbTag (CB_TAG_PAYLOAD_MM_SHARED_MEM);
  if (BlMmSharedMem != NULL) {
    PldMmSharedMem = BuildGuidHob (&gS3CommunicationGuid, sizeof (PLD_S3_COMMUNICATION));
    if (PldMmSharedMem != NULL) {
      PldMmSharedMem->CommBuffer.PhysicalStart = cb_unpack64 (BlMmSharedMem->comm_buffer.physical_start);
      PldMmSharedMem->CommBuffer.CpuStart = cb_unpack64 (BlMmSharedMem->comm_buffer.physical_start);
      PldMmSharedMem->CommBuffer.PhysicalSize = cb_unpack64 (BlMmSharedMem->comm_buffer.physical_size);
      PldMmSharedMem->PldAcpiS3Enable = 0;

      DEBUG ((DEBUG_INFO, "Created S3 communication HOB\n"));
    }
  }

  BlSmramInfo = FindCbTag (CB_TAG_PAYLOAD_MM_SMRAM_REGION);
  if (BlSmramInfo != NULL && PldMmSharedMem != NULL) {
    PldSmramHob = BuildGuidHob (
                    &gEfiSmmSmramMemoryGuid,
                    sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK) + sizeof (EFI_SMRAM_DESCRIPTOR)
                    );
    if (PldSmramHob != NULL) {
      PldSmramHob->NumberOfSmmReservedRegions = 2;

      // First, create an SMRAM descriptor to describe the shared memory.
      PldSmramHob->Descriptor[0].PhysicalStart = PldMmSharedMem->CommBuffer.PhysicalStart;
      PldSmramHob->Descriptor[0].CpuStart = PldMmSharedMem->CommBuffer.CpuStart;
      PldSmramHob->Descriptor[0].PhysicalSize = PldMmSharedMem->CommBuffer.PhysicalSize;
      PldSmramHob->Descriptor[0].RegionState = EFI_ALLOCATED;

      // Now, copy the primary SMRAM region to the HOB.
      PldSmramHob->Descriptor[1].PhysicalStart = cb_unpack64 (BlSmramInfo->descriptor.physical_start);
      PldSmramHob->Descriptor[1].CpuStart = cb_unpack64 (BlSmramInfo->descriptor.physical_start);
      PldSmramHob->Descriptor[1].PhysicalSize = cb_unpack64 (BlSmramInfo->descriptor.physical_size);
      PldSmramHob->Descriptor[1].RegionState = 0;

      DEBUG ((DEBUG_INFO, "Created SMM SMRAM memory info HOB\n"));
    }
  }

  BlSpiFlashInfo = FindCbTag (CB_TAG_PLD_MM_SPI_CONTROLLER_INFO);
  if (BlSpiFlashInfo != NULL) {
    PldSpiFlashInfo = BuildGuidHob (&gSpiFlashInfoGuid, sizeof (SPI_FLASH_INFO));
    if (PldSpiFlashInfo != NULL) {
      PldSpiFlashInfo->Revision = BlSpiFlashInfo->revision;
      PldSpiFlashInfo->Flags = BlSpiFlashInfo->flags;

      PldSpiFlashInfo->SpiAddress.AddressSpaceId = BlSpiFlashInfo->spi_address.address_space_id;
      PldSpiFlashInfo->SpiAddress.RegisterBitWidth = BlSpiFlashInfo->spi_address.register_bit_width;
      PldSpiFlashInfo->SpiAddress.RegisterBitOffset = BlSpiFlashInfo->spi_address.register_bit_offset;
      PldSpiFlashInfo->SpiAddress.Address = cb_unpack64 (BlSpiFlashInfo->spi_address.address);

      // Required for UefiPayload implementation compatibility
      PldSpiFlashInfo->SpiAddress.AccessSize = EFI_ACPI_3_0_DWORD;

      DEBUG ((DEBUG_INFO, "Created SPI flash info HOB\n"));
    }
  }

  // TODO: Split this code block out into another function?
  if (BlMmInfo != NULL) {
    MmIplBuildCommBufferHob ();

    //
    // Unblock the APICs and memory-based serial ports.
    // - FIXME: Is there a more dynamic way to do this?
    //
    Status = MmUnblockMemoryRequest (0xFEC00000, 1); // IOAPIC
    ASSERT_EFI_ERROR (Status);

    Status = MmUnblockMemoryRequest (0xFEE00000, 1); // LAPIC
    ASSERT_EFI_ERROR (Status);

    GuidHob = GetFirstGuidHob (&gUniversalPayloadSerialPortInfoGuid);
    if (GuidHob != NULL) {
      UniversalSerialPort = GET_GUID_HOB_DATA (GuidHob);
      if (UniversalSerialPort->UseMmio) {
        Status = MmUnblockMemoryRequest (UniversalSerialPort->RegisterBase, EFI_SIZE_TO_PAGES (8 * UniversalSerialPort->RegisterStride));
        ASSERT_EFI_ERROR (Status);
      }
    }

    CbMemConsole = FindCbTag (CB_TAG_CBMEM_CONSOLE);
    if (CbMemConsole != NULL) {
      Status = MmUnblockMemoryRequest (CbMemConsole->cbmem_addr, EFI_SIZE_TO_PAGES (((struct cbmem_console *)(UINTN)CbMemConsole->cbmem_addr)->size));
      ASSERT_EFI_ERROR (Status);
    }

    //
    // Unblock the SPI controller and related elements.
    // - FIXME: Again, is this optimal?
    //
    if (BlSpiFlashInfo != NULL && PldSpiFlashInfo != NULL) {
      Status = MmUnblockMemoryRequest (PldSpiFlashInfo->SpiAddress.Address, EFI_SIZE_TO_PAGES (256));
      ASSERT_EFI_ERROR (Status);

      Status = MmUnblockMemoryRequest (MmioRead32 (PldSpiFlashInfo->SpiAddress.Address + R_SPI_BASE) & ~(B_SPI_BAR0_MASK), 1);
      ASSERT_EFI_ERROR (Status);

      Status = MmUnblockMemoryRequest (0xFED30000, 1);
      ASSERT_EFI_ERROR (Status);
    }

    //
    // Initialise variable store and unblock the region.
    //
    Status = FmapLocateArea ("SMMSTORE", &SmmStoreFmapRegionAddress, &SmmStoreFmapRegionSize);
    if (!EFI_ERROR (Status)) {
      Status = InitVariableStore ((UINT32)SmmStoreFmapRegionAddress, SmmStoreFmapRegionSize);
      ASSERT_EFI_ERROR (Status);

      Status = BuildVariableRuntimeCacheInfoHob ();
      ASSERT_EFI_ERROR (Status);

      Status = MmUnblockMemoryRequest (SmmStoreFmapRegionAddress, EFI_SIZE_TO_PAGES (SmmStoreFmapRegionSize));
      ASSERT_EFI_ERROR (Status);

      DEBUG ((DEBUG_INFO, "Created variable flash info HOB\n"));
    }
  }

  return EFI_SUCCESS;
}
