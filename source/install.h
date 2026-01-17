/*
 * OmniNX Installer - Installation Logic
 */

#pragma once
#include "version.h"
#include <utils/types.h>

// Installation modes
typedef enum {
    INSTALL_MODE_UPDATE,   // OmniNX detected - selective deletion
    INSTALL_MODE_CLEAN     // No OmniNX - full wipe
} install_mode_t;

// Main installation function
int perform_installation(omninx_variant_t pack_variant, install_mode_t mode);

// Update mode operations
int update_mode_cleanup(omninx_variant_t variant);
int update_mode_install(omninx_variant_t variant);

// Clean install operations
int clean_mode_backup(void);
int clean_mode_wipe(void);
int clean_mode_restore(void);
int clean_mode_install(omninx_variant_t variant);

// Helper: Delete a list of paths
int delete_path_list(const char* paths[], const char* description);

// Helper: Delete old version markers (except current variant)
int cleanup_old_version_markers(omninx_variant_t current_variant);

// Remove staging directory after installation
int cleanup_staging_directory(omninx_variant_t pack_variant);
