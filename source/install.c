/*
 * OmniNX Installer - Installation Logic
 */

#include "install.h"
#include "backup.h"
#include "deletion_lists.h"
#include "fs.h"
#include "version.h"
#include "gfx.h"
#include <libs/fatfs/ff.h>
#include <string.h>
#include <utils/sprintf.h>

#ifndef VERSION
#define VERSION "1.0.0"
#endif

// Forward declaration
static void combine_path(char *result, size_t size, const char *base, const char *add);

// Color definitions (some already defined in types.h, but we override for consistency)
#undef COLOR_CYAN
#undef COLOR_WHITE
#undef COLOR_GREEN
#undef COLOR_YELLOW
#undef COLOR_ORANGE
#undef COLOR_RED
#define COLOR_CYAN    0xFF00FFFF
#define COLOR_WHITE   0xFFFFFFFF
#define COLOR_GREEN   0xFF00FF00
#define COLOR_YELLOW  0xFFFFDD00
#define COLOR_ORANGE  0xFF00A5FF
#define COLOR_RED     0xFFFF0000

static void set_color(u32 color) {
    gfx_con_setcol(color, gfx_con.fillbg, gfx_con.bgcol);
}

// Check if we need to clear screen (when getting close to bottom)
static void check_and_clear_screen_if_needed(void) {
    // In the gfx system:
    // - gfx_con.x is the vertical position (line number, increments by 16 per line)
    // - gfx_con.y is the horizontal position (pixels from left)
    // - Screen is 720px wide, 1280px tall
    // - Font is 16px, so we have 720/16 = 45 lines before wrapping
    // - When gfx_con.x > 720-16, it wraps to x=0, causing overwrite
    
    // Clear when we're past ~35 lines (35 * 16 = 560 pixels) to leave room
    if (gfx_con.x > 35 * 16) {
        // Clear screen and reset position
        gfx_clear_grey(0x1B);
        gfx_con_setpos(0, 0);
        
        // Reprint same header as initial launch
        set_color(COLOR_CYAN);
        gfx_printf("========================================\n");
        gfx_printf("  OmniNX Installer Payload v%s\n", VERSION);
        gfx_printf("========================================\n\n");
        set_color(COLOR_WHITE);
    }
}

// Check if file/directory exists
static bool path_exists(const char *path) {
    FILINFO fno;
    return (f_stat(path, &fno) == FR_OK);
}

// Count total items (files + directories) in a directory tree recursively
static int count_directory_items(const char *path) {
    DIR dir;
    FILINFO fno;
    int res;
    int count = 0;
    
    res = f_opendir(&dir, path);
    if (res != FR_OK) {
        return 0;
    }
    
    while (1) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) break;
        
        // Skip . and ..
        if (fno.fname[0] == '.' && (fno.fname[1] == '\0' || (fno.fname[1] == '.' && fno.fname[2] == '\0'))) {
            continue;
        }
        
        count++; // Count this item
        
        // If it's a directory, recursively count its contents
        if (fno.fattrib & AM_DIR) {
            char sub_path[256];
            s_printf(sub_path, "%s/%s", path, fno.fname);
            count += count_directory_items(sub_path);
        }
    }
    
    f_closedir(&dir);
    return count;
}

// Helper to combine paths (handles trailing slashes properly)
static void combine_path(char *result, size_t size, const char *base, const char *add) {
    size_t base_len = strlen(base);
    if (base_len > 0 && base[base_len - 1] == '/') {
        // Base has trailing slash, just append
        s_printf(result, "%s%s", base, add);
    } else {
        // No trailing slash, add one
        s_printf(result, "%s/%s", base, add);
    }
}

