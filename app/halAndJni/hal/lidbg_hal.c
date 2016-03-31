
#include "lidbg_servicer.h"
#include "lidbg_flycam_par.h"

// #define LOG_NDEBUG 0

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

#define OUR_LIDBG_FILE   "/dev/lidbg_flycam0"

#define DEBG_TAG "hal_futengfei."

static int send_driver_ioctl(char magic , char nr, unsigned long  arg)
{
    int flycam_fd, ret = DEFAULT_ERR_VALUE;

    flycam_fd = open(OUR_LIDBG_FILE, O_RDWR);
    if(flycam_fd < 0)
    {
        lidbg(DEBG_TAG"[%s].fail2open[%s]\n", __FUNCTION__, OUR_LIDBG_FILE);
        close(flycam_fd);
        return DEFAULT_ERR_VALUE;
    }
    if ((ret = ioctl(flycam_fd, _IO(magic, nr), arg)) < 0)
    {
        lidbg(DEBG_TAG"[%s].fail.ioctl.[%d]\n", __FUNCTION__, nr);
    }
    lidbg(DEBG_TAG"[%s].sucess2open[%s,(%c,%d)ret=%d]\n", __FUNCTION__, OUR_LIDBG_FILE, magic, nr, ret);
    close(flycam_fd);
    return ret;
}
static int cmd2kernel_func(struct lidbg_device_t *dev, struct lidbg_state_t  state)
{
    int err = 0;
    struct lidbg_device_t *mdev = dev;
    lidbg(DEBG_TAG"[%s].[%d,%s]\n", __FUNCTION__ , state.cmd_type, state.data == NULL ? "null" : state.data);

    switch (state.cmd_type)
    {
    case cmd2kernel_set_path :
        if(state.data != NULL)
            err = send_driver_ioctl(FLYCAM_FRONT_ONLINE_IOC_MAGIC, NR_PATH, (unsigned long) state.data);
        else
        {
            lidbg(DEBG_TAG"[%s].skip NR_PATH,path=null\n", __FUNCTION__ , state.cmd_type, state.data == NULL ? "null" : state.data);
            return DEFAULT_ERR_VALUE;
        }
        break;
    case cmd2kernel_start_record :
        err = send_driver_ioctl(FLYCAM_FRONT_ONLINE_IOC_MAGIC, NR_START_REC, -1);
        break;
    case cmd2kernel_stop_record :
        err = send_driver_ioctl(FLYCAM_FRONT_ONLINE_IOC_MAGIC, NR_STOP_REC, -1);
        break;
    default :
        break;
    }

    pthread_mutex_lock(&g_lock);
    pthread_mutex_unlock(&g_lock);
    return err;
}

void lidbg_init_globals(void)
{
    pthread_mutex_init(&g_lock, NULL);
    g_havelidbgNode = (access(OUR_LIDBG_FILE, W_OK) == 0) ? 1 : 0;
    lidbg(DEBG_TAG"[%s][%s,%d]\n", __FUNCTION__, OUR_LIDBG_FILE, g_havelidbgNode );
}

static int close_lidbg(struct lidbg_device_t *dev)
{
    lidbg(DEBG_TAG"[%s].\n", __FUNCTION__ );
    if (dev)
    {
        free(dev);
    }
    return 0;
}

static int lidbg_module_open(const struct hw_module_t *module, char const *name, struct hw_device_t **device)
{
    struct lidbg_device_t *dev = malloc(sizeof(struct lidbg_device_t));

    lidbg(DEBG_TAG"[%s].[%s]\n", __FUNCTION__ , name == NULL ? "null" : name);

    pthread_once(&g_init, lidbg_init_globals);

    memset(dev, 0, sizeof(*dev));
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t *)module;
    dev->common.close = (int ( *)(struct hw_device_t *))close_lidbg;
    dev->cmd2kernel = cmd2kernel_func;

    *device = (struct hw_device_t *)dev;
    return 0;
}


static struct hw_module_methods_t lidbg_module_methods =
{
    .open =  lidbg_module_open,
};

struct hw_module_t HAL_MODULE_INFO_SYM =
{
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = LIDBG_HARDWARE_MODULE_ID,
    .name = "lidbg hal futengfei 2016-03-21 14:42:31",
    .author = "XJS, Inc.",
    .methods = &lidbg_module_methods,
};
