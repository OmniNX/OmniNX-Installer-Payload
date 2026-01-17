/*
 * OmniNX Installer - Deletion Lists for Update Mode
 * Arrays of paths to delete during update installation
 */

#pragma once

// Atmosphere subdirectories to delete
static const char* atmosphere_dirs_to_delete[] = {
    "sd:/atmosphere/config",
    "sd:/atmosphere/crash_reports",
    "sd:/atmosphere/erpt_reports",
    "sd:/atmosphere/exefs_patches/CrunchPatch",
    "sd:/atmosphere/exefs_patches/Crunchyroll Patch 1.10.0",
    "sd:/atmosphere/exefs_patches/bluetooth_patches",
    "sd:/atmosphere/exefs_patches/bootlogo",
    "sd:/atmosphere/exefs_patches/btm_patches",
    "sd:/atmosphere/exefs_patches/es_patches",
    "sd:/atmosphere/exefs_patches/hid_patches",
    "sd:/atmosphere/exefs_patches/logo1",
    "sd:/atmosphere/exefs_patches/nfim_ctest",
    "sd:/atmosphere/exefs_patches/nim_ctest",
    "sd:/atmosphere/exefs_patches/nvnflinger_cmu",
    "sd:/atmosphere/extrazz",
    "sd:/atmosphere/fatal_errors",
    "sd:/atmosphere/fatal_reports",
    "sd:/atmosphere/flags",
    "sd:/atmosphere/hbl_html",
    "sd:/atmosphere/hosts",
    "sd:/atmosphere/kips",
    "sd:/atmosphere/kip1",
    "sd:/atmosphere/kip_patches",
    "sd:/atmosphere/logs",
    NULL
};

// Atmosphere root directories (title IDs)
static const char* atmosphere_root_dirs_to_delete[] = {
    "sd:/atmosphere/0000000000534C56",
    "sd:/atmosphere/00FF0000B378D640",
    "sd:/atmosphere/00FF0000636C6BFF",
    "sd:/atmosphere/00FF0000A53BB665",
    "sd:/atmosphere/0100000000000008",
    "sd:/atmosphere/010000000000000D",
    "sd:/atmosphere/010000000000002B",
    "sd:/atmosphere/0100000000000032",
    "sd:/atmosphere/0100000000000034",
    "sd:/atmosphere/0100000000000036",
    "sd:/atmosphere/0100000000000037",
    "sd:/atmosphere/010000000000003C",
    "sd:/atmosphere/0100000000000042",
    "sd:/atmosphere/0100000000000F12",
    "sd:/atmosphere/0100000000001000",
    "sd:/atmosphere/0100000000001007",
    "sd:/atmosphere/0100000000001013",
    "sd:/atmosphere/010000000000DA7A",
    "sd:/atmosphere/010000000000bd00",
    "sd:/atmosphere/01006a800016e000",
    "sd:/atmosphere/01009D901BC56000",
    "sd:/atmosphere/0100A3900C3E2000",
    "sd:/atmosphere/0100F43008C44000",
    "sd:/atmosphere/050000BADDAD0000",
    "sd:/atmosphere/4200000000000000",
    "sd:/atmosphere/420000000000000B",
    "sd:/atmosphere/420000000000000E",
    "sd:/atmosphere/4200000000000010",
    "sd:/atmosphere/4200000000000FFF",
    "sd:/atmosphere/420000000007E51A",
    "sd:/atmosphere/420000000007E51B",
    "sd:/atmosphere/690000000000000D",
    NULL
};

// Atmosphere contents directories (title IDs)
static const char* atmosphere_contents_dirs_to_delete[] = {
    "sd:/atmosphere/contents/0000000000534C56",
    "sd:/atmosphere/contents/00FF0000B378D640",
    "sd:/atmosphere/contents/00FF0000636C6BFF",
    "sd:/atmosphere/contents/00FF0000A53BB665",
    "sd:/atmosphere/contents/0100000000000008",
    "sd:/atmosphere/contents/010000000000000D",
    "sd:/atmosphere/contents/010000000000002B",
    "sd:/atmosphere/contents/0100000000000032",
    "sd:/atmosphere/contents/0100000000000034",
    "sd:/atmosphere/contents/0100000000000036",
    "sd:/atmosphere/contents/0100000000000037",
    "sd:/atmosphere/contents/010000000000003C",
    "sd:/atmosphere/contents/0100000000000042",
    "sd:/atmosphere/contents/0100000000000895",
    "sd:/atmosphere/contents/0100000000000F12",
    "sd:/atmosphere/contents/0100000000001000",
    "sd:/atmosphere/contents/0100000000001007",
    "sd:/atmosphere/contents/0100000000001013",
    "sd:/atmosphere/contents/010000000000DA7A",
    "sd:/atmosphere/contents/010000000000bd00",
    "sd:/atmosphere/contents/01006a800016e000",
    "sd:/atmosphere/contents/01009D901BC56000",
    "sd:/atmosphere/contents/0100A3900C3E2000",
    "sd:/atmosphere/contents/0100F43008C44000",
    "sd:/atmosphere/contents/050000BADDAD0000",
    "sd:/atmosphere/contents/4200000000000000",
    "sd:/atmosphere/contents/420000000000000B",
    "sd:/atmosphere/contents/420000000000000E",
    "sd:/atmosphere/contents/4200000000000010",
    "sd:/atmosphere/contents/4200000000000FFF",
    "sd:/atmosphere/contents/420000000007E51A",
    "sd:/atmosphere/contents/420000000007E51B",
    "sd:/atmosphere/contents/690000000000000D",
    NULL
};

