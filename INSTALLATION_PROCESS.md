# OmniNX Installer - Installation Process Documentation

This document provides a detailed, step-by-step breakdown of everything that is checked and done during the OmniNX installation/update process.

## Overview

The OmniNX Installer Payload operates in two modes:
- **Update Mode**: When OmniNX is already installed, performs selective cleanup
- **Clean Install Mode**: When no OmniNX is detected, performs full wipe with backup/restore

---

## Phase 1: Initialization & Detection

### Step 1: Hardware Initialization
**Location**: `main.c:185`
- Initialize hardware via `hw_init()`
- Pivot stack to `IPL_STACK_TOP`
- Initialize heap at `IPL_HEAP_START`

### Step 2: Configuration Setup
**Location**: `main.c:190`
- Set default bootloader configuration:
  - Autoboot: disabled
  - Boot wait: 3 seconds
  - Backlight: 100%
  - SE keygen status: false
  - Detect chip variant (T210B01 check)
  - Check RCM patched status

### Step 3: SD Card Mounting
**Location**: `main.c:193`
- Attempt to mount SD card filesystem
- **Error Handling**: If mount fails → reboot system (can't show error without display)

### Step 4: System Initialization
**Location**: `main.c:198-211`
- Initialize Minerva memory controller (set to 800MHz)
- Initialize display and framebuffer (720x1280)
- Initialize graphics console
- Set display backlight to 100%
- Overclock BPMP (Boot and Power Management Processor)

### Step 5: Installation Detection
**Location**: `main.c:216`, `version.c:127`
- Check for `sd:/config/omninx/manifest.ini` file
- If manifest exists:
  - Parse manifest to extract `current_pack` value
  - Determine variant: `standard`, `light`, or `oc`
  - Read version information
  - Set `is_installed = true`
- **Detection Method**: Reads INI file with `[OmniNX]` section containing `current_pack` key

### Step 6: Pack Variant Detection
**Location**: `main.c:219`, `version.c:146`
- Check for staging directories (in order of priority):
  1. `sd:/OmniNX Standard/`
  2. `sd:/OmniNX Light/`
  3. `sd:/OmniNX OC/`
- **Error Handling**: If none found → display error message and wait for button input

### Step 7: Installation Mode Determination
**Location**: `main.c:287`
- If `is_installed == true` → `INSTALL_MODE_UPDATE`
- If `is_installed == false` → `INSTALL_MODE_CLEAN`

---

## Phase 2: User Confirmation

### Step 8: Information Display
**Location**: `main.c:289-298`
- Display installation mode: "Update" or "Saubere Installation" (Clean Install)
- Display pack variant to install: "Standard", "Light", or "OC"
- Display current installation variant (if any) or "Keine" (None)

### Step 9: User Input Wait
**Location**: `main.c:314-338`
- Wait for user confirmation via:
  - **A button** on right Joy-Con, OR
  - **Power button**
- Blocks until button is pressed (checks every 50ms)

---

## Phase 3: Update Mode Execution

**Trigger**: `INSTALL_MODE_UPDATE` (OmniNX already installed)

### Step 1: Cleanup (Selective Deletion)
**Location**: `install.c:321-380`

#### 10. Clean Atmosphere Subdirectories
**Location**: `install.c:325-327`, `deletion_lists.h:9-34`
Deletes 34 specific subdirectories:
- `atmosphere/config/`
- `atmosphere/crash_reports/`
- `atmosphere/exefs_patches/*` (various patch directories)
- `atmosphere/kip1/`, `atmosphere/kips/`
- `atmosphere/logs/`
- And more...

#### 11. Clean Atmosphere Root Directories (Title IDs)
**Location**: `install.c:330-331`, `deletion_lists.h:38-72`
Deletes 29 title ID directories:
- `atmosphere/0000000000534C56/`
- `atmosphere/0100000000000008/`
- `atmosphere/010000000000000D/`
- And 26 more title ID folders...

#### 12. Clean Atmosphere Contents Directories
**Location**: `install.c:333-334`, `deletion_lists.h:75-110`
Deletes 29 corresponding `atmosphere/contents/` title ID directories

#### 13. Clean Atmosphere Files
**Location**: `install.c:336-337`, `deletion_lists.h:113-122`
Deletes 7 specific files:
- `atmosphere/config/exosphere.ini`
- `atmosphere/config/override_config.ini`
- `atmosphere/config/stratosphere.ini`
- `atmosphere/hbl.nsp`
- `atmosphere/package3`
- `atmosphere/reboot_payload.bin`
- `atmosphere/stratosphere.romfs`

#### 14. Clean Bootloader Directories
**Location**: `install.c:340-343`, `deletion_lists.h:125-134`
Deletes 7 bootloader subdirectories:
- `bootloader/boot/`
- `bootloader/bootlogo/`
- `bootloader/ini2/`
- `bootloader/payloads/`
- `bootloader/reboot/`
- `bootloader/res/`
- `bootloader/sys/`

#### 15. Clean Bootloader Files
**Location**: `install.c:345-346`, `deletion_lists.h:137-146`
Deletes 7 specific files:
- `bootloader/ArgonNX.bin`
- `bootloader/bootlogo.bmp`
- `bootloader/hekate_ipl.ini`
- `bootloader/nyx.ini`
- `bootloader/patches.ini`
- `bootloader/update.bin`
- `bootloader/ini/EmuMMC ohne Mods.ini`

#### 16. Clean Config Directories
**Location**: `install.c:349-352`, `deletion_lists.h:149-160`
Deletes 8 config subdirectories:
- `config/aio-switch-updater/`
- `config/blue_pack_updater/`
- `config/kefir-updater/`
- `config/nx-hbmenu/`
- `config/quickntp/`
- `config/sys-con/`
- `config/sys-patch/`
- `config/uberhand/`, `config/ultrahand/`

#### 17. Clean Switch Directories
**Location**: `install.c:355-358`, `deletion_lists.h:163-226`
Deletes 55+ switch application directories:
- `switch/.overlays/`, `switch/.packages/`
- `switch/aio-switch-updater/`
- `switch/DBI_810/`, `switch/DBI_810_DE/`, `switch/DBI_810_EN/`
- `switch/tinfoil/`
- `switch/EdiZon/`, `switch/JKSV/`, `switch/checkpoint/`
- And 40+ more application directories...

#### 18. Clean Switch Files
**Location**: `install.c:360-361`, `deletion_lists.h:229-278`
Deletes 47+ NRO files and other files:
- `switch/tinfoil.nro`
- `switch/DBI.nro`
- `switch/DBI/DBI_810_*.nro` (various versions)
- `switch/daybreak.nro`
- And 40+ more files...

#### 19. Clean Root Files
**Location**: `install.c:364-367`, `deletion_lists.h:281-295`
Deletes 12 root-level CFW files:
- `boot.dat`, `boot.ini`
- `exosphere.bin`, `exosphere.ini`
- `hbmenu.nro`
- `loader.bin`, `payload.bin`
- `update.bin`, `version`
- And more...

#### 20. Clean Miscellaneous Directories
**Location**: `install.c:369-370`, `deletion_lists.h:298-309`
Deletes 9 miscellaneous directories:
- `argon/`, `games/`, `NSPs (Tools)/`
- `Patched Apps/`, `SaltySD/`
- `scripts/`, `tools/`
- `warmboot_mariko/`
- `switch/tinfoil/db/`

#### 21. Clean Miscellaneous Files
**Location**: `install.c:372-373`, `deletion_lists.h:312-322`
Deletes 8 miscellaneous files:
- `fusee-primary.bin`, `fusee.bin`
- `SaltySD/*.elf` (various bootstrap files)

#### 22. Cleanup Old Version Markers
**Location**: `install.c:308-318`, `deletion_lists.h:325-335`
Deletes legacy version marker files:
- `1.0.0l`, `1.0.0s`, `1.0.0oc`
- `1.4.0-pre`, `1.4.0-pre-c`, `1.4.0-pre-d`
- `1.4.1`, `1.5.0`

**Note**: Each deletion operation checks if path exists before attempting deletion.

---

### Step 2: File Installation
**Location**: `install.c:383-526`

#### 23. Copy Atmosphere Directory
**Location**: `install.c:400-402`
- **Source**: `{staging}/atmosphere/` (e.g., `sd:/OmniNX Standard/atmosphere/`)
- **Destination**: `sd:/atmosphere/`
- **Progress**: Recursively counts all files/directories, displays percentage
- **Tracking**: Updates progress every 10 items or when percentage changes

#### 24. Copy Bootloader Directory
**Location**: `install.c:404-406`
- **Source**: `{staging}/bootloader/`
- **Destination**: `sd:/bootloader/`
- Same progress tracking as above

#### 25. Copy Config Directory
**Location**: `install.c:408-410`
- **Source**: `{staging}/config/`
- **Destination**: `sd:/config/`
- Same progress tracking

#### 26. Copy Switch Directory
**Location**: `install.c:412-414`
- **Source**: `{staging}/switch/`
- **Destination**: `sd:/switch/`
- Same progress tracking

#### 27. Copy warmboot_mariko Directory
**Location**: `install.c:416-418`
- **Source**: `{staging}/warmboot_mariko/`
- **Destination**: `sd:/warmboot_mariko/`
- Only if directory exists in staging

#### 28. Copy SaltySD Directory (OC Variant Only)
**Location**: `install.c:421-425`
- **Source**: `{staging}/SaltySD/`
- **Destination**: `sd:/SaltySD/`
- **Condition**: Only copied if `variant == VARIANT_OC`
- **Note**: This is a large directory (~2500 files), so progress tracking is important

#### 29. Copy Root Files
**Location**: `install.c:428-466`
Copies individual files from staging root to SD root:
- `boot.dat` → `sd:/boot.dat`
- `boot.ini` → `sd:/boot.ini`
- `exosphere.ini` → `sd:/exosphere.ini`
- `hbmenu.nro` → `sd:/hbmenu.nro`
- `loader.bin` → `sd:/loader.bin`
- `payload.bin` → `sd:/payload.bin`

Each file is only copied if it exists in staging.

#### 30. Create manifest.ini
**Location**: `install.c:468-516`
Creates/overwrites `sd:/config/omninx/manifest.ini`:

**Content Structure**:
```ini
[OmniNX]
current_pack={variant}        # "standard", "light", or "oc"
version={VERSION}              # e.g., "1.0.0"
update_channel={0|1|2}        # light=0, oc=1, standard=2
channel_pack={variant}        # Same as current_pack
```

**Directory Creation**: Ensures `sd:/config/omninx/` exists before creating file.

---

### Step 3: Cleanup Staging Directory
**Location**: `install.c:626-656`

#### 31. Remove Staging Directory
**Location**: `install.c:634-652`
- Recursively deletes the entire staging directory:
  - `sd:/OmniNX Standard/` OR
  - `sd:/OmniNX Light/` OR
  - `sd:/OmniNX OC/`
- **Error Handling**: Warns if deletion fails but continues (non-fatal)

---

## Phase 4: Clean Install Mode Execution

**Trigger**: `INSTALL_MODE_CLEAN` (No OmniNX detected)

### Step 1: Backup User Data
**Location**: `backup.c:18-52`

#### 32. Create Backup Directory
- Creates `sd:/temp_backup/` directory

#### 33. Backup DBI
- **Source**: `sd:/switch/DBI/`
- **Destination**: `sd:/temp_backup/DBI/`
- Only if source exists

#### 34. Backup Tinfoil
- **Source**: `sd:/switch/tinfoil/`
- **Destination**: `sd:/temp_backup/tinfoil/`
- Only if source exists

#### 35. Backup prod.keys
- **Source**: `sd:/switch/prod.keys`
- **Destination**: `sd:/temp_backup/prod.keys`
- Only if source exists

---

### Step 2: Wipe Directories
**Location**: `install.c:543-602`

#### 36. Delete Entire Directories
Recursively deletes (if they exist):
- `sd:/atmosphere/` (entire directory tree)
- `sd:/bootloader/` (entire directory tree)
- `sd:/config/` (entire directory tree)
- `sd:/switch/` (entire directory tree)

#### 37. Delete Root Files
- Uses same deletion list as update mode (Step 19)
- Deletes `boot.dat`, `boot.ini`, `exosphere.ini`, etc.

#### 38. Delete Miscellaneous Items
- Uses same deletion lists as update mode (Steps 20-21)
- Deletes `argon/`, `games/`, `SaltySD/`, etc.

#### 39. Recreate Switch Directory
- Creates empty `sd:/switch/` directory for restoration

---

### Step 3: Restore User Data
**Location**: `backup.c:55-95`

#### 40. Restore DBI
- **Source**: `sd:/temp_backup/DBI/`
- **Destination**: `sd:/switch/DBI/`
- **Cleanup**: After restoration, deletes old DBI NRO files:
  - `sd:/switch/DBI/DBI_810_EN.nro`
  - `sd:/switch/DBI/DBI_810_DE.nro`
  - `sd:/switch/DBI/DBI_845_EN.nro`
  - `sd:/switch/DBI/DBI_845_DE.nro`
  - `sd:/switch/DBI/DBI.nro`

#### 41. Restore Tinfoil
- **Source**: `sd:/temp_backup/tinfoil/`
- **Destination**: `sd:/switch/tinfoil/`
- **Cleanup**: After restoration, deletes `sd:/switch/tinfoil/tinfoil.nro`

#### 42. Restore prod.keys
- **Source**: `sd:/temp_backup/prod.keys`
- **Destination**: `sd:/switch/prod.keys`

#### 43. Cleanup Backup Directory
**Location**: `backup.c:98-103`
- Deletes `sd:/temp_backup/` directory after restoration

---

### Step 4: Install Files
**Location**: `install.c:620-623`
- **Same as Update Mode Step 2** (Steps 23-30)
- Copies all directories and files from staging
- Creates `manifest.ini`

---

### Step 5: Cleanup Staging
- **Same as Update Mode Step 3** (Step 31)
- Removes staging directory

---

## Phase 5: Completion & Launch

### Step 44: Installation Summary
**Location**: `main.c:363-377`
- Display success message if `result == FR_OK && total_errors == 0`
- Display error count if errors occurred
- Show completion status

### Step 45: Payload Launch Wait
**Location**: `main.c:382-454`
- Wait for A button or Power button
- Check if `sd:/bootloader/update.bin` exists
- If payload exists:
  - Launch payload (relocates and executes)
- If payload doesn't exist:
  - Display error message
  - Wait for button again
  - Reboot system

---

## Technical Details

### Progress Tracking
- **File Counting**: Recursively counts all files and directories before copy
- **Progress Updates**: Updates display every 10 items or when percentage changes
- **Screen Management**: Automatically clears and reprints header when approaching bottom of screen

### Error Handling
- **Path Existence Checks**: Every operation checks if source exists before attempting
- **File System Errors**: Logged with descriptive error messages
- **Fallback Methods**: Some operations have fallback copy methods if primary fails
- **Non-Fatal Errors**: Staging directory cleanup failures are warned but don't abort installation

### File Copy Mechanism
- **Buffer Size**: 1MB (`FS_BUFFER_SIZE = 0x100000`)
- **Read/Write Verification**: Verifies bytes read/written match expected amounts
- **Attribute Preservation**: Copies file attributes (via `f_chmod`)
- **Recursive Copy**: Handles nested directory structures automatically

### Memory Management
- **Dynamic Allocation**: Uses heap for file buffers and path strings
- **Cleanup**: Frees allocated memory after use
- **Stack Pivot**: Switches to dedicated stack area for payload execution

### Detection Logic
- **Installation Detection**: Based on `manifest.ini` file, not version marker files
- **Variant Detection**: Parses `current_pack` from manifest INI format
- **Pack Detection**: Simple directory existence check (in priority order)

---

## Summary

### Update Mode Flow
1. Detect existing installation → Update mode
2. Selective cleanup (Steps 10-22)
3. Copy new files (Steps 23-30)
4. Create manifest (Step 30)
5. Cleanup staging (Step 31)

### Clean Install Mode Flow
1. No installation detected → Clean install mode
2. Backup user data (Steps 32-35)
3. Full wipe (Steps 36-39)
4. Restore user data (Steps 40-43)
5. Copy new files (Steps 23-30)
6. Create manifest (Step 30)
7. Cleanup staging (Step 31)

**Result**: In both modes, the SD card ends up with a fresh OmniNX installation while preserving user data and savegames.
