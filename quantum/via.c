/* Copyright 2019 Jason Williams (Wilba)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RAW_ENABLE
#    error "RAW_ENABLE is not enabled"
#endif

#ifndef DYNAMIC_KEYMAP_ENABLE
#    error "DYNAMIC_KEYMAP_ENABLE is not enabled"
#endif

#include "via.h"
#include "protocol/host.h"      //新增
#include "raw_hid.h"
#include "dynamic_keymap.h"
#include "eeprom.h"
#include "eeconfig.h"
#include "matrix.h"
#include "timer.h"
#include "wait.h"
#include "version.h" // for QMK_BUILDDATE used in EEPROM magic

//修改起始
#if defined(BACKLIGHT_ENABLE) && !defined(VIA_CUSTOM_LIGHTING_ENABLE)
#    define VIA_QMK_BACKLIGHT_ENABLE
#endif

#if defined(RGBLIGHT_ENABLE) && !defined(VIA_CUSTOM_LIGHTING_ENABLE)
#    define VIA_QMK_RGBLIGHT_ENABLE
#endif

#if defined(RGB_MATRIX_ENABLE) && !defined(VIA_QMK_RGBLIGHT_ENABLE) && !defined(VIA_CUSTOM_LIGHTING_ENABLE)
#    define VIA_QMK_RGB_MATRIX_ENABLE
#endif

#ifdef OPENRGB_ENABLE
#   include "openrgb.h"
#endif

#ifdef SIGNALRGB_SUPPORT_ENABLE
#   include "signalrgb.h"
#endif
// Forward declare some helpers.
#if defined(VIA_QMK_BACKLIGHT_ENABLE)
void via_qmk_backlight_set_value(uint8_t *data);
void via_qmk_backlight_get_value(uint8_t *data);
#endif

#if defined(VIA_QMK_RGBLIGHT_ENABLE)
void via_qmk_rgblight_set_value(uint8_t *data);
void via_qmk_rgblight_get_value(uint8_t *data);
#endif

#if defined(VIA_QMK_RGB_MATRIX_ENABLE)
#    include <lib/lib8tion/lib8tion.h>
void via_qmk_rgb_matrix_set_value(uint8_t *data);
void via_qmk_rgb_matrix_get_value(uint8_t *data);
void eeconfig_update_rgb_matrix(void);
#endif
//修改结束

#if defined(AUDIO_ENABLE)
#    include "audio.h"
#endif

#if defined(BACKLIGHT_ENABLE)
#    include "backlight.h"
#endif

#if defined(RGBLIGHT_ENABLE)
#    include "rgblight.h"
#endif

#if (defined(RGB_MATRIX_ENABLE) || defined(LED_MATRIX_ENABLE))
#    include <lib/lib8tion/lib8tion.h>
#endif

#if defined(RGB_MATRIX_ENABLE)
#    include "rgb_matrix.h"
#endif

#if defined(LED_MATRIX_ENABLE)
#    include "led_matrix.h"
#endif

// Can be called in an overriding via_init_kb() to test if keyboard level code usage of
// EEPROM is invalid and use/save defaults.
bool via_eeprom_is_valid(void) {
    char *  p      = QMK_BUILDDATE; // e.g. "2019-11-05-11:29:54"
    uint8_t magic0 = ((p[2] & 0x0F) << 4) | (p[3] & 0x0F);
    uint8_t magic1 = ((p[5] & 0x0F) << 4) | (p[6] & 0x0F);
    uint8_t magic2 = ((p[8] & 0x0F) << 4) | (p[9] & 0x0F);

    return (eeprom_read_byte((void *)VIA_EEPROM_MAGIC_ADDR + 0) == magic0 && eeprom_read_byte((void *)VIA_EEPROM_MAGIC_ADDR + 1) == magic1 && eeprom_read_byte((void *)VIA_EEPROM_MAGIC_ADDR + 2) == magic2);
}

// Sets VIA/keyboard level usage of EEPROM to valid/invalid
// Keyboard level code (eg. via_init_kb()) should not call this
void via_eeprom_set_valid(bool valid) {
    char *  p      = QMK_BUILDDATE; // e.g. "2019-11-05-11:29:54"
    uint8_t magic0 = ((p[2] & 0x0F) << 4) | (p[3] & 0x0F);
    uint8_t magic1 = ((p[5] & 0x0F) << 4) | (p[6] & 0x0F);
    uint8_t magic2 = ((p[8] & 0x0F) << 4) | (p[9] & 0x0F);

    eeprom_update_byte((void *)VIA_EEPROM_MAGIC_ADDR + 0, valid ? magic0 : 0xFF);
    eeprom_update_byte((void *)VIA_EEPROM_MAGIC_ADDR + 1, valid ? magic1 : 0xFF);
    eeprom_update_byte((void *)VIA_EEPROM_MAGIC_ADDR + 2, valid ? magic2 : 0xFF);
}

// Override this at the keyboard code level to check
// VIA's EEPROM valid state and reset to defaults as needed.
// Used by keyboards that store their own state in EEPROM,
// for backlight, rotary encoders, etc.
// The override should not set via_eeprom_set_valid(true) as
// the caller also needs to check the valid state.
__attribute__((weak)) void via_init_kb(void) {}

// Called by QMK core to initialize dynamic keymaps etc.
void via_init(void) {
    // Let keyboard level test EEPROM valid state,
    // but not set it valid, it is done here.
    via_init_kb();
    via_set_layout_options_kb(via_get_layout_options());

    // If the EEPROM has the magic, the data is good.
    // OK to load from EEPROM.
    if (!via_eeprom_is_valid()) {
        eeconfig_init_via();
    }
}

void eeconfig_init_via(void) {
    // set the magic number to false, in case this gets interrupted
    via_eeprom_set_valid(false);
    // This resets the layout options
    via_set_layout_options(VIA_EEPROM_LAYOUT_OPTIONS_DEFAULT);
    // This resets the keymaps in EEPROM to what is in flash.
    dynamic_keymap_reset();
    // This resets the macros in EEPROM to nothing.
    dynamic_keymap_macro_reset();
    // Save the magic number last, in case saving was interrupted
    via_eeprom_set_valid(true);
}

// This is generalized so the layout options EEPROM usage can be
// variable, between 1 and 4 bytes.
uint32_t via_get_layout_options(void) {
    uint32_t value = 0;
    // Start at the most significant byte
    void *source = (void *)(VIA_EEPROM_LAYOUT_OPTIONS_ADDR);
    for (uint8_t i = 0; i < VIA_EEPROM_LAYOUT_OPTIONS_SIZE; i++) {
        value = value << 8;
        value |= eeprom_read_byte(source);
        source++;
    }
    return value;
}

__attribute__((weak)) void via_set_layout_options_kb(uint32_t value) {}

void via_set_layout_options(uint32_t value) {
    via_set_layout_options_kb(value);
    // Start at the least significant byte
    void *target = (void *)(VIA_EEPROM_LAYOUT_OPTIONS_ADDR + VIA_EEPROM_LAYOUT_OPTIONS_SIZE - 1);
    for (uint8_t i = 0; i < VIA_EEPROM_LAYOUT_OPTIONS_SIZE; i++) {
        eeprom_update_byte(target, value & 0xFF);
        value = value >> 8;
        target--;
    }
}

#if defined(AUDIO_ENABLE)
float via_device_indication_song[][2] = SONG(STARTUP_SOUND);
#endif // AUDIO_ENABLE

// Used by VIA to tell a device to flash LEDs (or do something else) when that
// device becomes the active device being configured, on startup or switching
// between devices. This function will be called six times, at 200ms interval,
// with an incrementing value starting at zero. Since this function is called
// an even number of times, it can call a toggle function and leave things in
// the original state.
__attribute__((weak)) void via_set_device_indication(uint8_t value) {
#if defined(BACKLIGHT_ENABLE)
    backlight_toggle();
#endif // BACKLIGHT_ENABLE
#if defined(RGBLIGHT_ENABLE)
    rgblight_toggle_noeeprom();
#endif // RGBLIGHT_ENABLE
#if defined(RGB_MATRIX_ENABLE)
    rgb_matrix_toggle_noeeprom();
#endif // RGB_MATRIX_ENABLE
#if defined(LED_MATRIX_ENABLE)
    led_matrix_toggle_noeeprom();
#endif // LED_MATRIX_ENABLE
#if defined(AUDIO_ENABLE)
    if (value == 0) {
        wait_ms(10);
        PLAY_SONG(via_device_indication_song);
    }
#endif // AUDIO_ENABLE
}

// Called by QMK core to process VIA-specific keycodes.
bool process_record_via(uint16_t keycode, keyrecord_t *record) {
    // Handle macros
    if (record->event.pressed) {
        if (keycode >= QK_MACRO && keycode <= QK_MACRO_MAX) {
            uint8_t id = keycode - QK_MACRO;
            dynamic_keymap_macro_send(id);
            return false;
        }
    }

    /*//修改开始
    // TODO: ideally this would be generalized and refactored into
    // QMK core as advanced keycodes, until then, the simple case
    // can be available here to keyboards using VIA
    switch (keycode) {
        case FN_MO13:
            if (record->event.pressed) {
                layer_on(1);
                update_tri_layer(1, 2, 3);
            } else {
                layer_off(1);
                update_tri_layer(1, 2, 3);
            }
            return false;
            break;
        case FN_MO23:
            if (record->event.pressed) {
                layer_on(2);
                update_tri_layer(1, 2, 3);
            } else {
                layer_off(2);
                update_tri_layer(1, 2, 3);
            }
            return false;
            break;
    }
    //修改结束*/

    return true;
}

