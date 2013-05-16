

// #define LOG_NDEBUG 0
#define LOG_TAG "hal_futengfei"

#include <cutils/log.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include "hal_lidbg_commen.h"

static pthread_once_t g_init = PTHREAD_ONCE_INIT;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
static struct lidbg_state_t g_notification;
static struct lidbg_state_t g_battery;
static int g_havelidbgNode = 0;


char const*const OUR_LIDBG_FILE   = "/dev/mlidbg0";



void lidbg_init_globals(void)
{

     pthread_mutex_init(&g_lock, NULL);
    g_havelidbgNode = (access(OUR_LIDBG_FILE, W_OK) == 0) ? 1 : 0;
    LOGE("[futengfei]cominto===================[%s][%d]\n",__func__,g_havelidbgNode );

}

static int send_cmd_func(struct lidbg_device_t* dev, struct lidbg_state_t const* state)
{
    int err = 0;
        LOGE("[futengfei]cominto===================[%s]\n",__func__ );

    pthread_mutex_lock(&g_lock);
    pthread_mutex_unlock(&g_lock);
    return err;
}



static int close_lidbg(struct lidbg_device_t *dev)
{

    LOGE("[futengfei]cominto===================[%s]\n",__func__ );
    if (dev) {
        free(dev);
    }
    return 0;
}

static int open_lidbg(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
{
    int (*send_cmd)(struct lidbg_device_t* dev, struct lidbg_state_t * state);
    LOGE("[futengfei]cominto===================[%s]\n",__func__ );
    if (0 == strcmp(LIGHT_ID_BACKLIGHT, name)) {
        send_cmd = send_cmd_func;
    }
    else {
        return -EINVAL;
    }

    pthread_once(&g_init, lidbg_init_globals);

    struct lidbg_device_t *dev = malloc(sizeof(struct lidbg_device_t));
    memset(dev, 0, sizeof(*dev));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*)module;
    dev->common.close = (int (*)(struct hw_device_t*))close_lidbg;
    dev->send_cmd = send_cmd;

    *device = (struct hw_device_t*)dev;
    return 0;
}


static struct hw_module_methods_t lidbg_module_methods = {
    .open =  open_lidbg,
};


const struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 3,
    .version_minor = 3,
    .id = LIDBG_HARDWARE_MODULE_ID,
    .name = "lidbg hal test Module,futengfei",
    .author = "XJS, Inc.",
    .methods = &lidbg_module_methods,
};
