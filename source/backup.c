/*
 * OmniNX Installer - Backup and Restore Operations
 */

#include "backup.h"
#include "fs.h"
#include <libs/fatfs/ff.h>
#include <string.h>
#include <utils/sprintf.h>

// Check if file/directory exists
static bool path_exists(const char *path) {
    FILINFO fno;
    return (f_stat(path, &fno) == FR_OK);
}

// Backup user data before clean install
int backup_user_data(void) {
    int res;
    
    // Create temp backup directory
    res = f_mkdir(TEMP_BACKUP_PATH);
    if (res != FR_OK && res != FR_EXIST) {
        return res;
    }
    
    // Backup DBI if it exists
    if (path_exists("sd:/switch/DBI")) {
        res = folder_copy("sd:/switch/DBI", TEMP_BACKUP_PATH);
        if (res != FR_OK) {
            return res;
        }
    }
    
    // Backup Tinfoil if it exists
    if (path_exists("sd:/switch/tinfoil")) {
        res = folder_copy("sd:/switch/tinfoil", TEMP_BACKUP_PATH);
        if (res != FR_OK) {
            return res;
        }
    }
    
    // Backup prod.keys if it exists
    if (path_exists("sd:/switch/prod.keys")) {
        res = file_copy("sd:/switch/prod.keys", "sd:/temp_backup/prod.keys");
        if (res != FR_OK) {
            return res;
        }
    }
    
    return FR_OK;
}

// Restore user data after clean install
int restore_user_data(void) {
    int res;
    
    // Recreate switch directory (should already exist, but be safe)
    res = f_mkdir("sd:/switch");
    if (res != FR_OK && res != FR_EXIST) {
        return res;
    }
    
    // Restore DBI if backup exists
    if (path_exists("sd:/temp_backup/DBI")) {
        res = folder_copy("sd:/temp_backup/DBI", "sd:/switch");
        if (res == FR_OK) {
            // Delete old DBI .nro files
            f_unlink("sd:/switch/DBI/DBI_810_EN.nro");
            f_unlink("sd:/switch/DBI/DBI_810_DE.nro");
            f_unlink("sd:/switch/DBI/DBI_845_EN.nro");
            f_unlink("sd:/switch/DBI/DBI_845_DE.nro");
            f_unlink("sd:/switch/DBI/DBI.nro");
        }
    }
    
    // Restore Tinfoil if backup exists
    if (path_exists("sd:/temp_backup/tinfoil")) {
        res = folder_copy("sd:/temp_backup/tinfoil", "sd:/switch");
        if (res == FR_OK) {
            // Delete old tinfoil.nro
            f_unlink("sd:/switch/tinfoil/tinfoil.nro");
        }
    }
    
    // Restore prod.keys if backup exists
    if (path_exists("sd:/temp_backup/prod.keys")) {
        res = file_copy("sd:/temp_backup/prod.keys", "sd:/switch/prod.keys");
        if (res != FR_OK) {
            return res;
        }
    }
    
    return FR_OK;
}

// Clean up temporary backup directory
int cleanup_backup(void) {
    if (path_exists(TEMP_BACKUP_PATH)) {
        return folder_delete(TEMP_BACKUP_PATH);
    }
    return FR_OK;
}
