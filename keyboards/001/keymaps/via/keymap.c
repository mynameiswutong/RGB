#include QMK_KEYBOARD_H

enum custom_keycodes {
    OP_MOD = SAFE_RANGE, // openRGB toggle 0x5db1
    SR_MOD, //0x5db2
};
#define DRIVER_LED_TOTAL 100
/*
#define KC_MCTL KC_MISSION_CONTROL
#define KC_LPAD KC_LAUNCHPAD

#define WIN     DF(WIN_BASE)
#define MACOS   DF(MAC_BASE)

#define KC_TASK LGUI(KC_TAB)
#define KC_FLXP LGUI(KC_E)
*/

/*enum layers{
    WIN_BASE,
    WIN_FN,
    MAC_BASE,
    MAC_FN
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[WIN_BASE] = LAYOUT_tegic_ansi( // Base 
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,   KC_F7,    KC_F8,    KC_F9,    KC_F10,    KC_F11,   KC_F12,   KC_PSCR,  KC_SLCK,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,    KC_7,     KC_8,     KC_9,     KC_0,      KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,    KC_U,     KC_I,     KC_O,     KC_P,      KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_DEL,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,    KC_J,     KC_K,     KC_L,     KC_SCLN,   KC_QUOT,  KC_ENT,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,    KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,   KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LGUI,  KC_LALT,                      KC_SPC,								 KC_RALT,  MO(WIN_FN),KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT
	),
	[WIN_FN] = LAYOUT_tegic_ansi( // FN 
	    MACOS  ,  KC_BRID,  KC_BRIU,   KC_TASK,  KC_FLXP, RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,   _______,  RGB_TOG,
	    _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,  _______,  _______,  _______,   KC_DEL,   _______,  KC_END,   _______,
	    KC_GR_MO,  _______,  _______,  RGB_HUI,  RGB_HUD,  RGB_SAI,  RGB_SAD, RGB_VAI,  RGB_VAD,  RGB_MOD,  _______,   _______,  _______,  KC_INS,   _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,  _______,  _______,  _______,   _______,  _______,
	    _______,  _______,  _______,  _______,  _______,  _______,  NK_TOGG, _______,  _______,  _______,  _______,	  _______,			  KC_VOLU,
	    _______,  _______,  _______,                      _______,                               _______,  _______,   _______,  KC_MPRV,  KC_VOLD,  KC_MNXT
	),
	[MAC_BASE] = LAYOUT_tegic_ansi(
        KC_ESC,   KC_BRID,  KC_BRIU,  KC_MCTL,  KC_LPAD,  RGB_VAD,  RGB_VAI, KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,   KC_VOLD,  KC_VOLU,  KC_PSCR,  KC_SLCK,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,    KC_7,     KC_8,     KC_9,     KC_0,      KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,    KC_U,     KC_I,     KC_O,     KC_P,      KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_DEL,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,    KC_J,     KC_K,     KC_L,     KC_SCLN,   KC_QUOT,  KC_ENT,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,    KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,   KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LGUI,  KC_LALT,                      KC_SPC,								 KC_RALT,  MO(MAC_FN),KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT
	),
	[MAC_FN] = LAYOUT_tegic_ansi(
	    WIN    , KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,  _______,  RGB_TOG,
	    _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,  _______,  _______,  _______,   KC_DEL,   _______,  KC_END,   _______,
	    KC_GR_MO,  _______,  _______,  RGB_HUI,  RGB_HUD,  RGB_SAI,  RGB_SAD, RGB_VAI,  RGB_VAD,  RGB_MOD,  _______,   _______,  _______,  KC_INS,   _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,  _______,  _______,  _______,   _______,  _______,
	    _______,  _______,  _______,  _______,  _______,  _______,  NK_TOGG, _______,  _______,  _______,  _______,	  _______,			  KC_VOLU,
	    _______,  _______,  _______,                      _______,                               _______,  _______,   _______,  KC_MPRV,  KC_VOLD,  KC_MNXT
	)
};*/

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* Base */
    LAYOUT(
    KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,     KC_F12,   KC_PSCR,  KC_DEL,   KC_HOME,  KC_END,   KC_PGUP,  KC_PGDN,
    KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,    KC_EQL,            KC_BSPC,   KC_NUM,   KC_PSLS,  KC_PAST,  KC_PMNS,
    KC_TAB, 	   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,    KC_RBRC,      KC_BSLS,   KC_P7,    KC_P8,    KC_P9,    KC_PPLS,
    KC_CAPS,          KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,                KC_ENT,   KC_P4,    KC_P5,    KC_P6,
    KC_LSFT,              KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,             KC_UP,   KC_P1,    KC_P2,    KC_P3,    KC_PENT,
    KC_LCTL,      KC_LGUI,    KC_LALT,                        KC_SPC,                                     KC_RALT,  MO(1),   KC_RCTL,  KC_LEFT,  KC_DOWN,   KC_RGHT,  KC_P0,    KC_PDOT),

    LAYOUT(
    QK_BOOT, KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,  KC_MPRV,  KC_MPLY,  KC_MNXT,  _______,  _______,  KC_CALC,  KC_BRIU,  KC_BRID,  _______,  _______,  _______,  _______,  OP_MOD ,  SR_MOD ,
    _______, RGB_M_R,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,  _______,  _______,
    _______,      _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  RGB_TOG,  _______,  RGB_HUD,  RGB_HUI,       _______,  _______,  _______,  _______,  _______,
    _______,        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  RGB_SAD,  RGB_SAI,               _______,  _______,  _______,  _______,
    _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  RGB_SPD,  RGB_SPI,           _______,  RGB_VAI,  _______,  _______,  _______,  _______,
    _______,    _______,    _______,                     _______,                                      _______,  _______,  _______,  RGB_MOD, RGB_VAD,  RGB_RMOD, _______,  _______),
};

bool rgb_matrix_indicators_kb(void) {
    if (host_keyboard_led_state().caps_lock) {  // Capslock = RED
        rgb_matrix_set_color(CAPS_LOCK_LED_INDEX, 0, 191, 255);  //caps_lock led index is #70
    //} ;
    //if (host_keyboard_led_state().num_lock) {  // Capslock = RED
        //rgb_matrix_set_color(22, 0, 191, 255);
    } 
    return false;
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    //static uint32_t key_timer;

    switch (keycode) {
        case OP_MOD:{
            if (record->event.pressed) {
                via_openrgb_enabled();
            }
            return false;
        }
        case SR_MOD:{
            if (record->event.pressed) {
            #ifdef SIGNALRGB_SUPPORT_ENABLE
                via_signalrgb_enabled();
            #endif
            }
            return false;
        }
        case RGB_MOD:{
            if (record->event.pressed) {
                via_openrgb_disbled();
                rgb_matrix_step_noeeprom();
            }
            return false;
        }
        return false;
        default:
            return true; //Process all other keycodes normally
    }
}