//修改开始
bool g_openrgb_enabled = false; //Used to switch OpenRGB on/off, default openRGB off
bool g_signalrgb_enabled = false; //Used to switch SignalRGB on/off, default SignalRGB off

void via_openrgb_disabled(void){
    #ifdef OPENRGB_ENABLE
        g_openrgb_enabled = false;
    #endif
}

void via_openrgb_enabled(void){
 #ifdef OPENRGB_ENABLE
    rgb_matrix_mode_noeeprom(RGB_MATRIX_OPENRGB_DIRECT);    //Set OPENRGB_DIRECT mode, (not written to EEPROM)
    g_signalrgb_enabled = false;
    g_openrgb_enabled = true;
 #endif
}

void via_signalrgb_disabled(void){
    #ifdef SIGNALRGB_SUPPORT_ENABLE
        g_signalrgb_enabled = false;
    #endif
}

void via_signalrgb_enabled(void){
 #ifdef SIGNALRGB_SUPPORT_ENABLE
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SIGNALRGB);     //Set SIGNALRGB mode, (not written to EEPROM)
    g_openrgb_enabled = false;
    g_signalrgb_enabled = true;
 #endif
}

// void     via_signalrgb_toggle(void){
//     if(g_signalrgb_enabled){
//         g_signalrgb_enabled = false;
//         rgb_matrix_reload_from_eeprom();
//     }else{
//         g_signalrgb_enabled = true;
//         g_openrgb_enabled = false;
// #ifdef SIGNALRGB_SUPPORT_ENABLE
//         rgb_matrix_mode_noeeprom(RGB_MATRIX_SIGNALRGB);
// #endif
//     }
// }

