/*
 * OmniNX Installer - Version and Variant Detection
 */

#pragma once
#include <utils/types.h>

// OmniNX variants
typedef enum {
    VARIANT_NONE = 0,
    VARIANT_STANDARD,
    VARIANT_LIGHT,
    VARIANT_OC
} omninx_variant_t;

// Installation status
typedef struct {
    omninx_variant_t variant;
    bool is_installed;
    char version_file[32];  // e.g., "1.0.0s"
} omninx_status_t;

// Detect current OmniNX installation status
omninx_status_t detect_omninx_installation(void);

// Detect which pack variant is present on SD card
omninx_variant_t detect_pack_variant(void);

// Get human-readable variant name
const char* get_variant_name(omninx_variant_t variant);

// Get staging/source directory path for variant
const char* get_staging_path(omninx_variant_t variant);

// Get manifest.ini path for variant (in staging directory)
const char* get_manifest_path(omninx_variant_t variant);
