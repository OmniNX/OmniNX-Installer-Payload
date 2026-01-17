/*
 * OmniNX Installer Payload
 * Minimal payload to install OmniNX CFW Pack files outside of Horizon OS
 *
 * Based on TegraExplorer/hekate by CTCaer, naehrwert, shchmue
 * Based on HATS-Installer-Payload by sthetix
 */

#ifndef VERSION
#define VERSION "1.0.0"
#endif

#include <string.h>

#include <display/di.h>
#include "gfx.h"
#include <libs/fatfs/ff.h>
#include <mem/heap.h>
#include <mem/minerva.h>
#include <power/max77620.h>
#include <soc/bpmp.h>
#include <soc/fuse.h>
#include <soc/hw_init.h>
#include <soc/pmc.h>
#include <soc/t210.h>
#include "nx_sd.h"
#include <storage/sdmmc.h>
#include <utils/btn.h>
#include <utils/util.h>
#include <utils/sprintf.h>
#include <input/joycon.h>

#include "fs.h"
#include "version.h"
#include "install.h"

// Configuration
#define PAYLOAD_PATH      "sd:/bootloader/update.bin"

// Payload launch defines
#define RELOC_META_OFF      0x7C
#define PATCHED_RELOC_SZ    0x94
#define PATCHED_RELOC_STACK 0x40007000
#define PATCHED_RELOC_ENTRY 0x40010000
#define EXT_PAYLOAD_ADDR    0xC0000000
#define RCM_PAYLOAD_ADDR    (EXT_PAYLOAD_ADDR + ALIGN(PATCHED_RELOC_SZ, 0x10))
#define COREBOOT_END_ADDR   0xD0000000

// Hekate config structure (simplified)
typedef struct _hekate_config
{
    u32 autoboot;
    u32 autoboot_list;
    u32 bootwait;
    u32 backlight;
    u32 autohosoff;
    u32 autonogc;
    u32 updater2p;
    u32 bootprotect;
    bool t210b01;
    bool se_keygen_done;
    bool sept_run;
    bool aes_slots_new;
    bool emummc_force_disable;
    bool rcm_patched;
    u32  errors;
} hekate_config;

// Boot config structures (required by bdk)
hekate_config h_cfg;
boot_cfg_t __attribute__((section ("._boot_cfg"))) b_cfg;
volatile nyx_storage_t *nyx_str = (nyx_storage_t *)NYX_STORAGE_ADDR;

static void *coreboot_addr;
static int total_errors = 0;

// Use BDK colors (already defined in types.h)
#define COLOR_CYAN    0xFF00FFFF
#define COLOR_WHITE   0xFFFFFFFF

static void set_default_configuration(void)
{
    h_cfg.autoboot = 0;
    h_cfg.autoboot_list = 0;
    h_cfg.bootwait = 3;
    h_cfg.se_keygen_done = 0;
    h_cfg.backlight = 100;
    h_cfg.autohosoff = 0;
    h_cfg.autonogc = 1;
    h_cfg.updater2p = 0;
    h_cfg.bootprotect = 0;
    h_cfg.errors = 0;
    h_cfg.sept_run = 0;
    h_cfg.aes_slots_new = false;
    h_cfg.rcm_patched = fuse_check_patched_rcm();
    h_cfg.emummc_force_disable = false;
    h_cfg.t210b01 = hw_get_chip_id() == GP_HIDREV_MAJOR_T210B01;
}

static void set_color(u32 color) {
    gfx_con_setcol(color, gfx_con.fillbg, gfx_con.bgcol);
}

static void print_header(void) {
    gfx_clear_grey(0x1B);
    gfx_con_setpos(0, 0);
    set_color(COLOR_CYAN);
    gfx_printf("========================================\n");
    gfx_printf("  OmniNX Installer Payload v%s\n", VERSION);
    gfx_printf("========================================\n\n");
    set_color(COLOR_WHITE);
}


void reloc_patcher(u32 payload_dst, u32 payload_src, u32 payload_size) {
    memcpy((u8 *)payload_src, (u8 *)IPL_LOAD_ADDR, PATCHED_RELOC_SZ);

    volatile reloc_meta_t *relocator = (reloc_meta_t *)(payload_src + RELOC_META_OFF);

    relocator->start = payload_dst - ALIGN(PATCHED_RELOC_SZ, 0x10);
    relocator->stack = PATCHED_RELOC_STACK;
    relocator->end   = payload_dst + payload_size;
    relocator->ep    = payload_dst;

    if (payload_size == 0x7000) {
        memcpy((u8 *)(payload_src + ALIGN(PATCHED_RELOC_SZ, 0x10)), coreboot_addr, 0x7000);
    }
}