// Recursive folder copy with progress tracking
static int folder_copy_progress_recursive(const char *src, const char *dst, int *copied, int total, u32 start_x, u32 start_y, const char *display_name, int *last_percent) {
    DIR dir;
    FILINFO fno;
    int res;
    char src_full[256];
    char dst_full[256];
    char dst_dir[256];
    
    res = f_opendir(&dir, src);
    if (res != FR_OK) {
        return res;
    }
    
    // Get folder name from src path
    const char *folder_name = strrchr(src, '/');
    if (folder_name) {
        folder_name++;
    } else {
        folder_name = src;
    }
    
    // Create destination folder path (handle trailing slash in dst)
    combine_path(dst_dir, sizeof(dst_dir), dst, folder_name);
    
    // Try to create the directory (ignore if it already exists)
    res = f_mkdir(dst_dir);
    if (res == FR_EXIST) {
        res = FR_OK;  // Directory already exists, that's fine
    } else if (res != FR_OK) {
        // If mkdir fails, check if it's actually a file (shouldn't happen, but be safe)
        FILINFO fno;
        if (f_stat(dst_dir, &fno) == FR_OK && !(fno.fattrib & AM_DIR)) {
            // Destination exists but is a file, not a directory - this is an error
            f_closedir(&dir);
            return FR_DENIED;
        }
        // If it's a directory, continue (might have been created between check and mkdir)
        if (f_stat(dst_dir, &fno) == FR_OK && (fno.fattrib & AM_DIR)) {
            res = FR_OK;
        } else {
            f_closedir(&dir);
            return res;
        }
    }
    
    // Copy contents
    while (1) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) break;
        
        // Skip . and ..
        if (fno.fname[0] == '.' && (fno.fname[1] == '\0' || (fno.fname[1] == '.' && fno.fname[2] == '\0'))) {
            continue;
        }
        
        // Build source and destination paths
        combine_path(src_full, sizeof(src_full), src, fno.fname);
        combine_path(dst_full, sizeof(dst_full), dst_dir, fno.fname);
        
        if (fno.fattrib & AM_DIR) {
            res = folder_copy_progress_recursive(src_full, dst_dir, copied, total, start_x, start_y, display_name, last_percent);
            // Increment counter for directory (it was counted in total)
            (*copied)++;
        } else {
            res = file_copy(src_full, dst_full);
            (*copied)++;
        }
        
        // Update progress every 10 items or when percentage changes
        if ((*copied % 10 == 0 || total == 0) && res == FR_OK) {
            int percent = total > 0 ? (*copied * 100) / total : 0;
            if (percent != *last_percent || *copied % 50 == 0) {
                gfx_con_setpos(start_x, start_y);
                set_color(COLOR_CYAN);
                gfx_printf("  Kopiere: %s [%3d%%] (%d/%d)", display_name, percent, *copied, total);
                set_color(COLOR_WHITE);
                *last_percent = percent;
            }
        }
        
        if (res != FR_OK) break;
    }
    
    f_closedir(&dir);
    return res;
}

// Progress-aware folder copy (improved version)
static int folder_copy_with_progress_v2(const char *src, const char *dst, const char *display_name) {
    int copied = 0;
    int total = 0;
    int last_percent = -1;
    u32 start_x, start_y;
    int res;
    
    // Check if source exists
    if (!path_exists(src)) {
        set_color(COLOR_ORANGE);
        gfx_printf("  Ueberspringe: %s (nicht gefunden)\n", display_name);
        set_color(COLOR_WHITE);
        return FR_NO_FILE;
    }
    
    // Count total items first
    total = count_directory_items(src);
    
    if (total == 0) {
        // Empty directory, just create destination
        const char *folder_name = strrchr(src, '/');
        if (folder_name) folder_name++;
        else folder_name = src;
        char dst_path[256];
        combine_path(dst_path, sizeof(dst_path), dst, folder_name);
        res = f_mkdir(dst_path);
        if (res == FR_OK || res == FR_EXIST) {
            return FR_OK;
        }
        return res;
    }
    
    // Save cursor position
    gfx_con_getpos(&start_x, &start_y);
    
    // Show initial status
    set_color(COLOR_CYAN);
    gfx_printf("  Kopiere: %s [  0%%] (0/%d)", display_name, total);
    set_color(COLOR_WHITE);
    
    // Perform the copy with progress updates
    res = folder_copy_progress_recursive(src, dst, &copied, total, start_x, start_y, display_name, &last_percent);
    
    // Final update - overwrite the same line
    gfx_con_setpos(start_x, start_y);
    if (res == FR_OK) {
        set_color(COLOR_GREEN);
        gfx_printf("  Kopiere: %s [100%%] (%d/%d) - Fertig!\n", display_name, copied, total);
        set_color(COLOR_WHITE);
    } else {
        set_color(COLOR_RED);
        gfx_printf("  Kopiere: %s - Fehlgeschlagen!\n", display_name);
        gfx_printf("  Fehler: %s (Code=%d)\n", fs_error_str(res), res);
        gfx_printf("  Quelle: %s\n", src);
        gfx_printf("  Ziel: %s\n", dst);
        set_color(COLOR_WHITE);
        
        // Fallback: try using the original folder_copy function
        set_color(COLOR_ORANGE);
        gfx_printf("  Versuche alternative Kopiermethode...\n");
        set_color(COLOR_WHITE);
        int fallback_res = folder_copy(src, dst);
        if (fallback_res == FR_OK) {
            set_color(COLOR_GREEN);
            gfx_printf("  Alternative Kopie erfolgreich!\n");
            set_color(COLOR_WHITE);
            return FR_OK;
        }
    }
    
    return res;
}

