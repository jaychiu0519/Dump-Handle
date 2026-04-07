##  @file
#  EFI application that displays and verifies ACPI tables
#
#  Copyright (c) 2020, ARM Limited. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
#
##

[Defines]
  INF_VERSION                    = 0x0001001B
  BASE_NAME                      = Dump_Handle
  FILE_GUID                      = 522453bd-a4fa-417c-8c85-78f1f80b1c1f
  MODULE_TYPE                    = UEFI_APPLICATION
  VERSION_STRING                 = 1.0
  ENTRY_POINT                    = ShellCEntryLib
  UEFI_HII_RESOURCE_SECTION      = TRUE

[Sources.common]
  Dump_Handle.c

[Packages]
  MdePkg/MdePkg.dec
  ShellPkg/ShellPkg.dec

[LibraryClasses]
  UefiLib
  ShellCEntryLib
  BaseLib
  BaseMemoryLib
  MemoryAllocationLib
  UefiBootServicesTableLib
  DevicePathLib

[Protocols]
  gEfiDevicePathProtocolGuid
  gEfiSimpleNetworkProtocolGuid