static int launch_payload(const char *path) {
    if (!path)
        return 1;

    if (sd_mount()) {
        FIL fp;
        if (f_open(&fp, path, FA_READ)) {
            gfx_printf("Payload nicht gefunden: %s\n", path);
            return 1;
        }

        void *buf;
        u32 size = f_size(&fp);

        if (size < 0x30000)
            buf = (void *)RCM_PAYLOAD_ADDR;
        else {
            coreboot_addr = (void *)(COREBOOT_END_ADDR - size);
            buf = coreboot_addr;
        }

        if (f_read(&fp, buf, size, NULL)) {
            f_close(&fp);
            return 1;
        }

        f_close(&fp);
        sd_unmount();

        if (size < 0x30000) {
            reloc_patcher(PATCHED_RELOC_ENTRY, EXT_PAYLOAD_ADDR, ALIGN(size, 0x10));
            hw_reinit_workaround(false, byte_swap_32(*(u32 *)(buf + size - sizeof(u32))));
        } else {
            reloc_patcher(PATCHED_RELOC_ENTRY, EXT_PAYLOAD_ADDR, 0x7000);
            hw_reinit_workaround(true, 0);
        }

        sdmmc_storage_init_wait_sd();

        void (*ext_payload_ptr)() = (void *)EXT_PAYLOAD_ADDR;
        (*ext_payload_ptr)();
    }

    return 1;
}

static int file_exists(const char *path) {
    FILINFO fno;
    return (f_stat(path, &fno) == FR_OK);
}

extern void pivot_stack(u32 stack_top);