// Atmosphere files to delete
static const char* atmosphere_files_to_delete[] = {
    "sd:/atmosphere/config/exosphere.ini",
    "sd:/atmosphere/config/override_config.ini",
    "sd:/atmosphere/config/stratosphere.ini",
    "sd:/atmosphere/hbl.nsp",
    "sd:/atmosphere/package3",
    "sd:/atmosphere/reboot_payload.bin",
    "sd:/atmosphere/stratosphere.romfs",
    NULL
};

// Bootloader directories to delete
static const char* bootloader_dirs_to_delete[] = {
    "sd:/bootloader/boot",
    "sd:/bootloader/bootlogo",
    "sd:/bootloader/ini2",
    "sd:/bootloader/payloads",
    "sd:/bootloader/reboot",
    "sd:/bootloader/res",
    "sd:/bootloader/sys",
    NULL
};

// Bootloader files to delete
static const char* bootloader_files_to_delete[] = {
    "sd:/bootloader/ArgonNX.bin",
    "sd:/bootloader/bootlogo.bmp",
    "sd:/bootloader/hekate_ipl.ini",
    "sd:/bootloader/nyx.ini",
    "sd:/bootloader/patches.ini",
    "sd:/bootloader/update.bin",
    "sd:/bootloader/ini/EmuMMC ohne Mods.ini",
    NULL
};

// Config directories to delete
static const char* config_dirs_to_delete[] = {
    "sd:/config/aio-switch-updater",
    "sd:/config/blue_pack_updater",
    "sd:/config/kefir-updater",
    "sd:/config/nx-hbmenu",
    "sd:/config/quickntp",
    "sd:/config/sys-con",
    "sd:/config/sys-patch",
    "sd:/config/uberhand",
    "sd:/config/ultrahand",
    NULL
};

// Switch directories to delete
static const char* switch_dirs_to_delete[] = {
    "sd:/switch/.overlays",
    "sd:/switch/.packages",
    "sd:/switch/90DNS_tester",
    "sd:/switch/aio-switch-updater",
    "sd:/switch/amsPLUS-downloader",
    "sd:/switch/appstore",
    "sd:/switch/AtmoXL-Titel-Installer",
    "sd:/switch/breeze",
    "sd:/switch/checkpoint",
    "sd:/switch/cheats-updater",
    "sd:/switch/chiaki",
    "sd:/switch/ChoiDujourNX",
    "sd:/switch/crash_ams",
    "sd:/switch/Daybreak",
    "sd:/switch/DBI_658_EN",
    "sd:/switch/DBI_810",
    "sd:/switch/DBI_810_DE",
    "sd:/switch/DBI_810_EN",
    "sd:/switch/DBI_RU",
    "sd:/switch/DNS_mitm Tester",
    "sd:/switch/EdiZon",
    "sd:/switch/Fizeau",
    "sd:/switch/FTPD",
    "sd:/switch/fw-downloader",
    "sd:/switch/gamecard_installer",
    "sd:/switch/Goldleaf",
    "sd:/switch/haze",
    "sd:/switch/JKSV",
    "sd:/switch/kefir-updater",
    "sd:/switch/ldnmitm_config",
    "sd:/switch/Linkalho",
    "sd:/switch/Moonlight-Switch",
    "sd:/switch/Neumann",
    "sd:/switch/NX-Activity-Log",
    "sd:/switch/NX-Save-Sync",
    "sd:/switch/NX-Shell",
    "sd:/switch/NX-Update-Checker ",
    "sd:/switch/NXGallery",
    "sd:/switch/NXRemoteLauncher",
    "sd:/switch/NXThemesInstaller",
    "sd:/switch/nxdumptool",
    "sd:/switch/nxmtp",
    "sd:/switch/Payload_launcher",
    "sd:/switch/Reboot",
    "sd:/switch/reboot_to_argonNX",
    "sd:/switch/reboot_to_hekate",
    "sd:/switch/Shutdown_System",
    "sd:/switch/SimpleModDownloader",
    "sd:/switch/SimpleModManager",
    "sd:/switch/sphaira",
    "sd:/switch/studious-pancake",
    "sd:/switch/Switch-Time",
    "sd:/switch/SwitchIdent",
    "sd:/switch/Switch_themes_Installer",
    "sd:/switch/Switchfin",
    "sd:/switch/Sys-Clk Manager",
    "sd:/switch/Sys-Con",
    "sd:/switch/sys-clk-manager",
    "sd:/switch/themezer-nx",
    "sd:/switch/themezernx",
    "sd:/switch/tinwoo",
    NULL
};