// ------------------- signalrgb protocol ---------------------

//------------------ signalrgb protocol ------------------


// Keyboard level code can override this to handle custom messages from VIA.
// See raw_hid_receive() implementation.
// DO NOT call raw_hid_send() in the override function.
//__attribute__((weak)) void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
__attribute__((weak)) void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id = &(data[0]);
    *command_id = id_unhandled;
}

// VIA handles received HID messages first, and will route to
// raw_hid_receive_kb() for command IDs that are not handled here.
// This gives the keyboard code level the ability to handle the command
// specifically.
//
// raw_hid_send() is called at the end, with the same buffer, which was
// possibly modified with returned values.

#ifdef OPENRGB_ENABLE
extern uint8_t is_orgb_mode;
extern void orgb_raw_hid_receive(uint8_t *data, uint8_t length);
#endif

void raw_hid_receive(uint8_t *data, uint8_t length) {

// NEW begin

#ifdef OPENRGB_ENABLE
 if (is_orgb_mode) {
        orgb_raw_hid_receive(data, length);
        return;
 }
#endif

// NEW end 

uint8_t *command_id   = &(data[0]);
uint8_t *command_data = &(data[1]);

 //openrgb HID command
#ifdef OPENRGB_ENABLE
    if (g_openrgb_enabled){
        // openRGB
        switch (*data) {
            case OPENRGB_GET_PROTOCOL_VERSION:
                openrgb_get_protocol_version();
                break;
            case OPENRGB_GET_QMK_VERSION:
                openrgb_get_qmk_version();
                break;
            case OPENRGB_GET_DEVICE_INFO:
                openrgb_get_device_info();
                break;
            case OPENRGB_GET_MODE_INFO:
                openrgb_get_mode_info();
                break;
            case OPENRGB_GET_LED_INFO:
                openrgb_get_led_info(data);
                break;
            case OPENRGB_GET_ENABLED_MODES:
                openrgb_get_enabled_modes();
                break;

            case OPENRGB_SET_MODE:
                openrgb_set_mode(data);
                break;
            case OPENRGB_DIRECT_MODE_SET_SINGLE_LED:
                openrgb_direct_mode_set_single_led(data);
                break;
            case OPENRGB_DIRECT_MODE_SET_LEDS:
                openrgb_direct_mode_set_leds(data);
                break;
        }
        return;
    }
#endif

 //signalrgb HID command
#ifdef SIGNALRGB_SUPPORT_ENABLE
    if (g_signalrgb_enabled){
        // openRGB
        switch (*command_id) {
            case id_signalrgb_qmk_version:
            {
                get_qmk_version();
                break;      //return;
            }

            case id_signalrgb_protocol_version:
            {
                get_signalrgb_protocol_version();
                break;
            }

            case id_signalrgb_unique_identifier:
            {
                get_unique_identifier();
                break;
            }

            case id_signalrgb_stream_leds: 
            {
                led_streaming(data);
                break;
            }

            case id_signalrgb_effect_enable: 
            {
                signalrgb_mode_enable();
                break;
            }

            case id_signalrgb_effect_disable: 
            {
                signalrgb_mode_disable();
                break;
            }

            case id_signalrgb_total_leds:
            {
                signalrgb_total_leds();
                break;
            }

            case id_signalrgb_firmware_type:
            {
                signalrgb_firmware_type();
                break;
            }
            
        }
        return;
    }
#endif
 //via HID command
//修改结束
    switch (*command_id) {
        case id_get_protocol_version: {
            command_data[0] = VIA_PROTOCOL_VERSION >> 8;
            command_data[1] = VIA_PROTOCOL_VERSION & 0xFF;
            break;
        }
        case id_get_keyboard_value: {
            switch (command_data[0]) {
                case id_uptime: {
                    uint32_t value  = timer_read32();
                    command_data[1] = (value >> 24) & 0xFF;
                    command_data[2] = (value >> 16) & 0xFF;
                    command_data[3] = (value >> 8) & 0xFF;
                    command_data[4] = value & 0xFF;
                    break;
                }
                case id_layout_options: {
                    uint32_t value  = via_get_layout_options();
                    command_data[1] = (value >> 24) & 0xFF;
                    command_data[2] = (value >> 16) & 0xFF;
                    command_data[3] = (value >> 8) & 0xFF;
                    command_data[4] = value & 0xFF;
                    break;
                }
                case id_switch_matrix_state: {
                    uint8_t offset = command_data[1];
                    uint8_t rows   = 28 / ((MATRIX_COLS + 7) / 8);
                    uint8_t i      = 2;
                    for (uint8_t row = 0; row < rows && row + offset < MATRIX_ROWS; row++) {
                        matrix_row_t value = matrix_get_row(row + offset);
#if (MATRIX_COLS > 24)
                        command_data[i++] = (value >> 24) & 0xFF;
#endif
#if (MATRIX_COLS > 16)
                        command_data[i++] = (value >> 16) & 0xFF;
#endif
#if (MATRIX_COLS > 8)
                        command_data[i++] = (value >> 8) & 0xFF;
#endif
                        command_data[i++] = value & 0xFF;
                    }
                    break;
                }
                /*case id_firmware_version: {   //变更
                    uint32_t value  = VIA_FIRMWARE_VERSION;
                    command_data[1] = (value >> 24) & 0xFF;
                    command_data[2] = (value >> 16) & 0xFF;
                    command_data[3] = (value >> 8) & 0xFF;
                    command_data[4] = value & 0xFF;
                    break;
                }*/
                default: {
                    //*command_id = id_unhandled;     //变更
                    via_custom_value_command_kb(data, length);   
                    break;
                }
            }
            break;
        }
        case id_set_keyboard_value: {
            switch (command_data[0]) {
                case id_layout_options: {
                    uint32_t value = ((uint32_t)command_data[1] << 24) | ((uint32_t)command_data[2] << 16) | ((uint32_t)command_data[3] << 8) | (uint32_t)command_data[4];
                    via_set_layout_options(value);
                    break;
                }
                case id_device_indication: {
                    uint8_t value = command_data[1];
                    via_set_device_indication(value);
                    break;
                }
                default: {
                    //*command_id = id_unhandled;     
                    via_custom_value_command_kb(data, length);   //变更
                    break;
                }
            }
            break;
        }
        case id_dynamic_keymap_get_keycode: {
            uint16_t keycode = dynamic_keymap_get_keycode(command_data[0], command_data[1], command_data[2]);
            command_data[3]  = keycode >> 8;
            command_data[4]  = keycode & 0xFF;
            break;
        }
        case id_dynamic_keymap_set_keycode: {
            dynamic_keymap_set_keycode(command_data[0], command_data[1], command_data[2], (command_data[3] << 8) | command_data[4]);
            break;
        }
        case id_dynamic_keymap_reset: {
            dynamic_keymap_reset();
            break;
        }
        //修改起始
        case id_lighting_set_value: {
            #if defined(VIA_QMK_BACKLIGHT_ENABLE)
                        via_qmk_backlight_set_value(command_data);
            #endif
            #if defined(VIA_QMK_RGBLIGHT_ENABLE)
                        via_qmk_rgblight_set_value(command_data);
            #endif
            #if defined(VIA_QMK_RGB_MATRIX_ENABLE)
                        via_qmk_rgb_matrix_set_value(command_data);
            #endif
            #if defined(VIA_CUSTOM_LIGHTING_ENABLE)
                        via_custom_value_command_kb(data, length);  //raw_hid_receive_kb(data, length);
            #endif
            #if !defined(VIA_QMK_BACKLIGHT_ENABLE) && !defined(VIA_QMK_RGBLIGHT_ENABLE) && !defined(VIA_CUSTOM_LIGHTING_ENABLE) && !defined(VIA_QMK_RGB_MATRIX_ENABLE)
                        // Return the unhandled state
                        *command_id = id_unhandled;
            #endif
                        break;
                    }
                    case id_lighting_get_value: {
            #if defined(VIA_QMK_BACKLIGHT_ENABLE)
                        via_qmk_backlight_get_value(command_data);
            #endif
            #if defined(VIA_QMK_RGBLIGHT_ENABLE)
                        via_qmk_rgblight_get_value(command_data);
            #endif
            #if defined(VIA_QMK_RGB_MATRIX_ENABLE)
                        via_qmk_rgb_matrix_get_value(command_data);
            #endif
            #if defined(VIA_CUSTOM_LIGHTING_ENABLE)
                        via_custom_value_command_kb(data, length);  //raw_hid_receive_kb(data, length);
            #endif
            #if !defined(VIA_QMK_BACKLIGHT_ENABLE) && !defined(VIA_QMK_RGBLIGHT_ENABLE) && !defined(VIA_CUSTOM_LIGHTING_ENABLE) && !defined(VIA_QMK_RGB_MATRIX_ENABLE)
                        // Return the unhandled state
                        *command_id = id_unhandled;
            #endif
                        break;
                    }
                    case id_lighting_save: {
            #if defined(VIA_QMK_BACKLIGHT_ENABLE)
                        eeconfig_update_backlight_current();
            #endif
            #if defined(VIA_QMK_RGBLIGHT_ENABLE)
                        eeconfig_update_rgblight_current();
            #endif
            #if defined(VIA_QMK_RGB_MATRIX_ENABLE)
                        eeconfig_update_rgb_matrix();
            #endif
            #if defined(VIA_CUSTOM_LIGHTING_ENABLE)
                        via_custom_value_command_kb(data, length);  //raw_hid_receive_kb(data, length);
            #endif
            #if !defined(VIA_QMK_BACKLIGHT_ENABLE) && !defined(VIA_QMK_RGBLIGHT_ENABLE) && !defined(VIA_CUSTOM_LIGHTING_ENABLE) && !defined(VIA_QMK_RGB_MATRIX_ENABLE)
                        // Return the unhandled state
                        *command_id = id_unhandled;
            #endif
            //修改结束
            break;
        }
#ifdef VIA_EEPROM_ALLOW_RESET
        case id_eeprom_reset: {
            via_eeprom_set_valid(false);
            eeconfig_init_via();
            break;
        }
#endif
        case id_dynamic_keymap_macro_get_count: {
            command_data[0] = dynamic_keymap_macro_get_count();
            break;
        }
        case id_dynamic_keymap_macro_get_buffer_size: {
            uint16_t size   = dynamic_keymap_macro_get_buffer_size();
            command_data[0] = size >> 8;
            command_data[1] = size & 0xFF;
            break;
        }
        case id_dynamic_keymap_macro_get_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size   = command_data[2]; // size <= 28
            dynamic_keymap_macro_get_buffer(offset, size, &command_data[3]);
            break;
        }
        case id_dynamic_keymap_macro_set_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size   = command_data[2]; // size <= 28
            dynamic_keymap_macro_set_buffer(offset, size, &command_data[3]);
            break;
        }
        case id_dynamic_keymap_macro_reset: {
            dynamic_keymap_macro_reset();
            break;
        }
        case id_dynamic_keymap_get_layer_count: {
            command_data[0] = dynamic_keymap_get_layer_count();
            break;
        }
        case id_dynamic_keymap_get_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size   = command_data[2]; // size <= 28
            dynamic_keymap_get_buffer(offset, size, &command_data[3]);
            break;
        }
        case id_dynamic_keymap_set_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size   = command_data[2]; // size <= 28
            dynamic_keymap_set_buffer(offset, size, &command_data[3]);
            break;
        }
