
#define LOG_TAG "lidbg"

#include <cutils/log.h>
#include <cutils/atomic.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include "lidbg.h"

/******************************************************************************/

#define DEVICE_NAME "/dev/mlidbg0"
#define MODULE_NAME "Lidbg Module"
#define MODULE_AUTHOR "sw.lee.g2@gmail.com"

/*设备访问接口*/  
static int lidbg_send_cmd(struct lidbg_device_t* dev, char* cmd);

/*设备打开和关闭接口*/
static int close_lidbg(struct lidbg_device_t *dev);
static int open_lidbg(const struct hw_module_t* module, char const* name, struct hw_device_t** device);

/**
 * device methods
 */

/** Close the lidbg device */
static int close_lidbg(struct lidbg_device_t *dev)
{
    if (dev) {
        free(dev);
    }
    return 0;
}


/******************************************************************************/

/**
 * module methods
 */

/** Open a new instance of a lidbg device using name */
static int open_lidbg(const struct hw_module_t* module, char const* name, struct hw_device_t** device)
{

    struct lidbg_device_t *dev = malloc(sizeof(struct lidbg_device_t));
    memset(dev, 0, sizeof(*dev));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*)module;
    dev->common.close = (int (*)(struct hw_device_t*))close_lidbg;
    dev->send_cmd = lidbg_send_cmd;

    *device = (struct hw_device_t*)dev;
    return 0;
}


static int lidbg_send_cmd(struct lidbg_device_t* dev, char* cmd) 
{  
    int fd;
    LOGI("lidbg Stub: lidbg_send_cmd %s", cmd); 
    fd = open(DEVICE_NAME, O_RDWR);
    write(fd, cmd, /*sizeof(cmd)*/32);
    close(fd);
    return 0;
}  


/*模块方法表*/ 
static struct hw_module_methods_t lidbg_module_methods = {
    .open =  open_lidbg,
};

/*
 * The lidbg Module
 */
const struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = LIDBG_HARDWARE_MODULE_ID,
    .name = MODULE_NAME,
    .author = MODULE_AUTHOR,
    .methods = &lidbg_module_methods,
};
