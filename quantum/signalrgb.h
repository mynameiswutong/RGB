#pragma once

enum signalrgb_commands
{
    GET_QMK_VERSION = 0x21,
    GET_PROTOCOL_VERSION = 0x22,
    GET_UNIQUE_IDENTIFIER = 0x23,
    STREAM_RGB_DATA = 0x24,
    SET_SIGNALRGB_MODE_ENABLE = 0x25,
    SET_SIGNALRGB_MODE_DISABLE = 0x26,
    GET_TOTAL_LEDS = 0x27,
    GET_FIRMWARE_TYPE = 0x28,
};

enum signalrgb_responses //These are a bit clunky right now. Could use improvement.
{
    PROTOCOL_VERSION_BYTE_1 = 1,
    PROTOCOL_VERSION_BYTE_2 = 0,
    PROTOCOL_VERSION_BYTE_3 = 5,
    QMK_VERSION_BYTE_1 = 0,
    QMK_VERSION_BYTE_2 = 24,
    QMK_VERSION_BYTE_3 = 7,
    DEVICE_UNIQUE_IDENTIFIER_BYTE_1 = 0,
    DEVICE_UNIQUE_IDENTIFIER_BYTE_2 = 0,
    DEVICE_UNIQUE_IDENTIFIER_BYTE_3 = 0,
    FIRMWARE_TYPE_BYTE = 2,
    DEVICE_ERROR_LED_BOUNDS = 253,  //Error code to show that there are more leds than a packet will allow.
    DEVICE_ERROR_LED_COUNT = 254
};

void get_qmk_version(void);
void get_signalrgb_protocol_version(void);
void get_unique_identifier(void);
void led_streaming(uint8_t *data);
void signalrgb_mode_enable(void);
void signalrgb_mode_disable(void);
void signalrgb_total_leds(void);
void signalrgb_firmware_type(void);

//Changelogs for Firmware Versions------------------------------------
//V1.0.1 added detection for the total number of LEDs a board has. Plugins need a rewrite to make use of this change. Rewritten plugins will not function with older firmware.
//V1.0.2 added another detection byte for which fork of SignalRGB firmware device is running. This means we can keep overlap Unique Identifiers between Sonix and Mainline.
//V1.0.3 unifies the command IDs between Mainline, Sonix, and VIA. All commands have a 0x20 offset now.
//V1.0.4 improves detection handling, and has a complete rewrite of the plugins. Also merges Mainline and VIA branches. VIA Branch is deprecated.