#ifdef ENCODER_MAP_ENABLE
        case id_dynamic_keymap_get_encoder: {
            uint16_t keycode = dynamic_keymap_get_encoder(command_data[0], command_data[1], command_data[2] != 0);
            command_data[3]  = keycode >> 8;
            command_data[4]  = keycode & 0xFF;
            break;
        }
        case id_dynamic_keymap_set_encoder: {
            dynamic_keymap_set_encoder(command_data[0], command_data[1], command_data[2] != 0, (command_data[3] << 8) | command_data[4]);
            break;
        }
#endif
        default: {
            // The command ID is not known
            // Return the unhandled state
            *command_id = id_unhandled;
            break;
        }
    }

    // Return the same buffer, optionally with values changed
    // (i.e. returning state to the host, or the unhandled state).
    raw_hid_send(data, length);
}

#if defined(BACKLIGHT_ENABLE)

/*变更点
void via_qmk_backlight_command(uint8_t *data, uint8_t length) {
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id        = &(data[0]);
    uint8_t *value_id_and_data = &(data[2]);

    switch (*command_id) {
        case id_custom_set_value: {
            via_qmk_backlight_set_value(value_id_and_data);
            break;
        }
        case id_custom_get_value: {
            via_qmk_backlight_get_value(value_id_and_data);
            break;
        }
        case id_custom_save: {
            via_qmk_backlight_save();
            break;
        }
        default: {
            *command_id = id_unhandled;
            break;
        }
    }
}*/

