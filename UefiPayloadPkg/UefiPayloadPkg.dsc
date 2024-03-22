## @file
# Bootloader Payload Package
#
# Provides drivers and definitions to create uefi payload for bootloaders.
#
# Copyright (c) 2014 - 2023, Intel Corporation. All rights reserved.<BR>
# Copyright (c) Microsoft Corporation.
# Copyright 2024 Google LLC
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                       = UefiPayloadPkg
  PLATFORM_GUID                       = F71608AB-D63D-4491-B744-A99998C8CD96
  PLATFORM_VERSION                    = 0.1
  DSC_SPECIFICATION                   = 0x00010005
  SUPPORTED_ARCHITECTURES             = IA32|X64|AARCH64
  BUILD_TARGETS                       = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER                    = DEFAULT
  BUILD_ARCH                          = Legacy
  OUTPUT_DIRECTORY                    = Build/UefiPayloadPkg$(BUILD_ARCH)
  FLASH_DEFINITION                    = UefiPayloadPkg/UefiPayloadPkg.fdf
  PCD_DYNAMIC_AS_DYNAMICEX            = TRUE

  #
  # Set when these packages are available.
  #
  DEFINE USE_EDK2_PLATFORMS           = FALSE

  #
  # Setup Universal Payload
  #
  # ELF: Build UniversalPayload file as UniversalPayload.elf
  # FIT: Build UniversalPayload file as UniversalPayload.fit
  #
  DEFINE UNIVERSAL_PAYLOAD            = FALSE
  DEFINE UNIVERSAL_PAYLOAD_FORMAT     = ELF

  #
  # Flat DeviceTree handoff option
  #
  # FALSE: Handover HobList to Payload
  # TRUE:  Handover FDT to Payload
  #
  DEFINE HAND_OFF_FDT_ENABLE          = FALSE

  #
  # SBL:      UEFI payload for Slim Bootloader
  # COREBOOT: UEFI payload for coreboot
  #
  DEFINE   BOOTLOADER                 = SBL

  #
  # Architectural drivers
  #
  DEFINE SMM_SUPPORT                  = FALSE
  DEFINE PAYLOAD_MM_SUPPORT           = FALSE
  DEFINE DISABLE_RESET_SYSTEM         = FALSE

  #
  # EMU:      UEFI payload with EMU variable
  # SPI:      UEFI payload with SPI NV variable support
  # SMMSTORE: UEFI payload with coreboot SMM NV variable support
  # NONE:     UEFI payload with no variable modules
  #
  DEFINE VARIABLE_SUPPORT             = EMU

  #
  # INTEL: SPI flash support using Intel's hardware sequencing
  # QEMU:  SPI flash support using QEMU's flash devices
  #
  DEFINE SPI_FLASH_SUPPORT            = INTEL

  #
  # Capsule updates
  #
  # CAPSULE_MAIN_FW_GUID specifies GUID to be used by FmpDxe when
  # CAPSULE_SUPPORT is set to TRUE
  #
  DEFINE CAPSULE_SUPPORT              = FALSE
  DEFINE CAPSULE_MAIN_FW_GUID         =

  # Define the maximum size of the capsule image without a reset flag that the platform can support.
  DEFINE MAX_SIZE_NON_POPULATE_CAPSULE = 0xa00000

  #
  # Crypto Support
  #
  DEFINE CRYPTO_PROTOCOL_SUPPORT        = FALSE
  DEFINE CRYPTO_DRIVER_EXTERNAL_SUPPORT = FALSE

  #
  # Security options
  #
  DEFINE SECURITY_STUB_ENABLE         = TRUE
  DEFINE SECURE_BOOT_ENABLE           = FALSE
  DEFINE TPM_ENABLE                   = FALSE

  #
  # Miscellaneous drivers
  #
  DEFINE LOCKBOX_SUPPORT              = FALSE
  DEFINE RAM_DISK_ENABLE              = FALSE

  #
  # NULL:    NullMemoryTestDxe
  # GENERIC: GenericMemoryTestDxe
  #
  DEFINE MEMORY_TEST                  = NULL

  #
  # User interface options
  #
  DEFINE USE_PLATFORM_GOP             = FALSE
  DEFINE BOOTSPLASH_IMAGE             = FALSE
  DEFINE BOOT_MANAGER_ESCAPE          = FALSE
  DEFINE PLATFORM_BOOT_TIMEOUT        = 3

  DEFINE CFR_SETUP_MENU_ENABLE        = FALSE

  #
  # Shell options: [BUILD_SHELL, NONE]
  #
  DEFINE SHELL_TYPE                   = BUILD_SHELL

  #
  # CPU options
  #
  DEFINE MAX_LOGICAL_PROCESSORS       = 1024
  DEFINE CPU_RNG_ENABLE               = TRUE

  # For recent X86 CPU, 0x15 CPUID instruction will return Time Stamp Counter Frequence.
  # This is how BaseCpuTimerLib works, and a recommended way to get Frequence, so set the default value as TRUE.
  # Note: for emulation platform such as QEMU, this may not work and should set it as FALSE
  DEFINE CPU_TIMER_LIB_ENABLE         = TRUE

  #
  # HPET:  UEFI Payload will use HPET timer
  # LAPIC: UEFI Payload will use local APIC timer
  #
  DEFINE TIMER_SUPPORT                = HPET

  #
  # PCI options
  #
  DEFINE PCIE_BASE_SUPPORT            = TRUE
  DEFINE LOAD_OPTION_ROMS             = FALSE

  #
  # Storage options
  #
  DEFINE ATA_ENABLE                   = TRUE
  DEFINE NVME_ENABLE                  = TRUE
  DEFINE SD_ENABLE                    = TRUE
  DEFINE SD_MMC_TIMEOUT               = 1000000

  DEFINE CSM_ENABLE                   = FALSE

  #
  # UEFI network modules
  #
  DEFINE NETWORK_DRIVER_ENABLE = FALSE

  #
  # PS/2 options
  #
  DEFINE SIO_BUS_ENABLE               = TRUE
  DEFINE PS2_KEYBOARD_ENABLE          = TRUE
  DEFINE PS2_MOUSE_ENABLE             = TRUE

  # Define RTC related register.
  DEFINE RTC_INDEX_REGISTER  = 0x70
  DEFINE RTC_TARGET_REGISTER = 0x71

  #
  # Debug options
  #
  DEFINE SOURCE_DEBUG_ENABLE          = FALSE
  DEFINE SOURCE_DEBUG_USE_USB         = FALSE
  DEFINE SOURCE_DEBUG_USE_USB3        = FALSE
  DEFINE RELEASE_LOGGING              = FALSE
  DEFINE ENABLE_DEBUG_CODE            = FALSE
  DEFINE MULTIPLE_DEBUG_PORT_SUPPORT  = FALSE
  DEFINE USE_CBMEM_FOR_CONSOLE        = FALSE

  DEFINE PERFORMANCE_MEASUREMENT_ENABLE = FALSE
  DEFINE MEMORY_PROFILE_ENABLE          = FALSE

  #
  # Serial port set up
  #
  DEFINE BAUD_RATE                    = 115200
  DEFINE SERIAL_CLOCK_RATE            = 1843200
  DEFINE SERIAL_LINE_CONTROL          = 3 # 8-bits, no parity
  DEFINE SERIAL_HARDWARE_FLOW_CONTROL = FALSE
  DEFINE SERIAL_DETECT_CABLE          = FALSE
  DEFINE SERIAL_FIFO_CONTROL          = 7 # Enable FIFO
  DEFINE UART_DEFAULT_BAUD_RATE       = $(BAUD_RATE)
  DEFINE UART_DEFAULT_DATA_BITS       = 8
  DEFINE UART_DEFAULT_PARITY          = 1
  DEFINE UART_DEFAULT_STOP_BITS       = 1
  DEFINE DEFAULT_TERMINAL_TYPE        = 0

  DEFINE SERIAL_DRIVER_ENABLE         = TRUE

  # Enabling the serial terminal will slow down the boot menu rendering!
  DEFINE DISABLE_SERIAL_TERMINAL      = FALSE

  #
  #  typedef struct {
  #    UINT16  VendorId;          ///< Vendor ID to match the PCI device.  The value 0xFFFF terminates the list of entries.
  #    UINT16  DeviceId;          ///< Device ID to match the PCI device
  #    UINT32  ClockRate;         ///< UART clock rate.  Set to 0 for default clock rate of 1843200 Hz
  #    UINT64  Offset;            ///< The byte offset into to the BAR
  #    UINT8   BarIndex;          ///< Which BAR to get the UART base address
  #    UINT8   RegisterStride;    ///< UART register stride in bytes.  Set to 0 for default register stride of 1 byte.
  #    UINT16  ReceiveFifoDepth;  ///< UART receive FIFO depth in bytes. Set to 0 for a default FIFO depth of 16 bytes.
  #    UINT16  TransmitFifoDepth; ///< UART transmit FIFO depth in bytes. Set to 0 for a default FIFO depth of 16 bytes.
  #    UINT8   Reserved[2];
  #  } PCI_SERIAL_PARAMETER;
  #
  # Vendor FFFF Device 0000 Prog Interface 1, BAR #0, Offset 0, Stride = 1, Clock 1843200 (0x1c2000)
  #
  #                                       [Vendor]   [Device]  [----ClockRate---]  [------------Offset-----------] [Bar] [Stride] [RxFifo] [TxFifo]   [Rsvd]   [Vendor]
  DEFINE PCI_SERIAL_PARAMETERS        = {0xff,0xff, 0x00,0x00, 0x0,0x20,0x1c,0x00, 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x00,    0x01, 0x0,0x0, 0x0,0x0, 0x0,0x0, 0xff,0xff}

