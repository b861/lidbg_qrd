
#ifndef ANDROID_LIDBG_HAL_H
#define ANDROID_LIDBG_HAL_H

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <hardware/hardware.h>

__BEGIN_DECLS
#define DEFAULT_ERR_VALUE (-999)
#define LIDBG_HARDWARE_MODULE_ID "lidbg_hal"

typedef void (* reserve)(char *reserve);
typedef pthread_t (* jni_create_thread)(const char *name, void (*start)(void *), void *arg);
typedef void (* jni_test_callback)(char *msg);
typedef void (* jni_driver_abnormal_callback)(int value);

typedef struct
{
    size_t		size;
    jni_create_thread	create_thread_cb;
    jni_test_callback	test_cb;
    jni_driver_abnormal_callback driver_abnormal_cb;
} JniCallbacks;

typedef struct
{
    size_t	size;
    int   (*set_debug_level)(int level);
    int   (*hal_init)(int id, JniCallbacks *callbacks);
    int   (*set_path)(int id, char *path);
    int   (*start_record)(int id);
    int   (*stop_record)(int id);
} HalInterface;

struct lidbg_device_t
{
    struct hw_device_t common;
    const  HalInterface *(*get_hal_interface)(struct lidbg_device_t *dev);
};

__END_DECLS
#endif

