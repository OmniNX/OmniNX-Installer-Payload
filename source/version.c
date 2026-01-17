/*
 * OmniNX Installer - Version and Variant Detection
 */

#include "version.h"
#include <libs/fatfs/ff.h>
#include <string.h>
#include <utils/sprintf.h>
#include <utils/ini.h>
#include <utils/list.h>
#include <mem/heap.h>

// Manifest file path
#define MANIFEST_PATH "sd:/config/omninx/manifest.ini"

// Staging directories
#define STAGING_STANDARD  "sd:/OmniNX Standard"
#define STAGING_LIGHT     "sd:/OmniNX Light"
#define STAGING_OC        "sd:/OmniNX OC"

// Check if file exists
static bool file_exists(const char *path) {
    FILINFO fno;
    return (f_stat(path, &fno) == FR_OK);
}

// Read manifest.ini and extract pack variant
static omninx_variant_t read_manifest_variant(const char *manifest_path) {
    link_t sections;
    list_init(&sections);
    
    if (ini_parse(&sections, (char *)manifest_path, false) != 1) {
        // Clean up on failure
        LIST_FOREACH_ENTRY(ini_sec_t, sec, &sections, link) {
            LIST_FOREACH_ENTRY(ini_kv_t, kv, &sec->kvs, link) {
                if (kv->key) free(kv->key);
                if (kv->val) free(kv->val);
            }
            if (sec->name) free(sec->name);
        }
        return VARIANT_NONE;
    }
    
    omninx_variant_t variant = VARIANT_NONE;
    
    // Find [OmniNX] section
    LIST_FOREACH_ENTRY(ini_sec_t, sec, &sections, link) {
        if (sec->type == INI_CHOICE && sec->name && !strcmp(sec->name, "OmniNX")) {
            // Look for "current_pack" key
            LIST_FOREACH_ENTRY(ini_kv_t, kv, &sec->kvs, link) {
                if (kv->key && !strcmp(kv->key, "current_pack")) {
                    if (kv->val) {
                        if (!strcmp(kv->val, "standard")) {
                            variant = VARIANT_STANDARD;
                        } else if (!strcmp(kv->val, "light")) {
                            variant = VARIANT_LIGHT;
                        } else if (!strcmp(kv->val, "oc")) {
                            variant = VARIANT_OC;
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
    
    // Clean up
    LIST_FOREACH_ENTRY(ini_sec_t, sec, &sections, link) {
        LIST_FOREACH_ENTRY(ini_kv_t, kv, &sec->kvs, link) {
            if (kv->key) free(kv->key);
            if (kv->val) free(kv->val);
        }
        if (sec->name) free(sec->name);
    }
    
    return variant;
}

// Read manifest.ini and extract version string
static void read_manifest_version(const char *manifest_path, char *version_buf, size_t buf_size) {
    version_buf[0] = '\0';
    
    link_t sections;
    list_init(&sections);
    
    if (ini_parse(&sections, (char *)manifest_path, false) != 1) {
        // Clean up on failure
        LIST_FOREACH_ENTRY(ini_sec_t, sec, &sections, link) {
            LIST_FOREACH_ENTRY(ini_kv_t, kv, &sec->kvs, link) {
                if (kv->key) free(kv->key);
                if (kv->val) free(kv->val);
            }
            if (sec->name) free(sec->name);
        }
        return;
    }
    
    // Find [OmniNX] section
    LIST_FOREACH_ENTRY(ini_sec_t, sec, &sections, link) {
        if (sec->type == INI_CHOICE && sec->name && !strcmp(sec->name, "OmniNX")) {
            // Look for "version" key
            LIST_FOREACH_ENTRY(ini_kv_t, kv, &sec->kvs, link) {
                if (kv->key && !strcmp(kv->key, "version")) {
                    if (kv->val) {
                        strncpy(version_buf, kv->val, buf_size - 1);
                        version_buf[buf_size - 1] = '\0';
                    }
                    break;
                }
            }
            break;
        }
    }
    
    // Clean up
    LIST_FOREACH_ENTRY(ini_sec_t, sec, &sections, link) {
        LIST_FOREACH_ENTRY(ini_kv_t, kv, &sec->kvs, link) {
            if (kv->key) free(kv->key);
            if (kv->val) free(kv->val);
        }
        if (sec->name) free(sec->name);
    }
}

// Detect current OmniNX installation status
omninx_status_t detect_omninx_installation(void) {
    omninx_status_t status;
    status.is_installed = false;
    status.variant = VARIANT_NONE;
    status.version_file[0] = '\0';

    // Check for manifest.ini file
    if (file_exists(MANIFEST_PATH)) {
        status.variant = read_manifest_variant(MANIFEST_PATH);
        if (status.variant != VARIANT_NONE) {
            status.is_installed = true;
            read_manifest_version(MANIFEST_PATH, status.version_file, sizeof(status.version_file));
        }
    }

    return status;
}

// Detect which pack variant is present on SD card
omninx_variant_t detect_pack_variant(void) {
    // Check staging directories (in order of preference)
    if (file_exists(STAGING_STANDARD)) {
        return VARIANT_STANDARD;
    } else if (file_exists(STAGING_LIGHT)) {
        return VARIANT_LIGHT;
    } else if (file_exists(STAGING_OC)) {
        return VARIANT_OC;
    }
    
    return VARIANT_NONE;
}

// Get human-readable variant name
const char* get_variant_name(omninx_variant_t variant) {
    switch (variant) {
        case VARIANT_STANDARD: return "Standard";
        case VARIANT_LIGHT:     return "Light";
        case VARIANT_OC:        return "OC";
        default:                return "None";
    }
}

// Get staging/source directory path for variant
const char* get_staging_path(omninx_variant_t variant) {
    switch (variant) {
        case VARIANT_STANDARD: return STAGING_STANDARD;
        case VARIANT_LIGHT:     return STAGING_LIGHT;
        case VARIANT_OC:        return STAGING_OC;
        default:                return NULL;
    }
}

// Get manifest.ini path for variant (in staging directory)
const char* get_manifest_path(omninx_variant_t variant) {
    switch (variant) {
        case VARIANT_STANDARD: return "sd:/OmniNX Standard/config/omninx/manifest.ini";
        case VARIANT_LIGHT:     return "sd:/OmniNX Light/config/omninx/manifest.ini";
        case VARIANT_OC:        return "sd:/OmniNX OC/config/omninx/manifest.ini";
        default:                return NULL;
    }
}