[BuildOptions]
  *_*_*_CC_FLAGS                 = -D DISABLE_NEW_DEPRECATED_INTERFACES
!if $(RELEASE_LOGGING) == FALSE
  GCC:RELEASE_*_*_CC_FLAGS       = -DMDEPKG_NDEBUG
  INTEL:RELEASE_*_*_CC_FLAGS     = /D MDEPKG_NDEBUG
  MSFT:RELEASE_*_*_CC_FLAGS      = /D MDEPKG_NDEBUG
!endif
!if $(SOURCE_DEBUG_ENABLE) == TRUE
  GCC:*_*_X64_GENFW_FLAGS   = --keepexceptiontable
  INTEL:*_*_X64_GENFW_FLAGS = --keepexceptiontable
  MSFT:*_*_X64_GENFW_FLAGS  = --keepexceptiontable
!endif
!if $(MEMORY_PROFILE_ENABLE) == TRUE
  MSFT:*_*_*_CC_FLAGS          = /Od
  MSFT:RELEASE_*_*_DLINK_FLAGS = /DEBUG
  MSFT:RELEASE_*_*_CC_FLAGS    = /Zi
!endif
  *_*_*_CC_FLAGS                 = $(APPEND_CC_FLAGS)

[BuildOptions.AARCH64]
  GCC:*_*_*_CC_FLAGS         = -mcmodel=tiny -mstrict-align

[BuildOptions.common.EDKII.DXE_RUNTIME_DRIVER, BuildOptions.common.EDKII.SMM_CORE, BuildOptions.common.EDKII.DXE_SMM_DRIVER, BuildOptions.common.EDKII.MM_CORE_STANDALONE, BuildOptions.common.EDKII.MM_STANDALONE]
  GCC:*_*_*_DLINK_FLAGS      = -z common-page-size=0x1000
  XCODE:*_*_*_DLINK_FLAGS    = -seg1addr 0x1000 -segalign 0x1000
  XCODE:*_*_*_MTOC_FLAGS     = -align 0x1000
  CLANGPDB:*_*_*_DLINK_FLAGS = /ALIGN:4096
  MSFT:*_*_*_DLINK_FLAGS     = /ALIGN:4096

[BuildOptions.AARCH64.EDKII.DXE_RUNTIME_DRIVER]
  GCC:*_*_*_DLINK_FLAGS      = -z common-page-size=0x10000

################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################

!include MdePkg/MdeLibs.dsc.inc

[LibraryClasses]
  #
  # Entry point
  #
  DxeCoreEntryPoint|MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf

  #
  # Basic
  #
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
!if $(PCIE_BASE_SUPPORT) == FALSE
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
!else
  PciLib|MdePkg/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
  PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
!endif
  PciSegmentLib|MdePkg/Library/PciSegmentLibSegmentInfo/BasePciSegmentLibSegmentInfo.inf
  PciSegmentInfoLib|UefiPayloadPkg/Library/PciSegmentInfoLibAcpiBoardInfo/PciSegmentInfoLibAcpiBoardInfo.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
  DxeHobListLib|UefiPayloadPkg/Library/DxeHobListLib/DxeHobListLib.inf
  HobLib|UefiPayloadPkg/Library/DxeHobLib/DxeHobLib.inf
  CustomFdtNodeParserLib|UefiPayloadPkg/Library/CustomFdtNodeParserNullLib/CustomFdtNodeParserNullLib.inf

  #
  # UEFI & PI
  #
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf

  #
  # Generic Modules
  #
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
!if $(CAPSULE_SUPPORT) == TRUE
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeCapsuleLib.inf
  !if $(BOOTSPLASH_IMAGE) == TRUE
    # Note, however, that DisplayUpdateProgressLibGraphics aborts firmware
    # update if GOP is missing, so text progress works in more environments.
    DisplayUpdateProgressLib|MdeModulePkg/Library/DisplayUpdateProgressLibGraphics/DisplayUpdateProgressLibGraphics.inf
  !else
    DisplayUpdateProgressLib|MdeModulePkg/Library/DisplayUpdateProgressLibText/DisplayUpdateProgressLibText.inf
  !endif
  # If there are no specific checks to do, null-library suffices
  CapsuleUpdatePolicyLib|FmpDevicePkg/Library/CapsuleUpdatePolicyLibNull/CapsuleUpdatePolicyLibNull.inf
  FmpAuthenticationLib|SecurityPkg/Library/FmpAuthenticationLibPkcs7/FmpAuthenticationLibPkcs7.inf
  FmpDependencyCheckLib|FmpDevicePkg/Library/FmpDependencyCheckLib/FmpDependencyCheckLib.inf
  # No need to save/restore FMP dependencies unless they are utilized
  FmpDependencyDeviceLib|FmpDevicePkg/Library/FmpDependencyDeviceLibNull/FmpDependencyDeviceLibNull.inf
  FmpDependencyLib|FmpDevicePkg/Library/FmpDependencyLib/FmpDependencyLib.inf
  FmpPayloadHeaderLib|FmpDevicePkg/Library/FmpPayloadHeaderLibV1/FmpPayloadHeaderLibV1.inf
!else
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
!endif
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  BmpSupportLib|MdeModulePkg/Library/BaseBmpSupportLib/BaseBmpSupportLib.inf
  BootLogoLib|MdeModulePkg/Library/BootLogoLib/BootLogoLib.inf
  CustomizedDisplayLib|MdeModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf
  FrameBufferBltLib|MdeModulePkg/Library/FrameBufferBltLib/FrameBufferBltLib.inf
  ResetSystemLib|UefiPayloadPkg/Library/ResetSystemLib/ResetSystemLib.inf
!if $(USE_CBMEM_FOR_CONSOLE) == TRUE
  SerialPortLib|UefiPayloadPkg/Library/CbSerialPortLib/CbSerialPortLib.inf
  PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
!else
  !if $(MULTIPLE_DEBUG_PORT_SUPPORT) == TRUE
    SerialPortLib|UefiPayloadPkg/Library/BaseSerialPortLibHob/DxeBaseSerialPortLibHob.inf
  !else
    SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
  !endif
  PlatformHookLib|UefiPayloadPkg/Library/PlatformHookLib/PlatformHookLib.inf
!endif
  PlatformBootManagerLib|UefiPayloadPkg/Library/PlatformBootManagerLib/PlatformBootManagerLib.inf

  #
  # Misc
  #
  DebugPrintErrorLevelLib|UefiPayloadPkg/Library/DebugPrintErrorLevelLibHob/DebugPrintErrorLevelLibHob.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  ImagePropertiesRecordLib|MdeModulePkg/Library/ImagePropertiesRecordLib/ImagePropertiesRecordLib.inf
!if $(SOURCE_DEBUG_ENABLE) == TRUE
  PeCoffExtraActionLib|SourceLevelDebugPkg/Library/PeCoffExtraActionLibDebug/PeCoffExtraActionLibDebug.inf
!if $(SOURCE_DEBUG_USE_USB) == TRUE
  DebugCommunicationLib|SourceLevelDebugPkg/Library/DebugCommunicationLibUsb/DebugCommunicationLibUsb.inf
!elseif $(SOURCE_DEBUG_USE_USB3) == FALSE
  DebugCommunicationLib|SourceLevelDebugPkg/Library/DebugCommunicationLibSerialPort/DebugCommunicationLibSerialPort.inf
!endif
!else
  PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
!endif
!if $(BOOTLOADER) == "COREBOOT"
  PlatformSupportLib|UefiPayloadPkg/Library/PlatformSupportLibCb/PlatformSupportLibCb.inf
!else
  PlatformSupportLib|UefiPayloadPkg/Library/PlatformSupportLibNull/PlatformSupportLibNull.inf
!endif
!if $(UNIVERSAL_PAYLOAD) == FALSE
  !if $(BOOTLOADER) == "COREBOOT"
    BlParseLib|UefiPayloadPkg/Library/CbParseLib/CbParseLib.inf
    FmapParserLib|UefiPayloadPkg/Library/CbFmapParserLib/CbFmapParserLib.inf
  !else
    BlParseLib|UefiPayloadPkg/Library/SblParseLib/SblParseLib.inf
  !endif
!endif
  PayloadMmHelperLib|UefiPayloadPkg/Library/PayloadMmHelperLib/PayloadMmHelperLib.inf
!if $(CFR_SETUP_MENU_ENABLE) == TRUE
  CfrHelpersLib|UefiPayloadPkg/Library/CfrHelpersLib/CfrHelpersLib.inf
!endif

  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  DebugLib|MdeModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
