# Dump Handle

[![UEFI](https://img.shields.io/badge/Platform-UEFI-blue.svg)](https://uefi.org/)
[![Arch](https://img.shields.io/badge/Arch-X64-orange.svg)]()
[![License](https://img.shields.io/badge/License-BSD--2--Clause--Patent-green.svg)](LICENSE)

A professional UEFI Shell application designed to enumerate and inspect system handles, specifically focusing on **Device Path** protocols and **Ethernet MAC address** extraction through direct node traversal.

## 📌 Overview

**Dump Handle** is a diagnostic utility that provides a deep dive into the UEFI hardware topology. It allows developers to:
*   Enumerate all handles supporting the `EFI_DEVICE_PATH_PROTOCOL`.
*   Decode binary Device Path structures into human-readable text.
*   Perform detailed node-by-node parsing, displaying the **Type** and **Sub-Type** for each node in the path.
*   Identify network interfaces and extract MAC addresses directly from the `Messaging Device Path` nodes without relying on higher-level network protocols.

> [!NOTE]
> This is a training project developed for firmware engineering exercises and hardware topology debugging.

---

## 🛠 Prerequisites & Build

### Development Environment
*   **Target Architecture**: X64
*   **Build Environment**: [EDK II (EFI Development Kit II)](https://github.com/tianocore/edk2)
*   **Toolchain**: Compatible with standard EDK II build requirements (e.g., VS2022, GCC, or Xcode).

### Build Instructions
1.  Place the project folder into your EDK II workspace (e.g., `AppPkg/Applications/`).
2.  Add the `.inf` file path to your package's `.dsc` file.
3.  Open your UEFI build environment and run:
    ```bash
    build -a X64 -p AppPkg/AppPkg.dsc -m AppPkg/Applications/Dump_Handle/Dump_Handle.inf
    ```
4.  The executable `Dump_Handle.efi` will be generated in your build output directory.

---

## 🚀 Usage & Commands

Run the application within the **UEFI Shell**. The following parameters are supported:

| Parameter | Description |
| :--- | :--- |
| `-dp` | Dump all Device Paths, including detailed Node Types and Sub-Types. |
| `-net` / `-mac` | Scan and dump Ethernet devices and MAC addresses via Device Path traversal. |
| `-help` / `-h` | Show the help message and usage examples. |

### Examples
*   **To view full device topology:**
    `Dump_Handle.efi -dp`
*   **To find your network adapter's MAC address:**
    `Dump_Handle.efi -net`

---

## 📂 Project Structure

*   `Dump_Handle.c`: Core logic for handle enumeration and device path parsing.
*   `Dump_Handle.inf`: EDK II meta-data file for building the application.

---

## 📜 License

This project is licensed under the **BSD-2-Clause-Patent** - see the source code headers for details.

---
*Copyright (c) 2024, Firmware Engineering Training. All rights reserved.*