// Switch files (NRO) to delete
static const char* switch_files_to_delete[] = {
    "sd:/switch/90DNS_tester/90DNS_tester.nro",
    "sd:/switch/breeze.nro",
    "sd:/switch/cheats-updater.nro",
    "sd:/switch/chiaki.nro",
    "sd:/switch/ChoiDujourNX.nro",
    "sd:/switch/daybreak.nro",
    "sd:/switch/DBI.nro",
    "sd:/switch/DBI/DBI.nro",
    "sd:/switch/DBI/DBI_810_DE.nro",
    "sd:/switch/DBI/DBI_810_EN.nro",
    "sd:/switch/DBI/DBI_845_DE.nro",
    "sd:/switch/DBI/DBI_845_EN.nro",
    "sd:/switch/DBI/DBI_849_DE.nro",
    "sd:/switch/DBI/DBI_849_EN.nro",
    "sd:/switch/DBI_810_DE/DBI_810.nro",
    "sd:/switch/DBI_810_DE/DBI_810_DE.nro",
    "sd:/switch/DBI_810_EN/DBI_810_EN.nro",
    "sd:/switch/DBI_RU/DBI_RU.nro",
    "sd:/switch/DNS_mitm Tester.nro",
    "sd:/switch/EdiZon.nro",
    "sd:/switch/Fizeau.nro",
    "sd:/switch/Goldleaf.nro",
    "sd:/switch/haze.nro",
    "sd:/switch/JKSV.nro",
    "sd:/switch/ldnmitm_config.nro",
    "sd:/switch/linkalho.nro",
    "sd:/switch/Moonlight-Switch.nro",
    "sd:/switch/Neumann.nro",
    "sd:/switch/NX-Shell.nro",
    "sd:/switch/NXGallery.nro",
    "sd:/switch/NXThemesInstaller.nro",
    "sd:/switch/nxdumptool.nro",
    "sd:/switch/nxtc.bin",
    "sd:/switch/reboot_to_payload.nro",
    "sd:/switch/SimpleModDownloader.nro",
    "sd:/switch/SimpleModManager.nro",
    "sd:/switch/sphaira.nro",
    "sd:/switch/SwitchIdent.nro",
    "sd:/switch/Switch_themes_Installer/NXThemesInstaller.nro",
    "sd:/switch/Switchfin.nro",
    "sd:/switch/Sys-Clk Manager/sys-clk-manager.nro",
    "sd:/switch/Sys-Con.nro",
    "sd:/switch/sys-clk-manager.nro",
    "sd:/switch/tinfoil.nro",
    "sd:/switch/tinfoil/tinfoil.nro",
    "sd:/switch/tinwoo.nro",
    "sd:/switch/tinwoo/tinwoo.nro",
    NULL
};

// Root CFW files to delete
static const char* root_files_to_delete[] = {
    "sd:/boot.dat",
    "sd:/boot.ini",
    "sd:/exosphere.bin",
    "sd:/exosphere.ini",
    "sd:/hbmenu.nro",
    "sd:/install.bat",
    "sd:/license",
    "sd:/loader.bin",
    "sd:/mc-mitm.log",
    "sd:/payload.bin",
    "sd:/update.bin",
    "sd:/version",
    NULL
};

// Miscellaneous directories to delete
static const char* misc_dirs_to_delete[] = {
    "sd:/argon",
    "sd:/games",
    "sd:/NSPs (Tools)",
    "sd:/Patched Apps",
    "sd:/SaltySD",
    "sd:/scripts",
    "sd:/switch/tinfoil/db",
    "sd:/tools",
    "sd:/warmboot_mariko",
    NULL
};

// Miscellaneous files to delete
static const char* misc_files_to_delete[] = {
    "sd:/fusee-primary.bin",
    "sd:/fusee.bin",
    "sd:/SaltySD/exceptions.txt",
    "sd:/SaltySD/saltysd_bootstrap.elf",
    "sd:/SaltySD/saltysd_bootstrap32_3k.elf",
    "sd:/SaltySD/saltysd_bootstrap32_5k.elf",
    "sd:/SaltySD/saltysd_core.elf",
    "sd:/SaltySD/saltysd_core32.elf",
    NULL
};

// Old version marker files to delete
static const char* old_version_files_to_delete[] = {
    "sd:/1.0.0l",
    "sd:/1.0.0s",
    "sd:/1.0.0oc",
    "sd:/1.4.0-pre",
    "sd:/1.4.0-pre-c",
    "sd:/1.4.0-pre-d",
    "sd:/1.4.1",
    "sd:/1.5.0",
    NULL
};

// Helper function to count array elements (excluding NULL terminator)
static inline int count_paths(const char* paths[]) {
    int count = 0;
    while (paths[count] != NULL) count++;
    return count;
}