// Delete a list of paths
int delete_path_list(const char* paths[], const char* description) {
    int res;
    int deleted = 0;
    int failed = 0;
    
    for (int i = 0; paths[i] != NULL; i++) {
        if (path_exists(paths[i])) {
            FILINFO fno;
            f_stat(paths[i], &fno);
            
            if (fno.fattrib & AM_DIR) {
                res = folder_delete(paths[i]);
            } else {
                res = f_unlink(paths[i]);
            }
            
            if (res == FR_OK || res == FR_NO_FILE) {
                deleted++;
            } else {
                failed++;
            }
        }
    }
    
    return (failed == 0) ? FR_OK : FR_DISK_ERR;
}

// Delete old version markers (legacy files from old system)
int cleanup_old_version_markers(omninx_variant_t current_variant) {
    // Delete all old version files (they're no longer used)
    for (int i = 0; old_version_files_to_delete[i] != NULL; i++) {
        const char* path = old_version_files_to_delete[i];
        if (path_exists(path)) {
            f_unlink(path);
        }
    }
    
    return FR_OK;
}

// Update mode: Cleanup specific directories/files
int update_mode_cleanup(omninx_variant_t variant) {
    check_and_clear_screen_if_needed();
    
    set_color(COLOR_CYAN);
    gfx_printf("  Bereinige: atmosphere/\n");
    set_color(COLOR_WHITE);
    // Delete atmosphere subdirectories
    delete_path_list(atmosphere_dirs_to_delete, "atmosphere subdirs");
    
    // Delete atmosphere root directories (title IDs)
    delete_path_list(atmosphere_root_dirs_to_delete, "atmosphere root dirs");
    
    // Delete atmosphere contents directories (title IDs)
    delete_path_list(atmosphere_contents_dirs_to_delete, "atmosphere contents dirs");
    
    // Delete atmosphere files
    delete_path_list(atmosphere_files_to_delete, "atmosphere files");
    
    set_color(COLOR_CYAN);
    gfx_printf("  Bereinige: bootloader/\n");
    set_color(COLOR_WHITE);
    // Delete bootloader directories
    delete_path_list(bootloader_dirs_to_delete, "bootloader dirs");
    
    // Delete bootloader files
    delete_path_list(bootloader_files_to_delete, "bootloader files");
    
    set_color(COLOR_CYAN);
    gfx_printf("  Bereinige: config/\n");
    set_color(COLOR_WHITE);
    // Delete config directories
    delete_path_list(config_dirs_to_delete, "config dirs");
    
    set_color(COLOR_CYAN);
    gfx_printf("  Bereinige: switch/\n");
    set_color(COLOR_WHITE);
    // Delete switch directories
    delete_path_list(switch_dirs_to_delete, "switch dirs");
    
    // Delete switch files
    delete_path_list(switch_files_to_delete, "switch files");
    
    set_color(COLOR_CYAN);
    gfx_printf("  Bereinige: Root-Dateien\n");
    set_color(COLOR_WHITE);
    // Delete root files
    delete_path_list(root_files_to_delete, "root files");
    
    // Delete miscellaneous directories
    delete_path_list(misc_dirs_to_delete, "misc dirs");
    
    // Delete miscellaneous files
    delete_path_list(misc_files_to_delete, "misc files");
    
    set_color(COLOR_GREEN);
    gfx_printf("  Bereinigung abgeschlossen!\n");
    set_color(COLOR_WHITE);
    
    return FR_OK;
}

