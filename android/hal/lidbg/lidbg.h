

#ifndef ANDROID_LIDBG_INTERFACE_H
#define ANDROID_LIDBG_INTERFACE_H

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <hardware/hardware.h>

__BEGIN_DECLS

/**
 * The id of this module
 */
#define LIDBG_HARDWARE_MODULE_ID "lidbg"


struct lidbg_device_t {
    struct hw_device_t common;

    int (*send_cmd)(struct lidbg_device_t* dev, char* cmd);
};


__END_DECLS

#endif  // ANDROID_LIDBG_INTERFACE_H