#    if BACKLIGHT_LEVELS == 0
#        error BACKLIGHT_LEVELS == 0
#    endif

void via_qmk_backlight_get_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);
    switch (*value_id) {
        case id_qmk_backlight_brightness: {
            // level / BACKLIGHT_LEVELS * 255
            value_data[0] = ((uint16_t)get_backlight_level() * UINT8_MAX) / BACKLIGHT_LEVELS;
            break;
        }
        case id_qmk_backlight_effect: {
#    ifdef BACKLIGHT_BREATHING
            value_data[0] = is_backlight_breathing() ? 1 : 0;
#    else
            value_data[0] = 0;
#    endif
            break;
        }
    }
}

void via_qmk_backlight_set_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);
    switch (*value_id) {
        case id_qmk_backlight_brightness: {
            // level / 255 * BACKLIGHT_LEVELS
            backlight_level_noeeprom(((uint16_t)value_data[0] * BACKLIGHT_LEVELS) / UINT8_MAX);
            break;
        }
        case id_qmk_backlight_effect: {
#    ifdef BACKLIGHT_BREATHING
            if (value_data[0] == 0) {
                backlight_disable_breathing();
            } else {
                backlight_enable_breathing();
            }
#    endif
            break;
        }
    }
}

void via_qmk_backlight_save(void) {
    eeconfig_update_backlight_current();
}