!if $(LOCKBOX_SUPPORT) == TRUE
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxDxeLib.inf
!else
  LockBoxLib|MdeModulePkg/Library/LockBoxNullLib/LockBoxNullLib.inf
!endif
  FileExplorerLib|MdeModulePkg/Library/FileExplorerLib/FileExplorerLib.inf

  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
!if $(CRYPTO_PROTOCOL_SUPPORT) == TRUE
  BaseCryptLib|CryptoPkg/Library/BaseCryptLibOnProtocolPpi/DxeCryptLib.inf
  TlsLib|CryptoPkg/Library/BaseCryptLibOnProtocolPpi/DxeCryptLib.inf
!else
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
  TlsLib|CryptoPkg/Library/TlsLib/TlsLib.inf
!endif
!if $(CPU_RNG_ENABLE) == TRUE
  RngLib|MdePkg/Library/BaseRngLib/BaseRngLib.inf
!else
  RngLib|MdePkg/Library/BaseRngLibTimerLib/BaseRngLibTimerLib.inf
!endif

!if $(SECURE_BOOT_ENABLE) == TRUE
  PlatformSecureLib|SecurityPkg/Library/PlatformSecureLibNull/PlatformSecureLibNull.inf
  AuthVariableLib|SecurityPkg/Library/AuthVariableLib/AuthVariableLib.inf
  SecureBootVariableLib|SecurityPkg/Library/SecureBootVariableLib/SecureBootVariableLib.inf
  PlatformPKProtectionLib|SecurityPkg/Library/PlatformPKProtectionLibVarPolicy/PlatformPKProtectionLibVarPolicy.inf
  SecureBootVariableProvisionLib|SecurityPkg/Library/SecureBootVariableProvisionLib/SecureBootVariableProvisionLib.inf
!else
  AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
!endif

!if $(TPM_ENABLE) == TRUE
  Tpm12CommandLib|SecurityPkg/Library/Tpm12CommandLib/Tpm12CommandLib.inf
  Tpm2CommandLib|SecurityPkg/Library/Tpm2CommandLib/Tpm2CommandLib.inf
  Tcg2PhysicalPresenceLib|OvmfPkg/Library/Tcg2PhysicalPresenceLibQemu/DxeTcg2PhysicalPresenceLib.inf
  Tcg2PhysicalPresencePlatformLib|UefiPayloadPkg/Library/Tcg2PhysicalPresencePlatformLibUefipayload/DxeTcg2PhysicalPresencePlatformLib.inf
  TcgPpVendorLib|SecurityPkg/Library/TcgPpVendorLibNull/TcgPpVendorLibNull.inf
  Tcg2PpVendorLib|SecurityPkg/Library/Tcg2PpVendorLibNull/Tcg2PpVendorLibNull.inf
  TpmMeasurementLib|SecurityPkg/Library/DxeTpmMeasurementLib/DxeTpmMeasurementLib.inf
!else
  TpmMeasurementLib|MdeModulePkg/Library/TpmMeasurementLibNull/TpmMeasurementLibNull.inf
  Tcg2PhysicalPresenceLib|OvmfPkg/Library/Tcg2PhysicalPresenceLibNull/DxeTcg2PhysicalPresenceLib.inf
!endif

!if $(VARIABLE_SUPPORT) == "SMMSTORE" || ($(CAPSULE_SUPPORT) && $(BOOTLOADER) == "COREBOOT")
  SmmStoreLib|UefiPayloadPkg/Library/SmmStoreLib/SmmStoreLib.inf
!endif
  VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf
  VariablePolicyLib|MdeModulePkg/Library/VariablePolicyLib/VariablePolicyLib.inf
  VariablePolicyHelperLib|MdeModulePkg/Library/VariablePolicyHelperLib/VariablePolicyHelperLib.inf
  VariableFlashInfoLib|MdeModulePkg/Library/BaseVariableFlashInfoLib/BaseVariableFlashInfoLib.inf
  CcExitLib|UefiCpuPkg/Library/CcExitLibNull/CcExitLibNull.inf
  AmdSvsmLib|UefiCpuPkg/Library/AmdSvsmLibNull/AmdSvsmLibNull.inf
  FdtLib|MdePkg/Library/BaseFdtLib/BaseFdtLib.inf
  SmmRelocationLib|UefiCpuPkg/Library/SmmRelocationLib/SmmRelocationLib.inf
  HobPrintLib|MdeModulePkg/Library/HobPrintLib/HobPrintLib.inf
  BuildFdtLib|UefiPayloadPkg/Library/BuildFdtLib/BuildFdtLib.inf
  MmSaveStateLib|UefiCpuPkg/Library/MmSaveStateLib/IntelMmSaveStateLib.inf
  SmmCpuSyncLib|UefiCpuPkg/Library/SmmCpuSyncLib/SmmCpuSyncLib.inf

[LibraryClasses.common]
  UefiCpuBaseArchSupportLib|UefiCpuPkg/Library/BaseArchSupportLib/BaseArchSupportLib.inf

[LibraryClasses.X64]
  #
  # CPU
  #
  MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  LocalApicLib|UefiCpuPkg/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf
  MicrocodeLib|UefiCpuPkg/Library/MicrocodeLib/MicrocodeLib.inf
  IoApicLib|PcAtChipsetPkg/Library/BaseIoApicLib/BaseIoApicLib.inf
!if $(CPU_TIMER_LIB_ENABLE) == TRUE && $(UNIVERSAL_PAYLOAD) == TRUE
  TimerLib|UefiCpuPkg/Library/CpuTimerLib/BaseCpuTimerLib.inf
!else
  TimerLib|UefiPayloadPkg/Library/AcpiTimerLib/AcpiTimerLib.inf
!endif
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/DxeCpuExceptionHandlerLib.inf
  CpuPageTableLib|UefiCpuPkg/Library/CpuPageTableLib/CpuPageTableLib.inf

[LibraryClasses.AARCH64]
  ArmHvcLib|ArmPkg/Library/ArmHvcLib/ArmHvcLib.inf
  ArmLib|ArmPkg/Library/ArmLib/ArmBaseLib.inf
  ArmMmuLib|UefiCpuPkg/Library/ArmMmuLib/ArmMmuBaseLib.inf
  ArmMonitorLib|ArmPkg/Library/ArmMonitorLib/ArmMonitorLib.inf
  ArmSmcLib|MdePkg/Library/ArmSmcLib/ArmSmcLib.inf

  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  CacheMaintenanceLib|ArmPkg/Library/ArmCacheMaintenanceLib/ArmCacheMaintenanceLib.inf
  ResetSystemLib|ArmPkg/Library/ArmPsciResetSystemLib/ArmPsciResetSystemLib.inf
  PL011UartLib|ArmPlatformPkg/Library/PL011UartLib/PL011UartLib.inf
  PL011UartClockLib|ArmPlatformPkg/Library/PL011UartClockLib/PL011UartClockLib.inf
  SerialPortLib|ArmPlatformPkg/Library/PL011SerialPortLib/PL011SerialPortLib.inf

  # ARM Architectural Libraries
  TimerLib|ArmPkg/Library/ArmArchTimerLib/ArmArchTimerLib.inf
  ArmGenericTimerCounterLib|ArmPkg/Library/ArmGenericTimerPhyCounterLib/ArmGenericTimerPhyCounterLib.inf
  CpuExceptionHandlerLib|ArmPkg/Library/ArmExceptionLib/ArmExceptionLib.inf
  DefaultExceptionHandlerLib|ArmPkg/Library/DefaultExceptionHandlerLib/DefaultExceptionHandlerLib.inf

  # ARM PL031 RTC Driver
  RealTimeClockLib|ArmPlatformPkg/Library/PL031RealTimeClockLib/PL031RealTimeClockLib.inf
  TimeBaseLib|EmbeddedPkg/Library/TimeBaseLib/TimeBaseLib.inf

  # ARM on QEMU platform
  QemuFwCfgLib|OvmfPkg/Library/QemuFwCfgLib/QemuFwCfgMmioDxeLib.inf
  VirtNorFlashDeviceLib|OvmfPkg/Library/VirtNorFlashDeviceLib/VirtNorFlashDeviceLib.inf
  VirtNorFlashPlatformLib|OvmfPkg/Library/FdtNorFlashQemuLib/FdtNorFlashQemuLib.inf

  VirtioMmioDeviceLib|OvmfPkg/Library/VirtioMmioDeviceLib/VirtioMmioDeviceLib.inf
  VirtioLib|OvmfPkg/Library/VirtioLib/VirtioLib.inf

  # PCI Libraries
  PciCapLib|OvmfPkg/Library/BasePciCapLib/BasePciCapLib.inf
  PciCapPciSegmentLib|OvmfPkg/Library/BasePciCapPciSegmentLib/BasePciCapPciSegmentLib.inf
  PciCapPciIoLib|OvmfPkg/Library/UefiPciCapPciIoLib/UefiPciCapPciIoLib.inf

[LibraryClasses.common.SEC]
  HobLib|UefiPayloadPkg/Library/PayloadEntryHobLib/HobLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  DxeHobListLib|UefiPayloadPkg/Library/DxeHobListLibNull/DxeHobListLibNull.inf
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!if $(MULTIPLE_DEBUG_PORT_SUPPORT) == TRUE
  SerialPortLib|UefiPayloadPkg/Library/BaseSerialPortLibHob/BaseSerialPortLibHob.inf
