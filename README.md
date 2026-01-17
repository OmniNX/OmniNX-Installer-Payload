# OmniNX Installer Payload

A minimal payload for installing OmniNX CFW Pack files on Nintendo Switch outside of Horizon OS.

Based on [TegraExplorer](https://github.com/shchmue/TegraExplorer) and [hekate](https://github.com/CTCaer/hekate) by CTCaer, naehrwert, and shchmue.  
Based on [HATS-Installer-Payload](https://github.com/sthetix/HATS-Installer-Payload) by sthetix.

## Features

- **Automatic Variant Detection**: Detects which OmniNX pack variant is present (Standard/Light/OC)
- **Smart Installation Modes**: 
  - **Update Mode**: Selective deletion when OmniNX is already installed
  - **Clean Install**: Full wipe with backup/restore of user data (DBI, Tinfoil, prod.keys)
- **Version Detection**: Detects installed OmniNX version via marker files (`1.0.0s`, `1.0.0l`, `1.0.0oc`)
- **Progress Display**: Visual status messages during installation
- **Error Handling**: Detailed error reporting on screen
- **Payload Chaining**: Automatically launch hekate after installation

## Documentation

For detailed information about the installation process, see:
- **[INSTALLATION_PROCESS.md](INSTALLATION_PROCESS.md)** - Complete step-by-step breakdown of everything checked and done during installation/update

## Installation Modes

### Update Mode (OmniNX Detected)
- Detected when version marker files (`1.0.0s`, `1.0.0l`, or `1.0.0oc`) are found
- Performs selective deletion of specific directories/files
- Preserves user data, savegames, and installed games
- Updates only necessary CFW components

### Clean Install (No OmniNX Detected)
- Detected when no version marker files are found
- Performs full wipe of `/atmosphere`, `/bootloader`, `/config`, and `/switch`
- **Backs up and restores**:
  - `sd:/switch/DBI` → preserved
  - `sd:/switch/tinfoil` → preserved
  - `sd:/switch/prod.keys` → preserved
- Fresh installation of all CFW components

## Building

### Prerequisites

- **devkitARM** - ARM toolchain for Nintendo Switch development
- **BDK** - Blue Development Kit (included in this repo)

### Build Commands

```bash
make clean
make
```

The built payload will be output to `output/omninx-installer.bin`.

## Usage

### 1. Extract OmniNX Pack to SD Card

Extract the OmniNX pack zip file directly to your SD card. The pack should contain one of:

```
sd:/OmniNX Standard/
├── atmosphere/
├── bootloader/
├── config/
├── switch/
├── TegraExplorer/
├── warmboot_mariko/
├── boot.dat
├── boot.ini
├── exosphere.ini
├── hbmenu.nro
├── loader.bin
├── payload.bin
└── 1.0.0s
```

Or `sd:/OmniNX Light/` or `sd:/OmniNX OC/` for other variants.

**Important**: Extract both the OmniNX pack zip AND this payload to your SD card.

### 2. Launch Payload

Use hekate or another bootloader to launch the payload:

1. Place `omninx-installer.bin` in `sd:/bootloader/payloads/`
2. Launch the payload from hekate's payload menu
3. The payload will automatically:
   - Detect which pack variant is present
   - Detect if OmniNX is already installed
   - Perform appropriate installation (update or clean)
   - Launch hekate after completion

### 3. What Happens

1. Payload mounts the SD card
2. Detects current OmniNX installation (if any)
3. Detects which pack variant is on SD card
4. Determines installation mode (update vs clean)
5. Performs cleanup based on mode:
   - **Update**: Selective deletion of specific paths
   - **Clean**: Full wipe with backup/restore
6. Copies files from pack directory to SD root
7. Creates version marker file
8. Cleans up old version markers
9. Launches hekate (`sd:/bootloader/update.bin`)

## Variant Support

The payload supports three OmniNX variants:

- **Standard** (`1.0.0s`): Full CFW pack
- **Light** (`1.0.0l`): Lightweight CFW pack
- **OC** (`1.0.0oc`): Overclock-enabled CFW pack (includes SaltySD)

The payload automatically detects which variant is present on the SD card and installs accordingly.

## Project Structure

```
OmniNX-Installer-Payload/
├── source/              # Main source code
│   ├── main.c          # Entry point and main flow
│   ├── version.c       # Version/variant detection
│   ├── version.h
│   ├── install.c       # Installation logic
│   ├── install.h
│   ├── backup.c        # Backup/restore operations
│   ├── backup.h
│   ├── deletion_lists.h # Arrays of paths to delete
│   ├── fs.c            # File system operations
│   ├── fs.h
│   ├── gfx.c           # Graphics utilities
│   ├── gfx.h
│   ├── nx_sd.c         # SD card operations
│   ├── nx_sd.h
│   ├── link.ld         # Linker script
│   └── start.S         # Startup assembly
├── bdk/                # Blue Development Kit
├── Makefile            # Build configuration
├── VERSION             # Version file
└── README.md           # This file
```

## License

This project is based on TegraExplorer and hekate. Please refer to those projects for their respective licenses.

## Credits

- **CTCaer** - [hekate](https://github.com/CTCaer/hekate)
- **naehrwert** - Tegra exploration work
- **shchmue** - [TegraExplorer](https://github.com/shchmue/TegraExplorer)
- **sthetix** - [HATS-Installer-Payload](https://github.com/sthetix/HATS-Installer-Payload) (inspiration and base structure)
- **Woody2408** - OmniNX CFW Pack creator
