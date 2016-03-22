
#ifndef ANDROID_LIDBG_INTERFACE_H
#define ANDROID_LIDBG_INTERFACE_H

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <hardware/hardware.h>

__BEGIN_DECLS

#define DEFAULT_ERR_VALUE (-999)

#define LIDBG_HARDWARE_MODULE_ID "lidbg_hal"

enum cmd2kernel_action
{
    cmd2kernel_set_path =	1,
    cmd2kernel_start_record =	2,
    cmd2kernel_stop_record =	4,
} ;

struct lidbg_state_t
{
    enum cmd2kernel_action cmd_type;
    char *data;
};
struct lidbg_device_t
{
    struct hw_device_t common;
    int (*cmd2kernel)(struct lidbg_device_t *dev, struct  lidbg_state_t  state);
};

__END_DECLS

#endif