!endif

[LibraryClasses.common.DXE_CORE]
  DxeHobListLib|UefiPayloadPkg/Library/DxeHobListLibNull/DxeHobListLibNull.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
!if $(MEMORY_PROFILE_ENABLE) == TRUE
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationProfileLib.inf
  MemoryProfileLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationProfileLib.inf
!else
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
!endif
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
!if $(SOURCE_DEBUG_ENABLE) == TRUE
  DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/DxeDebugAgentLib.inf
!if $(SOURCE_DEBUG_USE_USB3) == TRUE
  DebugCommunicationLib|SourceLevelDebugPkg/Library/DebugCommunicationLibUsb3/DebugCommunicationLibUsb3Dxe.inf
!endif
!endif
!if $(PERFORMANCE_MEASUREMENT_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf
!endif

[LibraryClasses.common.DXE_DRIVER]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
!if $(MEMORY_PROFILE_ENABLE) == TRUE
  MemoryAllocationLib|MdeModulePkg/Library/UefiMemoryAllocationProfileLib/UefiMemoryAllocationProfileLib.inf
  MemoryProfileLib|MdeModulePkg/Library/UefiMemoryAllocationProfileLib/UefiMemoryAllocationProfileLib.inf
!else
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
!endif
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
!if $(SOURCE_DEBUG_ENABLE) == TRUE
  DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/DxeDebugAgentLib.inf
!if $(SOURCE_DEBUG_USE_USB3) == TRUE
  DebugCommunicationLib|SourceLevelDebugPkg/Library/DebugCommunicationLibUsb3/DebugCommunicationLibUsb3Dxe.inf
!endif
!endif
!if $(PERFORMANCE_MEASUREMENT_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
!endif
!if $(TPM_ENABLE) == TRUE
  Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibTcg/Tpm12DeviceLibTcg.inf
  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibTcg2/Tpm2DeviceLibTcg2.inf
!endif

[LibraryClasses.X64.DXE_DRIVER]
  MpInitLib|UefiCpuPkg/Library/MpInitLib/DxeMpInitLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
!if $(SECURE_BOOT_ENABLE) == TRUE
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/RuntimeCryptLib.inf
!endif
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
!if $(MEMORY_PROFILE_ENABLE) == TRUE
  MemoryAllocationLib|MdeModulePkg/Library/UefiMemoryAllocationProfileLib/UefiMemoryAllocationProfileLib.inf
  MemoryProfileLib|MdeModulePkg/Library/UefiMemoryAllocationProfileLib/UefiMemoryAllocationProfileLib.inf
!else
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
!endif
  ReportStatusCodeLib|MdeModulePkg/Library/RuntimeDxeReportStatusCodeLib/RuntimeDxeReportStatusCodeLib.inf
  VariablePolicyLib|MdeModulePkg/Library/VariablePolicyLib/VariablePolicyLibRuntimeDxe.inf
!if $(PERFORMANCE_MEASUREMENT_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
!endif
!if $(CAPSULE_SUPPORT) == TRUE
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeRuntimeCapsuleLib.inf
!endif

[LibraryClasses.common.UEFI_DRIVER,LibraryClasses.common.UEFI_APPLICATION]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
!if $(MEMORY_PROFILE_ENABLE) == TRUE
  MemoryAllocationLib|MdeModulePkg/Library/UefiMemoryAllocationProfileLib/UefiMemoryAllocationProfileLib.inf
  MemoryProfileLib|MdeModulePkg/Library/UefiMemoryAllocationProfileLib/UefiMemoryAllocationProfileLib.inf
!else
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
!endif
!if $(PERFORMANCE_MEASUREMENT_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
!endif

[LibraryClasses.X64.SMM_CORE]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  SmmServicesTableLib|MdeModulePkg/Library/PiSmmCoreSmmServicesTableLib/PiSmmCoreSmmServicesTableLib.inf

!if $(MEMORY_PROFILE_ENABLE) == TRUE
  MemoryAllocationLib|MdeModulePkg/Library/PiSmmCoreMemoryAllocationLib/PiSmmCoreMemoryAllocationProfileLib.inf
  MemoryProfileLib|MdeModulePkg/Library/PiSmmCoreMemoryAllocationLib/PiSmmCoreMemoryAllocationProfileLib.inf
!else
  MemoryAllocationLib|MdeModulePkg/Library/PiSmmCoreMemoryAllocationLib/PiSmmCoreMemoryAllocationLib.inf
!endif
  SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf
  SmmCorePlatformHookLib|MdeModulePkg/Library/SmmCorePlatformHookLibNull/SmmCorePlatformHookLibNull.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
!if $(PERFORMANCE_MEASUREMENT_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/SmmCorePerformanceLib/SmmCorePerformanceLib.inf
!endif

[LibraryClasses.X64.DXE_SMM_DRIVER]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  MmServicesTableLib|MdePkg/Library/MmServicesTableLib/MmServicesTableLib.inf
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf

!if $(MEMORY_PROFILE_ENABLE) == TRUE
  MemoryAllocationLib|MdeModulePkg/Library/SmmMemoryAllocationProfileLib/SmmMemoryAllocationProfileLib.inf
  MemoryProfileLib|MdeModulePkg/Library/SmmMemoryAllocationProfileLib/SmmMemoryAllocationProfileLib.inf
!else
  MemoryAllocationLib|MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf
!endif
  SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf
  SmmCpuPlatformHookLib|UefiCpuPkg/Library/SmmCpuPlatformHookLibNull/SmmCpuPlatformHookLibNull.inf
  SmmCpuFeaturesLib|UefiCpuPkg/Library/SmmCpuFeaturesLib/SmmCpuFeaturesLib.inf
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/SmmCpuExceptionHandlerLib.inf
  SmmCpuRendezvousLib|UefiCpuPkg/Library/SmmCpuRendezvousLib/SmmCpuRendezvousLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
!if $(PERFORMANCE_MEASUREMENT_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/SmmPerformanceLib/SmmPerformanceLib.inf
!endif
!if $(VARIABLE_SUPPORT) == "SPI"
!if $(SPI_FLASH_SUPPORT) == "INTEL"
  SpiFlashLib|UefiPayloadPkg/Library/SpiFlashLib/SpiFlashLib.inf
!elseif $(SPI_FLASH_SUPPORT) == "QEMU"
  SpiFlashLib|UefiPayloadPkg/Library/SpiFlashLibQemu/SpiFlashLibQemu.inf
!endif
  FlashDeviceLib|UefiPayloadPkg/Library/FlashDeviceLib/FlashDeviceLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf
!endif

[LibraryClasses.X64.MM_CORE_STANDALONE]
  ExtractGuidedSectionLib|StandaloneMmPkg/Library/StandaloneMmExtractGuidedSectionLib/StandaloneMmExtractGuidedSectionLib.inf
  FvLib|StandaloneMmPkg/Library/FvLib/FvLib.inf
  HobLib|StandaloneMmPkg/Library/StandaloneMmCoreHobLib/StandaloneMmCoreHobLib.inf
  MemLib|StandaloneMmPkg/Library/StandaloneMmMemLib/StandaloneMmMemLib.inf
  MemoryAllocationLib|StandaloneMmPkg/Library/StandaloneMmCoreMemoryAllocationLib/StandaloneMmCoreMemoryAllocationLib.inf
  MmServicesTableLib|MdePkg/Library/StandaloneMmServicesTableLib/StandaloneMmServicesTableLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/StandaloneMmReportStatusCodeLib.inf
  PerformanceLib|MdeModulePkg/Library/SmmCorePerformanceLib/StandaloneMmCorePerformanceLib.inf

  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/SmmCpuExceptionHandlerLib.inf

  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLibBase.inf

[LibraryClasses.X64.MM_STANDALONE]
  FvLib|StandaloneMmPkg/Library/FvLib/FvLib.inf
  HobLib|StandaloneMmPkg/Library/StandaloneMmHobLib/StandaloneMmHobLib.inf
  MemLib|StandaloneMmPkg/Library/StandaloneMmMemLib/StandaloneMmMemLib.inf
  MemoryAllocationLib|StandaloneMmPkg/Library/StandaloneMmMemoryAllocationLib/StandaloneMmMemoryAllocationLib.inf
  MmServicesTableLib|MdePkg/Library/StandaloneMmServicesTableLib/StandaloneMmServicesTableLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/StandaloneMmReportStatusCodeLib.inf
  PerformanceLib|MdeModulePkg/Library/SmmPerformanceLib/StandaloneMmPerformanceLib.inf

  StandaloneMmDriverEntryPoint|MdePkg/Library/StandaloneMmDriverEntryPoint/StandaloneMmDriverEntryPoint.inf
  SmmCpuPlatformHookLib|UefiCpuPkg/Library/SmmCpuPlatformHookLibNull/SmmCpuPlatformHookLibNull.inf
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/SmmCpuExceptionHandlerLib.inf

  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLibBase.inf
!if $(VARIABLE_SUPPORT) == "SPI"
!if $(SPI_FLASH_SUPPORT) == "INTEL"
  SpiFlashLib|UefiPayloadPkg/Library/SpiFlashLib/SpiFlashLib.inf
!elseif $(SPI_FLASH_SUPPORT) == "QEMU"
  SpiFlashLib|UefiPayloadPkg/Library/SpiFlashLibQemu/SpiFlashLibQemu.inf
!endif
  FlashDeviceLib|UefiPayloadPkg/Library/FlashDeviceLib/FlashDeviceLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf
!endif

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform.
#
################################################################################
[PcdsFeatureFlag]
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE
  ## This PCD specified whether ACPI SDT protocol is installed.
  gEfiMdeModulePkgTokenSpaceGuid.PcdInstallAcpiSdtProtocol|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdHiiOsRuntimeSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdPciDegradeResourceForOptionRom|FALSE
  ## Whether capsules are allowed to persist across reset.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSupportUpdateCapsuleReset|$(CAPSULE_SUPPORT)

[PcdsFeatureFlag.X64]
  gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmEnableBspElection|FALSE

[PcdsFixedAtBuild]
  gEfiMdePkgTokenSpaceGuid.PcdHardwareErrorRecordLevel|1
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x10000
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxHardwareErrorVariableSize|0x8000
  gEfiMdeModulePkgTokenSpaceGuid.PcdVariableStoreSize|0x10000
!if $(VARIABLE_SUPPORT) == "EMU"
  gEfiMdeModulePkgTokenSpaceGuid.PcdEmuVariableNvModeEnable        |TRUE
!elseif $(VARIABLE_SUPPORT) == "SPI" || $(VARIABLE_SUPPORT) == "SMMSTORE"
  gEfiMdeModulePkgTokenSpaceGuid.PcdEmuVariableNvModeEnable        |FALSE
!endif

  gEfiMdeModulePkgTokenSpaceGuid.PcdVpdBaseAddress|0x0
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseMemory|FALSE
!if ($(TARGET) == DEBUG || $(RELEASE_LOGGING) == TRUE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|TRUE
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|FALSE
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdUse1GPageTable|TRUE
  gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmStackSize|0x4000
  gEfiMdeModulePkgTokenSpaceGuid.PcdMmCommBufferPages|24
  gUefiPayloadPkgTokenSpaceGuid.PcdHandOffFdtEnable|$(HAND_OFF_FDT_ENABLE)

  gEfiMdeModulePkgTokenSpaceGuid.PcdBootManagerMenuFile|{ 0x21, 0xaa, 0x2c, 0x46, 0x14, 0x76, 0x03, 0x45, 0x83, 0x6e, 0x8a, 0xb6, 0xf4, 0x66, 0x23, 0x31 }
  gUefiPayloadPkgTokenSpaceGuid.PcdPcdDriverFile|{ 0x57, 0x72, 0xcf, 0x80, 0xab, 0x87, 0xf9, 0x47, 0xa3, 0xfe, 0xD5, 0x0B, 0x76, 0xd8, 0x95, 0x41 }

  #
  # Build the PcdDebugPropertyMask from build flags
  #
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x03
!if $(RELEASE_LOGGING) == TRUE
  !if $(ENABLE_DEBUG_CODE) == TRUE
    gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x07
  !endif
!else
  !if $(SOURCE_DEBUG_ENABLE) == TRUE
    !if $(ENABLE_DEBUG_CODE) == TRUE
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x17
    !else
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x13
    !endif
  !else
    !if $(ENABLE_DEBUG_CODE) == TRUE
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2F
    !else
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2B
    !endif
  !endif
!endif

!if $(MEMORY_PROFILE_ENABLE) == TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdMemoryProfileMemoryType|0x0679
  !if $(SMM_SUPPORT) == TRUE
    gEfiMdeModulePkgTokenSpaceGuid.PcdMemoryProfilePropertyMask|0x03
  !else
    gEfiMdeModulePkgTokenSpaceGuid.PcdMemoryProfilePropertyMask|0x01
  !endif
!endif

!if $(SOURCE_DEBUG_ENABLE) == TRUE
  gEfiSourceLevelDebugPkgTokenSpaceGuid.PcdDebugLoadImageMethod|0x2
!endif

  gEfiMdeModulePkgTokenSpaceGuid.PcdEdkiiFpdtStringRecordEnableOnly| TRUE
!if $(PERFORMANCE_MEASUREMENT_ENABLE) == TRUE
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask       | 0x1
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdSdMmcGenericTimeoutValue|$(SD_MMC_TIMEOUT)

  gUefiPayloadPkgTokenSpaceGuid.PcdBootManagerEscape|$(BOOT_MANAGER_ESCAPE)

  gEfiMdePkgTokenSpaceGuid.PcdMaximumUnicodeStringLength|1800000

  ## Whether FMP capsules are enabled.
  gEfiMdeModulePkgTokenSpaceGuid.PcdCapsuleFmpSupport|$(CAPSULE_SUPPORT)

!if $(CRYPTO_PROTOCOL_SUPPORT) == TRUE
!if $(CRYPTO_DRIVER_EXTERNAL_SUPPORT) == FALSE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.HmacSha256.Family                        | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Md5.Family                               | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Pkcs.Family                              | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Dh.Family                                | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Random.Family                            | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Rsa.Family                               | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Sha1.Family                              | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Sha256.Family                            | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Sha384.Family                            | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Sha512.Family                            | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.X509.Family                              | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Aes.Services.GetContextSize              | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Aes.Services.Init                        | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Aes.Services.CbcEncrypt                  | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Aes.Services.CbcDecrypt                  | TRUE
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Sm3.Family                               | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Hkdf.Family                              | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.Tls.Family                               | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.TlsSet.Family                            | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
  gEfiCryptoPkgTokenSpaceGuid.PcdCryptoServiceFamilyEnable.TlsGet.Family                            | PCD_CRYPTO_SERVICE_ENABLE_FAMILY
!endif
!endif

!if $(SECURE_BOOT_ENABLE) == TRUE
  # Override the default values from SecurityPkg to ensure images from all sources are verified in secure boot
  gEfiSecurityPkgTokenSpaceGuid.PcdOptionRomImageVerificationPolicy|0x04
  gEfiSecurityPkgTokenSpaceGuid.PcdFixedMediaImageVerificationPolicy|0x04
  gEfiSecurityPkgTokenSpaceGuid.PcdRemovableMediaImageVerificationPolicy|0x04
!endif

!if $(TPM_ENABLE) == TRUE
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2AcpiTableRev|4
!endif

  # Enable NX memory protection for all non-code regions, including OEM and OS
  # reserved ones, with the exception of LoaderData regions, of which OS loaders
  # (i.e., GRUB) may assume that its contents are executable.
  gEfiMdeModulePkgTokenSpaceGuid.PcdDxeNxMemoryProtectionPolicy|0xC000000000007FD1

  # Enable the non-executable DXE stack. (This gets set up by DxeIpl)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetNxForStack|TRUE

[PcdsFixedAtBuild.AARCH64]
  # ARM General Interrupt Controller
  gArmTokenSpaceGuid.PcdGicDistributorBase|0x8000000
  gArmTokenSpaceGuid.PcdGicRedistributorsBase|0x80a0000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase|0x8080000

  # AARCH64 use PL011 Serial Port device instead of UniversalPayload Serial
  gUefiPayloadPkgTokenSpaceGuid.PcdUseUniversalPayloadSerialPort|FALSE

  # ARM on QEMU platform
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableSize   | 0x40000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareSize   | 0x40000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize | 0x40000

[PcdsPatchableInModule]
!if $(NETWORK_DRIVER_ENABLE) == TRUE
  gEfiNetworkPkgTokenSpaceGuid.PcdAllowHttpConnections|TRUE
!endif
  gUefiPayloadPkgTokenSpaceGuid.SizeOfIoSpace|16
  gUefiPayloadPkgTokenSpaceGuid.PcdFDTPageSize|8

  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x7
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x8000004F
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxSizeNonPopulateCapsule|$(MAX_SIZE_NON_POPULATE_CAPSULE)

  #
  # Enable these parameters to be set on the command line
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialClockRate|$(SERIAL_CLOCK_RATE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialLineControl|$(SERIAL_LINE_CONTROL)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseHardwareFlowControl|$(SERIAL_HARDWARE_FLOW_CONTROL)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialDetectCable|$(SERIAL_DETECT_CABLE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialFifoControl|$(SERIAL_FIFO_CONTROL)

  gEfiMdeModulePkgTokenSpaceGuid.PcdPciSerialParameters|$(PCI_SERIAL_PARAMETERS)

  gUefiCpuPkgTokenSpaceGuid.PcdCpuMaxLogicalProcessorNumber|$(MAX_LOGICAL_PROCESSORS)
  gUefiCpuPkgTokenSpaceGuid.PcdCpuNumberOfReservedVariableMtrrs|0
  gUefiPayloadPkgTokenSpaceGuid.PcdBootloaderParameter|0

[PcdsPatchableInModule.IA32, PcdsPatchableInModule.X64]
  #
  # The following parameters are set by Library/PlatformHookLib
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseMmio|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x3F8
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialBaudRate|$(BAUD_RATE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterStride|1

[PcdsPatchableInModule.AARCH64]
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseMmio|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x9000000
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialBaudRate|$(BAUD_RATE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterStride|1

################################################################################
#
# Pcd DynamicEx Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsDynamicHii]
  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut|L"Timeout"|gEfiGlobalVariableGuid|0x0|$(PLATFORM_BOOT_TIMEOUT)

[PcdsDynamicExDefault]
  gEfiMdePkgTokenSpaceGuid.PcdDefaultTerminalType|$(DEFAULT_TERMINAL_TYPE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdAriSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdMrIovSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSrIovSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdPcieResizableBarSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSrIovSystemPageSize|0x1
  gUefiCpuPkgTokenSpaceGuid.PcdCpuApInitTimeOutInMicroSeconds|50000
  gUefiCpuPkgTokenSpaceGuid.PcdCpuApLoopMode|1
  gUefiCpuPkgTokenSpaceGuid.PcdCpuMicrocodePatchAddress|0x0
  gUefiCpuPkgTokenSpaceGuid.PcdCpuMicrocodePatchRegionSize|0x0
  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdEmuVariableNvStoreReserved|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase64|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase|0
!if $(VARIABLE_SUPPORT) == "SPI" || $(VARIABLE_SUPPORT) == "SMMSTORE"
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableSize  |0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareSize  |0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase64|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase64|0
!endif
  # Disable SMM S3 script
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiS3Enable|FALSE

  ## This PCD defines the video horizontal resolution.
  #  This PCD could be set to 0 then video resolution could be at highest resolution.
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|0
  ## This PCD defines the video vertical resolution.
  #  This PCD could be set to 0 then video resolution could be at highest resolution.
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|0

  ## The PCD is used to specify the video horizontal resolution of text setup.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoHorizontalResolution|0
  ## The PCD is used to specify the video vertical resolution of text setup.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoVerticalResolution|0

  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow|31
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn|100
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseSize|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdGhcbBase|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdTestKeyUsed|FALSE
  gUefiCpuPkgTokenSpaceGuid.PcdSevEsIsEnabled|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdPciDisableBusEnumeration|TRUE

[PcdsDynamicExDefault.IA32, PcdsDynamicExDefault.X64]
  gPcAtChipsetPkgTokenSpaceGuid.PcdRtcIndexRegister|$(RTC_INDEX_REGISTER)
  gPcAtChipsetPkgTokenSpaceGuid.PcdRtcTargetRegister|$(RTC_TARGET_REGISTER)

  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|$(UART_DEFAULT_BAUD_RATE)
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultDataBits|$(UART_DEFAULT_DATA_BITS)
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultParity|$(UART_DEFAULT_PARITY)
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultStopBits|$(UART_DEFAULT_STOP_BITS)

!if $(TPM_ENABLE) == TRUE
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2InitializationPolicy|0
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInitializationPolicy|0
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2SelfTestPolicy|0
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2ScrtmPolicy|0
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmScrtmPolicy|0
!endif

  ## Patched by BlSupportDxe
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInstanceGuid|{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

  ## Match the hash algorithms listed in Tcg2Dxe
  gEfiSecurityPkgTokenSpaceGuid.PcdTcg2HashAlgorithmBitmap|0x1F

[PcdsDynamicExDefault.AARCH64]
  # ARM on QEMU platform
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase64   | 0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase64 | 0

  # Timer IRQs
  gArmTokenSpaceGuid.PcdArmArchTimerSecIntrNum|29
  gArmTokenSpaceGuid.PcdArmArchTimerIntrNum|30
  # Not used in QEMU platform
  gArmTokenSpaceGuid.PcdArmArchTimerVirtIntrNum|0
  gArmTokenSpaceGuid.PcdArmArchTimerHypIntrNum|26
  gArmTokenSpaceGuid.PcdArmArchTimerHypVirtIntrNum|0x0

  # PL031 RealTimeClock
  gArmPlatformTokenSpaceGuid.PcdPL031RtcBase|0x9010000

  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0x4010000000
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseSize|0xfffffff
  gEfiMdePkgTokenSpaceGuid.PcdPciIoTranslation|0x0

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform.
#
################################################################################

!if "IA32" in "$(ARCH)"
  [Components.IA32]
  !if $(UNIVERSAL_PAYLOAD) == TRUE
    !if $(UNIVERSAL_PAYLOAD_FORMAT) == "ELF"
      UefiPayloadPkg/UefiPayloadEntry/UniversalPayloadEntry.inf
    !elseif $(UNIVERSAL_PAYLOAD_FORMAT) == "FIT"
      UefiPayloadPkg/UefiPayloadEntry/FitUniversalPayloadEntry.inf {
        <LibraryClasses>
          !if gUefiPayloadPkgTokenSpaceGuid.PcdHandOffFdtEnable == TRUE
            CustomFdtNodeParserLib|UefiPayloadPkg/Library/CustomFdtNodeParserLib/CustomFdtNodeParserLib.inf
            NULL|UefiPayloadPkg/Library/FdtParserLib/FdtParseLib.inf
          !endif
          NULL|UefiPayloadPkg/Library/HobParseLib/HobParseLib.inf
      }
    !else
      UefiPayloadPkg/UefiPayloadEntry/UefiPayloadEntry.inf
    !endif
  !else
    UefiPayloadPkg/UefiPayloadEntry/UefiPayloadEntry.inf
  !endif
!else
  [Components.X64, Components.AARCH64]
  !if $(UNIVERSAL_PAYLOAD) == TRUE
    !if $(UNIVERSAL_PAYLOAD_FORMAT) == "ELF"
      UefiPayloadPkg/UefiPayloadEntry/UniversalPayloadEntry.inf
    !elseif $(UNIVERSAL_PAYLOAD_FORMAT) == "FIT"
      UefiPayloadPkg/UefiPayloadEntry/FitUniversalPayloadEntry.inf {
        <LibraryClasses>
          !if gUefiPayloadPkgTokenSpaceGuid.PcdHandOffFdtEnable == TRUE
            CustomFdtNodeParserLib|UefiPayloadPkg/Library/CustomFdtNodeParserLib/CustomFdtNodeParserLib.inf
            NULL|UefiPayloadPkg/Library/FdtParserLib/FdtParseLib.inf
          !endif
          NULL|UefiPayloadPkg/Library/HobParseLib/HobParseLib.inf
      }
    !else
      UefiPayloadPkg/UefiPayloadEntry/UefiPayloadEntry.inf
    !endif
  !else
    UefiPayloadPkg/UefiPayloadEntry/UefiPayloadEntry.inf
  !endif
!endif

  #
  # UEFI network modules
  #
!if $(NETWORK_DRIVER_ENABLE) == TRUE
[Defines]
  DEFINE PLATFORMX64_ENABLE = TRUE
  !include NetworkPkg/Network.dsc.inc
!endif

[Components.X64, Components.AARCH64]
  #
  # DXE Core
  #
  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <LibraryClasses>
      !if $(MULTIPLE_DEBUG_PORT_SUPPORT) == TRUE
        DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
        SerialPortLib|UefiPayloadPkg/Library/BaseSerialPortLibHob/DxeBaseSerialPortLibHob.inf
      !endif
      NULL|MdeModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  }

  #
  # Components that produce the architectural protocols
  #
!if $(SECURITY_STUB_ENABLE) == TRUE
  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf {
      <LibraryClasses>
!if $(SECURE_BOOT_ENABLE) == TRUE
      NULL|SecurityPkg/Library/DxeImageVerificationLib/DxeImageVerificationLib.inf
!endif
!if $(TPM_ENABLE) == TRUE
      NULL|SecurityPkg/Library/DxeTpmMeasureBootLib/DxeTpmMeasureBootLib.inf
      NULL|SecurityPkg/Library/DxeTpm2MeasureBootLib/DxeTpm2MeasureBootLib.inf
!endif
  }
!endif

!if $(SECURE_BOOT_ENABLE) == TRUE
  SecurityPkg/VariableAuthenticated/SecureBootConfigDxe/SecureBootConfigDxe.inf
!endif

!if $(TPM_ENABLE) == TRUE
  SecurityPkg/Tcg/Tcg2Dxe/Tcg2Dxe.inf {
    <LibraryClasses>
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterDxe.inf
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
      HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterDxe.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha384/HashInstanceLibSha384.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha512/HashInstanceLibSha512.inf
      NULL|SecurityPkg/Library/HashInstanceLibSm3/HashInstanceLibSm3.inf
  }
  SecurityPkg/Tcg/Tcg2Config/Tcg2ConfigDxe.inf
  SecurityPkg/Tcg/TcgDxe/TcgDxe.inf {
    <LibraryClasses>
      Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibDTpm/Tpm12DeviceLibDTpm.inf
  }
  SecurityPkg/Tcg/TcgConfigDxe/TcgConfigDxe.inf
!endif

  MdeModulePkg/Universal/BdsDxe/BdsDxe.inf {
    <LibraryClasses>
!if $(CSM_ENABLE) == TRUE
      NULL|UefiPayloadPkg/Csm/CsmSupportLib/CsmSupportLib.inf
      NULL|OvmfPkg/Csm/LegacyBootManagerLib/LegacyBootManagerLib.inf
!endif
  }
!if $(BOOTSPLASH_IMAGE)
  MdeModulePkg/Logo/LogoDxe.inf
!endif
  MdeModulePkg/Application/UiApp/UiApp.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/DeviceManagerUiLib/DeviceManagerUiLib.inf
      NULL|MdeModulePkg/Library/BootManagerUiLib/BootManagerUiLib.inf
      NULL|MdeModulePkg/Library/BootMaintenanceManagerUiLib/BootMaintenanceManagerUiLib.inf
!if $(CSM_ENABLE) == TRUE
      NULL|OvmfPkg/Csm/LegacyBootManagerLib/LegacyBootManagerLib.inf
      NULL|OvmfPkg/Csm/LegacyBootMaintUiLib/LegacyBootMaintUiLib.inf
!endif
  }
  MdeModulePkg/Application/BootManagerMenuApp/BootManagerMenuApp.inf
!if $(CAPSULE_SUPPORT) == TRUE
  # Build FmpDxe meant for the inclusion into an update capsule as an embedded driver.
  FmpDevicePkg/FmpDxe/FmpDxe.inf {
    <Defines>
      # FmpDxe interprets its FILE_GUID as firmware GUID.  This allows including
      # multiple FmpDxe instances along each other targeting different
      # components.
      FILE_GUID = $(CAPSULE_MAIN_FW_GUID)
    <PcdsFixedAtBuild>
      gFmpDevicePkgTokenSpaceGuid.PcdFmpDeviceImageIdName|L"System Firmware"
      # Public certificate used for validation of UEFI capsules
      #
      # See BaseTools/Source/Python/Pkcs7Sign/Readme.md for more details on such
      # PCDs and include files.
      !include BaseTools/Source/Python/Pkcs7Sign/TestRoot.cer.gFmpDevicePkgTokenSpaceGuid.PcdFmpDevicePkcs7CertBufferXdr.inc
    <LibraryClasses>
!if $(BOOTLOADER) == "COREBOOT"
      FmpDeviceLib|UefiPayloadPkg/Library/FmpDeviceSmmLib/FmpDeviceSmmLib.inf
!else
      # TODO: provide platform-specific implementation of firmware flashing
      FmpDeviceLib|FmpDevicePkg/Library/FmpDeviceLibNull/FmpDeviceLibNull.inf
!endif
  }
  MdeModulePkg/Universal/EsrtDxe/EsrtDxe.inf
!endif

!if $(CFR_SETUP_MENU_ENABLE) == TRUE
  UefiPayloadPkg/CfrSetupMenuDxe/CfrSetupMenuDxe.inf
!endif

  MdeModulePkg/Universal/Metronome/Metronome.inf
  MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
!if $(DISABLE_RESET_SYSTEM) == FALSE
  MdeModulePkg/Universal/ResetSystemRuntimeDxe/ResetSystemRuntimeDxe.inf
!endif
  PcAtChipsetPkg/PcatRealTimeClockRuntimeDxe/PcatRealTimeClockRuntimeDxe.inf

  #
  # Following are the DXE drivers
  #
  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }

  MdeModulePkg/Universal/ReportStatusCodeRouter/RuntimeDxe/ReportStatusCodeRouterRuntimeDxe.inf
  MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf
  UefiCpuPkg/CpuIo2Dxe/CpuIo2Dxe.inf
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
!if $(MEMORY_TEST) == "GENERIC"
  MdeModulePkg/Universal/MemoryTest/GenericMemoryTestDxe/GenericMemoryTestDxe.inf
!elseif $(MEMORY_TEST) == "NULL"
  MdeModulePkg/Universal/MemoryTest/NullMemoryTestDxe/NullMemoryTestDxe.inf
!endif
  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf
  MdeModulePkg/Universal/PlatformDriOverrideDxe/PlatformDriOverrideDxe.inf
  MdeModulePkg/Universal/EbcDxe/EbcDxe.inf

  UefiPayloadPkg/BlSupportDxe/BlSupportDxe.inf

  #
  # SMBIOS Support
  #
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf

  #
  # ACPI Support
  #
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf
!if $(BOOTSPLASH_IMAGE)
  MdeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
  MdeModulePkg/Universal/Acpi/BootGraphicsResourceTableDxe/BootGraphicsResourceTableDxe.inf
!endif

  #
  # PCI Support
  #
  MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
  MdeModulePkg/Bus/Pci/PciHostBridgeDxe/PciHostBridgeDxe.inf {
    <LibraryClasses>
      PciHostBridgeLib|UefiPayloadPkg/Library/PciHostBridgeLib/PciHostBridgeLib.inf
  }

  #
  # SCSI/ATA/IDE/DISK Support
  #
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  FatPkg/EnhancedFatDxe/Fat.inf
  MdeModulePkg/Universal/Disk/UdfDxe/UdfDxe.inf
!if $(ATA_ENABLE) == TRUE
  MdeModulePkg/Bus/Pci/SataControllerDxe/SataControllerDxe.inf
  MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf
!endif
  MdeModulePkg/Bus/Ata/AtaAtapiPassThru/AtaAtapiPassThru.inf
  MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf
!if $(NVME_ENABLE) == TRUE
  MdeModulePkg/Bus/Pci/NvmExpressDxe/NvmExpressDxe.inf
!endif

!if $(RAM_DISK_ENABLE) == TRUE
  MdeModulePkg/Universal/Disk/RamDiskDxe/RamDiskDxe.inf
!endif
  #
  # SD/eMMC Support
  #
!if $(SD_ENABLE) == TRUE
  MdeModulePkg/Bus/Pci/SdMmcPciHcDxe/SdMmcPciHcDxe.inf
  MdeModulePkg/Bus/Sd/EmmcDxe/EmmcDxe.inf
  MdeModulePkg/Bus/Sd/SdDxe/SdDxe.inf
!endif

  #
  # Support for loading Option ROMs from PCI-Express devices
  #
!if $(LOAD_OPTION_ROMS) == TRUE
  UefiPayloadPkg/PciPlatformDxe/PciPlatformDxe.inf
!endif

  #
  # Usb Support
  #
  MdeModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
  MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
  MdeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf

!if $(CSM_ENABLE) == TRUE
  OvmfPkg/8259InterruptControllerDxe/8259.inf
  # Use HPET, PIT set to CSM compatible
  #OvmfPkg/8254TimerDxe/8254Timer.inf
  OvmfPkg/Csm/BiosThunk/VideoDxe/VideoDxe.inf
  OvmfPkg/Csm/LegacyBiosDxe/LegacyBiosDxe.inf
  OvmfPkg/Csm/Csm16/Csm16.inf
!endif

  #
  # ISA Support
  #
!if $(SERIAL_DRIVER_ENABLE) == TRUE
  MdeModulePkg/Universal/SerialDxe/SerialDxe.inf
!endif
!if $(SIO_BUS_ENABLE) == TRUE
  OvmfPkg/SioBusDxe/SioBusDxe.inf
!endif
!if $(PS2_KEYBOARD_ENABLE) == TRUE
  MdeModulePkg/Bus/Isa/Ps2KeyboardDxe/Ps2KeyboardDxe.inf
!endif
!if $(PS2_MOUSE_ENABLE) == TRUE
  MdeModulePkg/Bus/Isa/Ps2MouseDxe/Ps2MouseDxe.inf
!endif

  #
  # Console Support
  #
  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
!if $(DISABLE_SERIAL_TERMINAL) == FALSE
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
!endif

!if $(USE_PLATFORM_GOP) == TRUE
  UefiPayloadPkg/PlatformGopPolicy/PlatformGopPolicy.inf
!else
  UefiPayloadPkg/GraphicsOutputDxe/GraphicsOutputDxe.inf
!endif

!if $(PERFORMANCE_MEASUREMENT_ENABLE)
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTableDxe/FirmwarePerformanceDxe.inf
!endif

  #
  # SMM Support
  #
!if $(SMM_SUPPORT) == TRUE
  UefiPayloadPkg/SmmAccessDxe/SmmAccessDxe.inf
  UefiPayloadPkg/SmmControlRuntimeDxe/SmmControlRuntimeDxe.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmIpl.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmCore.inf
  MdeModulePkg/Universal/ReportStatusCodeRouter/Smm/ReportStatusCodeRouterSmm.inf
  MdeModulePkg/Universal/StatusCodeHandler/Smm/StatusCodeHandlerSmm.inf
  UefiCpuPkg/PiSmmCpuDxeSmm/PiSmmCpuDxeSmm.inf
  UefiPayloadPkg/PchSmiDispatchSmm/PchSmiDispatchSmm.inf
  UefiPayloadPkg/BlSupportSmm/BlSupportSmm.inf
  UefiCpuPkg/CpuIo2Smm/CpuIo2Smm.inf
!if $(PERFORMANCE_MEASUREMENT_ENABLE) == TRUE
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTableSmm/FirmwarePerformanceSmm.inf
!endif
!endif

  #
  # Payload MM support
  #
!if $(PAYLOAD_MM_SUPPORT) == TRUE
  UefiPayloadPkg/SmmAccessDxe/SmmAccessDxe.inf
  UefiPayloadPkg/SmmControlRuntimeDxe/SmmControlRuntimeDxe.inf
  StandaloneMmPkg/Drivers/MmCommunicationDxe/MmCommunicationDxe.inf
  StandaloneMmPkg/Drivers/MmCommunicationNotifyDxe/MmCommunicationNotifyDxe.inf
  StandaloneMmPkg/Drivers/StandaloneMmIplPei/StandaloneMmIplDxe.inf {
    <LibraryClasses>
      MmPlatformHobProducerLib|UefiPayloadPkg/Library/MmPlatformHobProducerLib/MmPlatformHobProducerLib.inf
      MmIplPlatformHookLib|UefiPayloadPkg/Library/MmIplPlatformHookLibPayloadMm/PayloadMmInitLib.inf
!if $(VARIABLE_SUPPORT) == "SPI"
      NULL|StandaloneMmPkg/Library/VariableMmDependency/VariableMmDependency.inf
!endif
  }
  StandaloneMmPkg/Core/StandaloneMmCore.inf {
    <LibraryClasses>
      StandaloneMmCoreEntryPoint|UefiPayloadPkg/Library/MmCorePayloadMmEntryPoint/StandaloneMmCoreEntryPoint.inf
  }
  MdeModulePkg/Universal/ReportStatusCodeRouter/Smm/ReportStatusCodeRouterStandaloneMm.inf
  MdeModulePkg/Universal/StatusCodeHandler/Smm/StatusCodeHandlerStandaloneMm.inf
  UefiPayloadPkg/BlSmmCpuPayloadMm/BlSmmCpuStandaloneMm.inf
  #UefiPayloadPkg/PchSmiDispatchSmm/PchSmiDispatchSmm.inf
  UefiCpuPkg/CpuIo2Smm/CpuIo2StandaloneMm.inf
!if $(PERFORMANCE_MEASUREMENT_ENABLE) == TRUE
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTableSmm/FirmwarePerformanceStandaloneMm.inf
!endif
!endif

!if $(VARIABLE_SUPPORT) == "EMU"
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf
!elseif $(VARIABLE_SUPPORT) == "SMMSTORE"
  UefiPayloadPkg/SmmStoreFvb/SmmStoreFvbRuntimeDxe.inf
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
      NULL|EmbeddedPkg/Library/NvVarStoreFormattedLib/NvVarStoreFormattedLib.inf
  }
!elseif $(VARIABLE_SUPPORT) == "SPI"
!if $(SMM_SUPPORT) == TRUE
  UefiPayloadPkg/FvbRuntimeDxe/FvbSmm.inf
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteSmm.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmm.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
      NULL|MdeModulePkg/Library/VarCheckHiiLib/VarCheckHiiLib.inf
      NULL|MdeModulePkg/Library/VarCheckPcdLib/VarCheckPcdLib.inf
      NULL|MdeModulePkg/Library/VarCheckPolicyLib/VarCheckPolicyLib.inf
  }
!endif
!if $(PAYLOAD_MM_SUPPORT) == TRUE
  UefiPayloadPkg/FvbRuntimeDxe/FvbStandaloneMm.inf
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteStandaloneMm.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableStandaloneMm.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
      NULL|MdeModulePkg/Library/VarCheckPolicyLib/VarCheckPolicyLibStandaloneMm.inf
  }
!endif
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmmRuntimeDxe.inf
!endif

  #
  # Misc
  #
!if $(CRYPTO_PROTOCOL_SUPPORT) == TRUE
!if $(CRYPTO_DRIVER_EXTERNAL_SUPPORT) == FALSE
  CryptoPkg/Driver/CryptoDxe.inf {
    <LibraryClasses>
      BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
      TlsLib|CryptoPkg/Library/TlsLib/TlsLib.inf
  }
!endif
!endif
!if $(CPU_RNG_ENABLE) == TRUE
  SecurityPkg/RandomNumberGenerator/RngDxe/RngDxe.inf
!endif

[Components.X64]
  UefiCpuPkg/CpuDxe/CpuDxe.inf

!if $(TIMER_SUPPORT) == "HPET"
  PcAtChipsetPkg/HpetTimerDxe/HpetTimerDxe.inf
!elseif $(TIMER_SUPPORT) == "LAPIC"
  OvmfPkg/LocalApicTimerDxe/LocalApicTimerDxe.inf {
    <LibraryClasses>
      NestedInterruptTplLib|OvmfPkg/Library/NestedInterruptTplLib/NestedInterruptTplLib.inf
  }
!else
  !error "Invalid TIMER_SUPPORT"
!endif

[Components.AARCH64]
  ArmPkg/Drivers/ArmPciCpuIo2Dxe/ArmPciCpuIo2Dxe.inf
  ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  UefiCpuPkg/Library/ArmMmuLib/ArmMmuBaseLib.inf

  EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf

  ArmPkg/Drivers/ArmGicDxe/ArmGicDxe.inf
  ArmPkg/Drivers/TimerDxe/TimerDxe.inf
  OvmfPkg/VirtNorFlashDxe/VirtNorFlashDxe.inf {
    <LibraryClasses>
      # don't use unaligned CopyMem () on the UEFI varstore NOR flash region
      BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  }

  SecurityPkg/RandomNumberGenerator/RngDxe/RngDxe.inf

  OvmfPkg/PlatformDxe/Platform.inf
  OvmfPkg/Fdt/VirtioFdtDxe/VirtioFdtDxe.inf
  OvmfPkg/Fdt/HighMemDxe/HighMemDxe.inf
  OvmfPkg/VirtioBlkDxe/VirtioBlk.inf
  OvmfPkg/VirtioScsiDxe/VirtioScsi.inf
  OvmfPkg/VirtioNetDxe/VirtioNet.inf
  OvmfPkg/VirtioRngDxe/VirtioRng.inf
  OvmfPkg/VirtioSerialDxe/VirtioSerial.inf

  MdeModulePkg/Universal/Disk/UdfDxe/UdfDxe.inf
  OvmfPkg/VirtioFsDxe/VirtioFsDxe.inf

  # SMBIOS Support
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf

  # PCI support
  UefiCpuPkg/CpuMmio2Dxe/CpuMmio2Dxe.inf {
    <LibraryClasses>
      NULL|OvmfPkg/Fdt/FdtPciPcdProducerLib/FdtPciPcdProducerLib.inf
  }
  OvmfPkg/PciHotPlugInitDxe/PciHotPlugInit.inf
  OvmfPkg/VirtioPciDeviceDxe/VirtioPciDeviceDxe.inf
  OvmfPkg/Virtio10Dxe/Virtio10.inf

  # Video support
  OvmfPkg/QemuRamfbDxe/QemuRamfbDxe.inf
  OvmfPkg/VirtioGpuDxe/VirtioGpu.inf

  # Hash2 Protocol Support
  SecurityPkg/Hash2DxeCrypto/Hash2DxeCrypto.inf

  # ACPI Support
  OvmfPkg/PlatformHasAcpiDtDxe/PlatformHasAcpiDtDxe.inf

  #------------------------------
  #  Build the shell
  #------------------------------

!if $(SHELL_TYPE) == BUILD_SHELL

  #
  # Shell Lib
  #
[LibraryClasses]
  BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  !include NetworkPkg/NetworkLibs.dsc.inc

[Components.X64, Components.AARCH64]
  ShellPkg/DynamicCommand/TftpDynamicCommand/TftpDynamicCommand.inf {
    <PcdsFixedAtBuild>
      ## This flag is used to control initialization of the shell library
      #  This should be FALSE for compiling the dynamic command.
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
  }
  ShellPkg/DynamicCommand/HttpDynamicCommand/HttpDynamicCommand.inf {
    <PcdsFixedAtBuild>
      ## This flag is used to control initialization of the shell library
      #  This should be FALSE for compiling the dynamic command.
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
  }
!if $(PERFORMANCE_MEASUREMENT_ENABLE) == TRUE
  ShellPkg/DynamicCommand/DpDynamicCommand/DpDynamicCommand.inf {
    <PcdsFixedAtBuild>
      ## This flag is used to control initialization of the shell library
      #  This should be FALSE for compiling the dynamic command.
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
  }
!endif
  ShellPkg/Application/Shell/Shell.inf {
    <PcdsFixedAtBuild>
      ## This flag is used to control initialization of the shell library
      #  This should be FALSE for compiling the shell application itself only.
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE

    #------------------------------
    #  Basic commands
    #------------------------------

    <LibraryClasses>
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellAcpiViewCommandLib/UefiShellAcpiViewCommandLib.inf

    #------------------------------
    #  Networking commands
    #------------------------------

    <LibraryClasses>
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf

    #------------------------------
    #  Support libraries
    #------------------------------

    <LibraryClasses>
      HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
      OrderedCollectionLib|MdePkg/Library/BaseOrderedCollectionRedBlackTreeLib/BaseOrderedCollectionRedBlackTreeLib.inf
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
      ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
      ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
  }

!if $(MEMORY_PROFILE_ENABLE) == TRUE
  MdeModulePkg/Application/MemoryProfileInfo/MemoryProfileInfo.inf
!endif

!endif
