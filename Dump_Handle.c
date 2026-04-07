/** @file
  UEFI Shell application to dump Handles, Device Paths, MAC Addresses,
  and detailed Device Path Node Types/Sub-Types.

  Copyright (c) 2024, Firmware Engineering Training.
  All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ShellCEntryLib.h>
#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>

/**
  Convert the Device Path Type value to a human-readable string.

  @param[in] Type  The Device Path node Type.

  @retval A constant Unicode string representing the type category.
**/
STATIC
CONST CHAR16 *
GetDevicePathTypeString (
  IN UINT8 Type
  )
{
  switch (Type) {
    case HARDWARE_DEVICE_PATH:
      return L"Hardware";
    case ACPI_DEVICE_PATH:
      return L"ACPI";
    case MESSAGING_DEVICE_PATH:
      return L"Messaging";
    case MEDIA_DEVICE_PATH:
      return L"Media";
    case BBS_DEVICE_PATH:
      return L"BBS";
    case END_DEVICE_PATH_TYPE:
      return L"End";
    default:
      return L"Unknown";
  }
}

/**
  Enumerate and print all handles that support EFI_DEVICE_PATH_PROTOCOL,
  including a detailed breakdown of each Device Path Node's Type and Sub-Type.

  This function locates all EFI handles that support EFI_DEVICE_PATH_PROTOCOL,
  retrieves the protocol instance from each handle, converts the binary device
  path into a human-readable text string, and then walks through each node
  to dump its Type and Sub-Type for deep hardware topology debugging.

  @retval None
**/
STATIC
VOID
DumpAllDevicePaths (
  VOID
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                *HandleBuffer;
  UINTN                     HandleCount;
  UINTN                     Index;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *Node;
  CHAR16                    *DevicePathText;
  UINTN                     NodeIndex;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol, 
                  &gEfiDevicePathProtocolGuid, 
                  NULL, 
                  &HandleCount, 
                  &HandleBuffer);

  if (EFI_ERROR (Status)) {
    Print(L"Error: Failed to locate any Device Path handles. (%r)\n", Status);
    return;
  }

  Print(L"Total Device Paths found: %d\n", HandleCount);
  Print(L"-------------------------------------------------------------------------\n");

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index], 
                    &gEfiDevicePathProtocolGuid, 
                    (VOID **)&DevicePath);

    if (!EFI_ERROR (Status)) {
      // 1. Print the human-readable string representation of the full path
      DevicePathText = ConvertDevicePathToText (DevicePath, FALSE, FALSE);
      if (DevicePathText != NULL) {
        Print (L"DevicePath(%08X) : %s\n", (UINTN)DevicePath, DevicePathText);
        FreePool (DevicePathText);
      }

      // 2. Walk through each node and dump Type/Sub-Type details
      Node = DevicePath;
      NodeIndex = 1;

      while (TRUE) {
        Print(L"  |-- Node[%d] Type: 0x%02X (%s), Sub-Type: 0x%02X\n", 
          NodeIndex, 
          DevicePathType(Node), 
          GetDevicePathTypeString(DevicePathType(Node)), 
          DevicePathSubType(Node));

        // Break the loop if we reach the end of the entire Device Path structure
        if (IsDevicePathEnd (Node)) {
          break;
        }

        // Advance pointer to the next Device Path node
        Node = NextDevicePathNode (Node);
        NodeIndex++;
      }
      Print(L"\n"); // Add an empty line between different handles for readability
    }
  }
  
  FreePool (HandleBuffer);
}