// Update mode: Copy files from staging
int update_mode_install(omninx_variant_t variant) {
    int res;
    const char* staging = get_staging_path(variant);
    char src_path[256];
    char dst_path[256];
    
    if (!staging) {
        return FR_INVALID_PARAMETER;
    }
    
    check_and_clear_screen_if_needed();
    
    set_color(COLOR_YELLOW);
    gfx_printf("Dateien werden kopiert...\n");
    set_color(COLOR_WHITE);
    
    // Copy directories (use progress-aware copy for large directories)
    s_printf(src_path, "%s/atmosphere", staging);
    res = folder_copy_with_progress_v2(src_path, "sd:/", "atmosphere/");
    if (res != FR_OK && res != FR_NO_FILE) return res;
    
    s_printf(src_path, "%s/bootloader", staging);
    res = folder_copy_with_progress_v2(src_path, "sd:/", "bootloader/");
    if (res != FR_OK && res != FR_NO_FILE) return res;
    
    s_printf(src_path, "%s/config", staging);
    res = folder_copy_with_progress_v2(src_path, "sd:/", "config/");
    if (res != FR_OK && res != FR_NO_FILE) return res;
    
    s_printf(src_path, "%s/switch", staging);
    res = folder_copy_with_progress_v2(src_path, "sd:/", "switch/");
    if (res != FR_OK && res != FR_NO_FILE) return res;
    
    s_printf(src_path, "%s/warmboot_mariko", staging);
    res = folder_copy_with_progress_v2(src_path, "sd:/", "warmboot_mariko/");
    if (res != FR_OK && res != FR_NO_FILE) return res;
    
    // OC variant includes SaltySD (this is the large one with ~2500 files)
    if (variant == VARIANT_OC) {
        s_printf(src_path, "%s/SaltySD", staging);
        res = folder_copy_with_progress_v2(src_path, "sd:/", "SaltySD/");
        if (res != FR_OK && res != FR_NO_FILE) return res;
    }
    
    // Copy root files
    set_color(COLOR_CYAN);
    gfx_printf("  Kopiere Root-Dateien...\n");
    set_color(COLOR_WHITE);
    
    s_printf(src_path, "%s/boot.dat", staging);
    s_printf(dst_path, "sd:/boot.dat");
    if (path_exists(src_path)) {
        file_copy(src_path, dst_path);
    }
    
    s_printf(src_path, "%s/boot.ini", staging);
    s_printf(dst_path, "sd:/boot.ini");
    if (path_exists(src_path)) {
        file_copy(src_path, dst_path);
    }
    
    s_printf(src_path, "%s/exosphere.ini", staging);
    s_printf(dst_path, "sd:/exosphere.ini");
    if (path_exists(src_path)) {
        file_copy(src_path, dst_path);
    }
    
    s_printf(src_path, "%s/hbmenu.nro", staging);
    s_printf(dst_path, "sd:/hbmenu.nro");
    if (path_exists(src_path)) {
        file_copy(src_path, dst_path);
    }
    
    s_printf(src_path, "%s/loader.bin", staging);
    s_printf(dst_path, "sd:/loader.bin");
    if (path_exists(src_path)) {
        file_copy(src_path, dst_path);
    }
    
    s_printf(src_path, "%s/payload.bin", staging);
    s_printf(dst_path, "sd:/payload.bin");
    if (path_exists(src_path)) {
        file_copy(src_path, dst_path);
    }
    
    // Create manifest.ini file
    set_color(COLOR_CYAN);
    gfx_printf("  Erstelle manifest.ini...\n");
    set_color(COLOR_WHITE);
    
    // Ensure config/omninx directory exists
    s_printf(dst_path, "sd:/config/omninx");
    f_mkdir(dst_path);
    
    // Determine pack name and update channel
    const char* pack_name;
    int update_channel;
    switch (variant) {
        case VARIANT_STANDARD:
            pack_name = "standard";
            update_channel = 2;
            break;
        case VARIANT_LIGHT:
            pack_name = "light";
            update_channel = 0;
            break;
        case VARIANT_OC:
            pack_name = "oc";
            update_channel = 1;
            break;
        default:
            pack_name = "unknown";
            update_channel = 0;
            break;
    }
    
    // Create manifest.ini
    s_printf(dst_path, "sd:/config/omninx/manifest.ini");
    FIL fp;
    if (f_open(&fp, dst_path, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
        f_printf(&fp, "[OmniNX]\n");
        f_printf(&fp, "current_pack=%s\n", pack_name);
        f_printf(&fp, "version=%s\n", VERSION);
        f_printf(&fp, "update_channel=%d\n", update_channel);
        f_printf(&fp, "channel_pack=%s\n", pack_name);
        f_close(&fp);
        set_color(COLOR_GREEN);
        gfx_printf("  [OK] manifest.ini erstellt\n");
        set_color(COLOR_WHITE);
    } else {
        set_color(COLOR_ORANGE);
        gfx_printf("  [WARN] manifest.ini konnte nicht erstellt werden\n");
        set_color(COLOR_WHITE);
    }
    
    // Cleanup old version markers (legacy files)
    cleanup_old_version_markers(variant);
    
    set_color(COLOR_GREEN);
    gfx_printf("  Kopie abgeschlossen!\n");
    set_color(COLOR_WHITE);
    
    return FR_OK;
}

// Clean mode: Backup user data
int clean_mode_backup(void) {
    set_color(COLOR_CYAN);
    gfx_printf("  Sichere: DBI, Tinfoil, prod.keys\n");
    set_color(COLOR_WHITE);
    int res = backup_user_data();
    if (res == FR_OK) {
        set_color(COLOR_GREEN);
        gfx_printf("  [OK] Sicherung abgeschlossen\n");
        set_color(COLOR_WHITE);
    }
    return res;
}

// Clean mode: Wipe directories
int clean_mode_wipe(void) {
    int res;
    
    // Delete entire directories
    if (path_exists("sd:/atmosphere")) {
        set_color(COLOR_CYAN);
        gfx_printf("  Loesche: atmosphere/\n");
        set_color(COLOR_WHITE);
        res = folder_delete("sd:/atmosphere");
        if (res != FR_OK && res != FR_NO_FILE) return res;
    }
    
    if (path_exists("sd:/bootloader")) {
        set_color(COLOR_CYAN);
        gfx_printf("  Loesche: bootloader/\n");
        set_color(COLOR_WHITE);
        res = folder_delete("sd:/bootloader");
        if (res != FR_OK && res != FR_NO_FILE) return res;
    }
    
    if (path_exists("sd:/config")) {
        set_color(COLOR_CYAN);
        gfx_printf("  Loesche: config/\n");
        set_color(COLOR_WHITE);
        res = folder_delete("sd:/config");
        if (res != FR_OK && res != FR_NO_FILE) return res;
    }
    
    if (path_exists("sd:/switch")) {
        set_color(COLOR_CYAN);
        gfx_printf("  Loesche: switch/\n");
        set_color(COLOR_WHITE);
        res = folder_delete("sd:/switch");
        if (res != FR_OK && res != FR_NO_FILE) return res;
    }
    
    // Delete root files
    set_color(COLOR_CYAN);
    gfx_printf("  Bereinige: Root-Dateien\n");
    set_color(COLOR_WHITE);
    delete_path_list(root_files_to_delete, "root files");
    
    // Delete miscellaneous directories
    delete_path_list(misc_dirs_to_delete, "misc dirs");
    
    // Delete miscellaneous files
    delete_path_list(misc_files_to_delete, "misc files");
    
    // Recreate switch directory
    set_color(COLOR_CYAN);
    gfx_printf("  Erstelle: switch/\n");
    set_color(COLOR_WHITE);
    f_mkdir("sd:/switch");
    
    set_color(COLOR_GREEN);
    gfx_printf("  Bereinigung abgeschlossen!\n");
    set_color(COLOR_WHITE);
    
    return FR_OK;
}

// Clean mode: Restore user data
int clean_mode_restore(void) {
    set_color(COLOR_CYAN);
    gfx_printf("  Stelle wieder her: DBI, Tinfoil, prod.keys\n");
    set_color(COLOR_WHITE);
    int res = restore_user_data();
    if (res == FR_OK) {
        set_color(COLOR_GREEN);
        gfx_printf("  [OK] Wiederherstellung abgeschlossen\n");
        set_color(COLOR_WHITE);
    }
    cleanup_backup();  // Clean up temp backup
    return res;
}

// Clean mode: Install files
int clean_mode_install(omninx_variant_t variant) {
    // Same as update mode install
    return update_mode_install(variant);
}

// Remove staging directory after installation
int cleanup_staging_directory(omninx_variant_t pack_variant) {
    const char* staging = get_staging_path(pack_variant);
    if (!staging) {
        return FR_INVALID_PARAMETER;
    }
    
    check_and_clear_screen_if_needed();
    
    if (path_exists(staging)) {
        set_color(COLOR_YELLOW);
        gfx_printf("\nEntferne Installationsordner...\n");
        set_color(COLOR_WHITE);
        set_color(COLOR_CYAN);
        gfx_printf("  Loesche: %s\n", staging);
        set_color(COLOR_WHITE);
        
        int res = folder_delete(staging);
        if (res == FR_OK) {
            set_color(COLOR_GREEN);
            gfx_printf("  [OK] Installationsordner entfernt\n");
            set_color(COLOR_WHITE);
        } else {
            set_color(COLOR_ORANGE);
            gfx_printf("  [WARN] Ordner konnte nicht entfernt werden (err=%d)\n", res);
            set_color(COLOR_WHITE);
        }
        return res;
    }
    
    return FR_OK;
}

// Main installation function
int perform_installation(omninx_variant_t pack_variant, install_mode_t mode) {
    int res;
    
    if (mode == INSTALL_MODE_UPDATE) {
        // Update mode: selective cleanup then install
        set_color(COLOR_YELLOW);
        gfx_printf("Schritt 1: Bereinigung...\n");
        set_color(COLOR_WHITE);
        res = update_mode_cleanup(pack_variant);
        if (res != FR_OK) return res;
        
        check_and_clear_screen_if_needed();
        gfx_printf("\n");
        set_color(COLOR_YELLOW);
        gfx_printf("Schritt 2: Dateien kopieren...\n");
        set_color(COLOR_WHITE);
        res = update_mode_install(pack_variant);
        if (res != FR_OK) return res;
        
        check_and_clear_screen_if_needed();
        // Remove staging directory
        res = cleanup_staging_directory(pack_variant);
        return res;
    } else {
        // Clean mode: backup, wipe, restore, install
        set_color(COLOR_YELLOW);
        gfx_printf("Schritt 1: Sichere Benutzerdaten...\n");
        set_color(COLOR_WHITE);
        res = clean_mode_backup();
        if (res != FR_OK) return res;
        
        check_and_clear_screen_if_needed();
        gfx_printf("\n");
        set_color(COLOR_YELLOW);
        gfx_printf("Schritt 2: Bereinige alte Installation...\n");
        set_color(COLOR_WHITE);
        res = clean_mode_wipe();
        if (res != FR_OK) return res;
        
        check_and_clear_screen_if_needed();
        gfx_printf("\n");
        set_color(COLOR_YELLOW);
        gfx_printf("Schritt 3: Stelle Benutzerdaten wieder her...\n");
        set_color(COLOR_WHITE);
        res = clean_mode_restore();
        if (res != FR_OK) return res;
        
        check_and_clear_screen_if_needed();
        gfx_printf("\n");
        set_color(COLOR_YELLOW);
        gfx_printf("Schritt 4: Dateien kopieren...\n");
        set_color(COLOR_WHITE);
        res = clean_mode_install(pack_variant);
        if (res != FR_OK) return res;
        
        check_and_clear_screen_if_needed();
        // Remove staging directory
        res = cleanup_staging_directory(pack_variant);
        return res;
    }
}