#endif // BACKLIGHT_ENABLE

#if defined(VIA_QMK_RGBLIGHT_ENABLE)    //#if defined(RGBLIGHT_ENABLE)
    #    ifndef RGBLIGHT_LIMIT_VAL
    #        define RGBLIGHT_LIMIT_VAL 255
    #    endif

    
    void via_qmk_rgblight_command(uint8_t *data, uint8_t length) {
        // data = [ command_id, channel_id, value_id, value_data ]
        uint8_t *command_id        = &(data[0]);
        uint8_t *value_id_and_data = &(data[2]);

        switch (*command_id) {
            case id_custom_set_value: {
                via_qmk_rgblight_set_value(value_id_and_data);
                break;
            }
            case id_custom_get_value: {
                via_qmk_rgblight_get_value(value_id_and_data);
                break;
            }
            case id_custom_save: {
                via_qmk_rgblight_save();
                break;
            }
            default: {
                *command_id = id_unhandled;
                break;
            }
        }
    }

    void via_qmk_rgblight_get_value(uint8_t *data) {
        // data = [ value_id, value_data ]
        uint8_t *value_id   = &(data[0]);
        uint8_t *value_data = &(data[1]);
        switch (*value_id) {
            case id_qmk_rgblight_brightness: {
                value_data[0] = ((uint16_t)rgblight_get_val() * UINT8_MAX) / RGBLIGHT_LIMIT_VAL;
                break;
            }
            case id_qmk_rgblight_effect: {
                //value_data[0] = rgblight_is_enabled() ? rgblight_get_mode() : 0;
                value_data[0] = rgblight_get_mode();
                break;
            }
            case id_qmk_rgblight_effect_speed: {
                value_data[0] = rgblight_get_speed();
                break;
            }
            case id_qmk_rgblight_color: {
                value_data[0] = rgblight_get_hue();
                value_data[1] = rgblight_get_sat();
                break;
            }
        }
    }

 void via_qmk_rgblight_set_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);
    switch (*value_id) {
        case id_qmk_rgblight_brightness: {
            rgblight_sethsv_noeeprom(rgblight_get_hue(), rgblight_get_sat(), ((uint16_t)value_data[0] * RGBLIGHT_LIMIT_VAL) / UINT8_MAX);
            break;
        }
        case id_qmk_rgblight_effect: {
            rgblight_mode_noeeprom(value_data[0]);/*变更点*/
            if (value_data[0] == 0) {
                rgblight_disable_noeeprom();
            } else {
                rgblight_enable_noeeprom();
                //rgblight_mode_noeeprom(value_data[0]);
            }
            break;
        }
        case id_qmk_rgblight_effect_speed: {
            rgblight_set_speed_noeeprom(value_data[0]);
            break;
        }
        case id_qmk_rgblight_color: {
            rgblight_sethsv_noeeprom(value_data[0], value_data[1], rgblight_get_val());
            break;
        }
    }
 }

    void via_qmk_rgblight_save(void) {
        eeconfig_update_rgblight_current();
    }