/**
  Enumerate and print MAC addresses of all Ethernet devices using ONLY Device Path.

  This function locates all EFI handles that support EFI_DEVICE_PATH_PROTOCOL,
  walks through the nodes of each device path, and extracts the hardware
  MAC address directly from the Messaging Device Path node.

  @retval None
**/
STATIC
VOID
DumpEthernetMacsFromDevicePath (
  VOID
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                *HandleBuffer;
  UINTN                     HandleCount;
  UINTN                     Index;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *Node;
  MAC_ADDR_DEVICE_PATH      *MacNode;
  CHAR16                    *DevicePathText;
  BOOLEAN                   FoundMac;
  UINTN                     MacCount;
  UINTN                     NodeIndex; // Added: Used to track the node sequence

  Status = gBS->LocateHandleBuffer (
                  ByProtocol, 
                  &gEfiDevicePathProtocolGuid, 
                  NULL, 
                  &HandleCount, 
                  &HandleBuffer);

  if (EFI_ERROR (Status)) {
    Print(L"Error: Failed to locate any Device Path handles. (%r)\n", Status);
    return;
  }

  Print(L"Scanning %d Device Paths for Ethernet MAC Addresses...\n", HandleCount);
  Print(L"-------------------------------------------------------------------------\n");

  MacCount = 0;

  // Iterate through all handles that support Device Path Protocol
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index], 
                    &gEfiDevicePathProtocolGuid, 
                    (VOID **)&DevicePath);

    if (EFI_ERROR(Status)) continue;

    Node     = DevicePath;
    FoundMac = FALSE;

    // Begin traversing the Device Path nodes for this specific handle
    while (!IsDevicePathEnd (Node)) {
      
      // Search for a Messaging Device Path node with a MAC Address SubType
      if ((DevicePathType (Node) == MESSAGING_DEVICE_PATH) &&
          (DevicePathSubType (Node) == MSG_MAC_ADDR_DP)) {
        
        MacNode = (MAC_ADDR_DEVICE_PATH *)Node;

        // Verify if the interface type corresponds to Ethernet (IfType == 0x01, 0x00, or 0x06)
        if (MacNode->IfType == 0x01 || MacNode->IfType == 0x00 || MacNode->IfType == 0x06) {
          FoundMac = TRUE;
          MacCount++;
          
          Print(L"[%d] MAC Address : %02X:%02X:%02X:%02X:%02X:%02X\n",
            MacCount,
            MacNode->MacAddress.Addr[0],
            MacNode->MacAddress.Addr[1],
            MacNode->MacAddress.Addr[2],
            MacNode->MacAddress.Addr[3],
            MacNode->MacAddress.Addr[4],
            MacNode->MacAddress.Addr[5]
            );
            
          // MAC node successfully located, break early from the while loop 
          break; 
        }
      }
      
      // Advance to the next Device Path node
      Node = NextDevicePathNode (Node);
    }

    // If a MAC address was found, print the full Device Path text and its Node hierarchy
    if (FoundMac) {
      DevicePathText = ConvertDevicePathToText (DevicePath, FALSE, FALSE);
      if (DevicePathText != NULL) {
        Print (L"DevicePath(%08X) : %s\n", (UINTN)DevicePath, DevicePathText);
        FreePool(DevicePathText);
        
        // --- Detailed Node Parsing Logic Starts Here ---
        
        // Critical: Reset the Node pointer back to the beginning of the Device Path structure
        Node = DevicePath; 
        NodeIndex = 1;

        while (TRUE) {
          Print(L"  |-- Node[%d] Type: 0x%02X (%s), Sub-Type: 0x%02X\n", 
            NodeIndex, 
            DevicePathType(Node), 
            GetDevicePathTypeString(DevicePathType(Node)), 
            DevicePathSubType(Node));

          // Check if this is the End of Device Path node
          if (IsDevicePathEnd (Node)) {
            break;
          }

          // Advance pointer to the next Device Path node
          Node = NextDevicePathNode (Node);
          NodeIndex++;
        }
        
        // --- Detailed Node Parsing Logic Ends Here ---
        
        // Add an empty line between different handles for readability
        Print (L"\n"); 
      }
    }
  }
  
  if (MacCount == 0) {
    Print(L"No Ethernet MAC addresses found in the current Device Paths.\n");
    Print(L"(Hint: Are the network drivers loaded/connected?)\n");
  }
  FreePool (HandleBuffer);
}

/**
  Print command usage information.

  This function displays help messages describing available command-line
  options and usage examples for the Handle Dump application.

  @retval None
**/
STATIC
VOID
PrintUsage (
  VOID
  )
{
  Print (L"Usage:\n");
  Print (L"  DumpHandle.efi -dp    : Dump all Device Paths, Node Types, and Sub-Types.\n");
  Print (L"  DumpHandle.efi -net   : Dump Ethernet devices and MAC addresses (via DevicePath).\n");
  Print (L"  DumpHandle.efi -help  : Show this help message.\n\n");
}

/**
  Entry point for the UEFI Shell application.

  This function parses command-line arguments, checks for specific user 
  flags, and performs requested operations such as dumping all device paths 
  or extracting Ethernet MAC addresses via pure Device Path traversal.

  @param[in]  Argc   Argument count.
  @param[in]  Argv   Argument vector.

  @retval 0   The application executed successfully.
  @retval 1   Invalid arguments were provided or an error occurred.
**/
INTN
EFIAPI
ShellAppMain (
  IN UINTN   Argc,
  IN CHAR16  **Argv
  )
{
  Print(L"=========================================================================\n");
  Print(L"                   UEFI Handle & Device Path Dump Tool                   \n");
  Print(L"=========================================================================\n");

  if (Argc == 1 || StrCmp (Argv[1], L"-help") == 0 || StrCmp (Argv[1], L"-h") == 0) {
    PrintUsage ();
    return 0;
  }

  if (StrCmp (Argv[1], L"-dp") == 0) {
    DumpAllDevicePaths ();
    return 0;
  }

  if (StrCmp (Argv[1], L"-net") == 0 || StrCmp (Argv[1], L"-mac") == 0) {
    DumpEthernetMacsFromDevicePath ();
    return 0;
  }

  Print(L"Invalid argument: %s\n", Argv[1]);
  PrintUsage ();
  return 1;
}