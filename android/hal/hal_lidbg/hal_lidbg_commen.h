/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_LIDBG_INTERFACE_H
#define ANDROID_LIDBG_INTERFACE_H

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <hardware/hardware.h>

__BEGIN_DECLS

#define LIDBG_HARDWARE_MODULE_ID "lidbg_hal"
#define LIGHT_ID_BACKLIGHT          "backlight"


struct lidbg_state_t {

  const char *cmd;
    int cmd_state;
};

struct lidbg_device_t {
    struct hw_device_t common;

//add your methods below
    int (*send_cmd)(struct lidbg_device_t* dev, struct lidbg_state_t * state);
};


__END_DECLS

#endif  // ANDROID_LIGHTS_INTERFACE_H