#endif // QMK_RGBLIGHT_ENABLE

/*变更点*/
#if defined(VIA_QMK_RGB_MATRIX_ENABLE)  //#if defined(RGB_MATRIX_ENABLE)

void via_qmk_rgb_matrix_command(uint8_t *data, uint8_t length) {
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id        = &(data[0]);
    uint8_t *value_id_and_data = &(data[2]);

    switch (*command_id) {
        case id_custom_set_value: {
            via_qmk_rgb_matrix_set_value(value_id_and_data);
            break;
        }
        case id_custom_get_value: {
            via_qmk_rgb_matrix_get_value(value_id_and_data);
            break;
        }
        case id_custom_save: {
            via_qmk_rgb_matrix_save();
            break;
        }
        default: {
            *command_id = id_unhandled;
            break;
        }
    }
}
/*//修改开始
#    if !defined(RGB_MATRIX_MAXIMUM_BRIGHTNESS) || RGB_MATRIX_MAXIMUM_BRIGHTNESS > UINT8_MAX
#        undef RGB_MATRIX_MAXIMUM_BRIGHTNESS
#        define RGB_MATRIX_MAXIMUM_BRIGHTNESS UINT8_MAX
#    endif

// VIA supports only 4 discrete values for effect speed; map these to some
// useful speed values for RGB Matrix.
enum speed_values {
    RGBLIGHT_SPEED_0 = UINT8_MAX / 16, // not 0 to avoid really slow effects
    RGBLIGHT_SPEED_1 = UINT8_MAX / 4,
    RGBLIGHT_SPEED_2 = UINT8_MAX / 2,     // matches the default value
    RGBLIGHT_SPEED_3 = UINT8_MAX / 4 * 3, // UINT8_MAX is really fast
};

static uint8_t speed_from_rgblight(uint8_t rgblight_speed) {
    switch (rgblight_speed) {
        case 0:
            return RGBLIGHT_SPEED_0;
        case 1:
            return RGBLIGHT_SPEED_1;
        case 2:
        default:
            return RGBLIGHT_SPEED_2;
        case 3:
            return RGBLIGHT_SPEED_3;
    }
}

static uint8_t speed_to_rgblight(uint8_t rgb_matrix_speed) {
    if (rgb_matrix_speed < ((RGBLIGHT_SPEED_0 + RGBLIGHT_SPEED_1) / 2)) {
        return 0;
    } else if (rgb_matrix_speed < ((RGBLIGHT_SPEED_1 + RGBLIGHT_SPEED_2) / 2)) {
        return 1;
    } else if (rgb_matrix_speed < ((RGBLIGHT_SPEED_2 + RGBLIGHT_SPEED_3) / 2)) {
        return 2;
    } else {
        return 3;
    }
}//修改结束*/

void via_qmk_rgb_matrix_get_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_qmk_rgblight_brightness: {    //case id_qmk_rgb_matrix_brightness: {
            value_data[0] = ((uint16_t)rgb_matrix_get_val() * UINT8_MAX) / RGB_MATRIX_MAXIMUM_BRIGHTNESS;
            break;
        }
        case id_qmk_rgblight_effect: {    //case id_qmk_rgb_matrix_effect: {
            value_data[0] = rgb_matrix_is_enabled() ? rgb_matrix_get_mode() : 0;
            break;
        }
        case id_qmk_rgblight_effect_speed: {  //case id_qmk_rgb_matrix_effect_speed: {
            value_data[0] = rgb_matrix_get_speed();
            break;
        }
        case id_qmk_rgblight_color: {     //case id_qmk_rgb_matrix_color: {
            value_data[0] = rgb_matrix_get_hue();
            value_data[1] = rgb_matrix_get_sat();
            break;
        }
    }
}