void ipl_main(void) {
    // Hardware initialization
    hw_init();
    pivot_stack(IPL_STACK_TOP);
    heap_init(IPL_HEAP_START);

    // Set bootloader's default configuration
    set_default_configuration();

    // Mount SD Card
    if (!sd_mount()) {
        // Can't show error without display, just reboot
        power_set_state(POWER_OFF_REBOOT);
    }

    // Initialize minerva for faster memory
    minerva_init();
    minerva_change_freq(FREQ_800);

    // Initialize display
    display_init();
    u32 *fb = display_init_framebuffer_pitch();
    gfx_init_ctxt(fb, 720, 1280, 720);
    gfx_con_init();
    display_backlight_pwm_init();
    display_backlight_brightness(100, 1000);

    // Overclock BPMP
    bpmp_clk_rate_set(BPMP_CLK_DEFAULT_BOOST);

    print_header();

    // Detect current OmniNX installation
    omninx_status_t current = detect_omninx_installation();
    
    // Detect which pack variant is on SD card
    omninx_variant_t pack_variant = detect_pack_variant();
    
    if (pack_variant == VARIANT_NONE) {
        set_color(COLOR_RED);
        gfx_printf("FEHLER: Kein OmniNX-Paket auf der SD-Karte gefunden!\n");
        gfx_printf("Erwartet wird eines der folgenden:\n");
        gfx_printf("  - sd:/OmniNX Standard/\n");
        gfx_printf("  - sd:/OmniNX Light/\n");
        gfx_printf("  - sd:/OmniNX OC/\n\n");
        set_color(COLOR_WHITE);
        
        // Initialize joycons for button input
        jc_init_hw();
        
        // Wait for button confirmation
        bool button_pressed = false;
        
        // Launch payload if available
        if (file_exists(PAYLOAD_PATH)) {
            set_color(COLOR_GREEN);
            gfx_printf("Druecke A-Taste (rechter Joy-Con) oder Power-Taste,\n");
            gfx_printf("um Hekate zu starten...\n");
            set_color(COLOR_WHITE);
        } else {
            set_color(COLOR_GREEN);
            gfx_printf("Druecke A-Taste (rechter Joy-Con) oder Power-Taste,\n");
            gfx_printf("um den Neustart zu starten...\n");
            set_color(COLOR_WHITE);
        }
        
        // First, wait for power button to be released if it's currently pressed
        while (btn_read() & BTN_POWER) {
            msleep(50);
        }
        
        while (!button_pressed) {
            // Check power button
            u8 btn_state = btn_read();
            if (btn_state & BTN_POWER) {
                button_pressed = true;
                break;
            }
            
            // Check joycon A button
            jc_gamepad_rpt_t *jc = joycon_poll();
            if (jc && jc->a) {
                button_pressed = true;
                break;
            }
            
            msleep(50); // Small delay to avoid busy-waiting
        }
        
        // Launch payload if available, otherwise reboot
        if (file_exists(PAYLOAD_PATH)) {
            gfx_printf("\n");
            set_color(COLOR_CYAN);
            gfx_printf("Payload wird gestartet...\n");
            set_color(COLOR_WHITE);
            msleep(500);
            launch_payload(PAYLOAD_PATH);
        } else {
            power_set_state(POWER_OFF_REBOOT);
        }
        return;
    }
    
    // Determine installation mode
    install_mode_t mode = current.is_installed ? INSTALL_MODE_UPDATE : INSTALL_MODE_CLEAN;
    
    // Show information
    set_color(COLOR_CYAN);
    gfx_printf("Installationsmodus: %s\n", mode == INSTALL_MODE_UPDATE ? "Update" : "Saubere Installation");
    gfx_printf("Paket-Variante: %s\n", get_variant_name(pack_variant));
    if (current.is_installed) {
        gfx_printf("Aktuelle Installation: %s\n", get_variant_name(current.variant));
    } else {
        gfx_printf("Aktuelle Installation: Keine\n");
    }
    set_color(COLOR_WHITE);
    gfx_printf("\n");
    
    // Initialize joycons for button input
    jc_init_hw();
    
    // Intro section - wait for user confirmation
    set_color(COLOR_YELLOW);
    gfx_printf("Bereit zum Installieren/Aktualisieren.\n");
    set_color(COLOR_WHITE);
    gfx_printf("\n");
    set_color(COLOR_GREEN);
    gfx_printf("Druecke A-Taste (rechter Joy-Con) oder Power-Taste,\n");
    gfx_printf("um die Installation zu starten...\n");
    set_color(COLOR_WHITE);
    
    // Wait for either A button or Power button
    bool button_pressed = false;
    
    // First, wait for power button to be released if it's currently pressed
    while (btn_read() & BTN_POWER) {
        msleep(50);
    }
    
    while (!button_pressed) {
        // Check power button - detect press (transition from not pressed to pressed)
        u8 btn_state = btn_read();
        if (btn_state & BTN_POWER) {
            button_pressed = true;
            break;
        }
        
        // Check joycon A button
        jc_gamepad_rpt_t *jc = joycon_poll();
        if (jc && jc->a) {
            button_pressed = true;
            break;
        }
        
        msleep(50); // Small delay to avoid busy-waiting
    }
    
    // Clear the prompt and start installation
    gfx_clear_grey(0x1B);
    gfx_con_setpos(0, 0);
    print_header();
    
    // Perform the installation
    set_color(COLOR_YELLOW);
    gfx_printf("Installation wird gestartet...\n\n");
    set_color(COLOR_WHITE);
    
    int result = perform_installation(pack_variant, mode);
    
    // Clear screen for final summary to ensure it's visible
    gfx_clear_grey(0x1B);
    gfx_con_setpos(0, 0);
    
    // Summary
    set_color(COLOR_CYAN);
    gfx_printf("========================================\n");
    gfx_printf("  OmniNX Installer Payload v%s\n", VERSION);
    gfx_printf("========================================\n\n");
    set_color(COLOR_WHITE);
    
    if (result == FR_OK && total_errors == 0) {
        set_color(COLOR_GREEN);
        gfx_printf("========================================\n");
        gfx_printf("    Installation abgeschlossen!\n");
        gfx_printf("========================================\n");
    } else {
        set_color(COLOR_RED);
        gfx_printf("========================================\n");
        gfx_printf("    Installation beendet\n");
        if (total_errors > 0) {
            gfx_printf("    %d Fehler\n", total_errors);
        }
        gfx_printf("========================================\n");
    }
    set_color(COLOR_WHITE);
    
    // Wait for user input before launching payload
    gfx_printf("\n");
    set_color(COLOR_GREEN);
    gfx_printf("Druecke A-Taste (rechter Joy-Con) oder Power-Taste,\n");
    gfx_printf("um Hekate zu starten...\n");
    set_color(COLOR_WHITE);
    
    // Wait for either A button or Power button (reuse button_pressed variable)
    button_pressed = false;
    
    // First, wait for power button to be released if it's currently pressed
    while (btn_read() & BTN_POWER) {
        msleep(50);
    }
    
    while (!button_pressed) {
        // Check power button - detect press (transition from not pressed to pressed)
        u8 btn_state = btn_read();
        if (btn_state & BTN_POWER) {
            // Power button is pressed
            button_pressed = true;
            break;
        }
        
        // Check joycon A button
        jc_gamepad_rpt_t *jc = joycon_poll();
        if (jc && jc->a) {
            button_pressed = true;
            break;
        }
        
        msleep(50); // Small delay to avoid busy-waiting
    }
    
    gfx_printf("\n");
    set_color(COLOR_CYAN);
    gfx_printf("Payload wird gestartet...\n");
    set_color(COLOR_WHITE);
    msleep(500); // Brief delay before launch
    
    if (file_exists(PAYLOAD_PATH)) {
        launch_payload(PAYLOAD_PATH);
    } else {
        // Payload not found, show error
        set_color(COLOR_RED);
        gfx_printf("\nFEHLER: Payload nicht gefunden!\n");
        gfx_printf("Pfad: %s\n", PAYLOAD_PATH);
        set_color(COLOR_WHITE);
        gfx_printf("\nDruecke A oder Power zum Neustart...\n");
        
        // Wait for button again
        button_pressed = false;
        
        // Wait for power button to be released if pressed
        while (btn_read() & BTN_POWER) {
            msleep(50);
        }
        
        while (!button_pressed) {
            u8 btn_state = btn_read();
            if (btn_state & BTN_POWER) {
                button_pressed = true;
                break;
            }
            
            jc_gamepad_rpt_t *jc = joycon_poll();
            if (jc && jc->a) {
                button_pressed = true;
                break;
            }
            
            msleep(50);
        }
        
        power_set_state(POWER_OFF_REBOOT);
    }
    
    // Should never reach here
    while (1)
        bpmp_halt();
}
