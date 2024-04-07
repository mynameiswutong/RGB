// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

//#include <stdint.h>

void raw_hid_receive(uint8_t *data, uint8_t length);

void raw_hid_send(uint8_t *data, uint8_t length);