void via_qmk_rgb_matrix_set_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);
    switch (*value_id) {
        case id_qmk_rgblight_brightness: {    //case id_qmk_rgb_matrix_brightness: {
            rgb_matrix_sethsv_noeeprom(rgb_matrix_get_hue(), rgb_matrix_get_sat(), scale8(value_data[0], RGB_MATRIX_MAXIMUM_BRIGHTNESS));
            break;
        }
        case id_qmk_rgblight_effect: {    //case id_qmk_rgb_matrix_effect: {
            if (value_data[0] == 0) {
                rgb_matrix_disable_noeeprom();
            } else {
                rgb_matrix_enable_noeeprom();
                rgb_matrix_mode_noeeprom(value_data[0]);
            }
            break;
        }
        case id_qmk_rgblight_effect_speed: {  //case id_qmk_rgb_matrix_effect_speed: {
            rgb_matrix_set_speed_noeeprom(value_data[0]);
            break;
        }
        case id_qmk_rgblight_color: {     //case id_qmk_rgb_matrix_color: {
            rgb_matrix_sethsv_noeeprom(value_data[0], value_data[1], rgb_matrix_get_val());
            break;
        }
    }
}

void via_qmk_rgb_matrix_save(void) {
    eeconfig_update_rgb_matrix();
}

#endif // RGB_MATRIX_ENABLE

#if defined(LED_MATRIX_ENABLE)

void via_qmk_led_matrix_command(uint8_t *data, uint8_t length) {
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id        = &(data[0]);
    uint8_t *value_id_and_data = &(data[2]);

    switch (*command_id) {
        case id_custom_set_value: {
            via_qmk_led_matrix_set_value(value_id_and_data);
            break;
        }
        case id_custom_get_value: {
            via_qmk_led_matrix_get_value(value_id_and_data);
            break;
        }
        case id_custom_save: {
            via_qmk_led_matrix_save();
            break;
        }
        default: {
            *command_id = id_unhandled;
            break;
        }
    }
}

void via_qmk_led_matrix_get_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_qmk_led_matrix_brightness: {
            value_data[0] = ((uint16_t)led_matrix_get_val() * UINT8_MAX) / LED_MATRIX_MAXIMUM_BRIGHTNESS;
            break;
        }
        case id_qmk_led_matrix_effect: {
            value_data[0] = led_matrix_is_enabled() ? led_matrix_get_mode() : 0;
            break;
        }
        case id_qmk_led_matrix_effect_speed: {
            value_data[0] = led_matrix_get_speed();
            break;
        }
    }
}

void via_qmk_led_matrix_set_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);
    switch (*value_id) {
        case id_qmk_led_matrix_brightness: {
            led_matrix_set_val_noeeprom(scale8(value_data[0], LED_MATRIX_MAXIMUM_BRIGHTNESS));
            break;
        }
        case id_qmk_led_matrix_effect: {
            if (value_data[0] == 0) {
                led_matrix_disable_noeeprom();
            } else {
                led_matrix_enable_noeeprom();
                led_matrix_mode_noeeprom(value_data[0]);
            }
            break;
        }
        case id_qmk_led_matrix_effect_speed: {
            led_matrix_set_speed_noeeprom(value_data[0]);
            break;
        }
    }
}

void via_qmk_led_matrix_save(void) {
    eeconfig_update_led_matrix();
}

#endif // LED_MATRIX_ENABLE

#if defined(AUDIO_ENABLE)

extern audio_config_t audio_config;

void via_qmk_audio_command(uint8_t *data, uint8_t length) {
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id        = &(data[0]);
    uint8_t *value_id_and_data = &(data[2]);

    switch (*command_id) {
        case id_custom_set_value: {
            via_qmk_audio_set_value(value_id_and_data);
            break;
        }
        case id_custom_get_value: {
            via_qmk_audio_get_value(value_id_and_data);
            break;
        }
        case id_custom_save: {
            via_qmk_audio_save();
            break;
        }
        default: {
            *command_id = id_unhandled;
            break;
        }
    }
}

void via_qmk_audio_get_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);
    switch (*value_id) {
        case id_qmk_audio_enable: {
            value_data[0] = audio_config.enable ? 1 : 0;
            break;
        }
        case id_qmk_audio_clicky_enable: {
            value_data[0] = audio_config.clicky_enable ? 1 : 0;
            break;
        }
    }
}

void via_qmk_audio_set_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);
    switch (*value_id) {
        case id_qmk_audio_enable: {
            audio_config.enable = value_data[0] ? 1 : 0;
            break;
        }
        case id_qmk_audio_clicky_enable: {
            audio_config.clicky_enable = value_data[0] ? 1 : 0;
            break;
        }
    }
}

void via_qmk_audio_save(void) {
    eeconfig_update_audio(audio_config.raw);
}

#endif // QMK_AUDIO_ENABLE