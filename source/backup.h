/*
 * OmniNX Installer - Backup and Restore Operations
 * For preserving user data during clean installs
 */

#pragma once
#include <utils/types.h>

#define TEMP_BACKUP_PATH "sd:/temp_backup"

// Backup user data (DBI, Tinfoil, prod.keys) before clean install
int backup_user_data(void);

// Restore user data after clean install
int restore_user_data(void);

// Clean up temporary backup directory
int cleanup_backup(void);
