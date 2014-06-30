#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/msm_tsens.h>
#include <linux/workqueue.h>
#include <linux/completion.h>
#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/msm_tsens.h>
#include <linux/msm_thermal.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/sysfs.h>
#include <linux/types.h>
#include <linux/android_alarm.h>
#include <linux/thermal.h>
#include <mach/rpm-regulator.h>
#include <mach/rpm-regulator-smd.h>
#include <linux/regulator/consumer.h>
#include <linux/msm_thermal_ioctl.h>

#include <linux/file.h>
#include <linux/mm.h>
#include <linux/pagemap.h>
#include <linux/backing-dev.h>
#include <linux/pagevec.h>
#include <linux/fadvise.h>
#include <linux/writeback.h>
#include <linux/usb.h>
#include <linux/random.h>
#include <linux/time.h>
#include <linux/input/mt.h>
#include <linux/rtc.h>
#include <linux/fb.h>
#include <linux/vmalloc.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/syscalls.h>
#include <asm/system.h>
#include <linux/time.h>
#include <linux/pwm.h>
#include <linux/hrtimer.h>
#include <linux/stat.h>
#include <linux/kprobes.h>
#include <asm/traps.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/semaphore.h>


#include <linux/kthread.h> //kthread_create()¡¢kthread_run()

#include <asm/uaccess.h>

#define GET_INODE_FROM_FILEP(filp) ((filp)->f_path.dentry->d_inode)





int lidbg_readwrite_file(const char *filename, char *rbuf, const char *wbuf, size_t length)
{
    int ret = 0;
    struct file *filp = (struct file *) - ENOENT;
    mm_segment_t oldfs;
    oldfs = get_fs();
    set_fs(KERNEL_DS);

    do
    {
        int mode = (wbuf) ? O_RDWR : O_RDONLY;
        filp = filp_open(filename, mode, S_IRUSR);
        if (IS_ERR(filp) || !filp->f_op)
        {
            ret = -ENOENT;
            break;
        }
        if (!filp->f_op->write || !filp->f_op->read)
        {
            filp_close(filp, NULL);
            ret = -ENOENT;
            break;
        }
        if (length == 0)
        {
            struct inode    *inode;
            inode = GET_INODE_FROM_FILEP(filp);
            if (!inode)
            {
                printk(KERN_CRIT"kernel_readwrite_file: Error 2\n");
                ret = -ENOENT;
                break;
            }
            ret = i_size_read(inode->i_mapping->host);
            break;
        }
        if (wbuf)
        {
            ret = filp->f_op->write(filp, wbuf, length, &filp->f_pos);
            if (ret < 0)
            {
                printk(KERN_CRIT"kernel_readwrite_file: Error 3\n");
                break;
            }
        }
        else
        {
            ret = filp->f_op->read(filp, rbuf, length, &filp->f_pos);
            if (ret < 0)
            {
                printk(KERN_CRIT"kernel_readwrite_file: Error 4\n");
                break;
            }
        }
    }
    while (0);

    if (!IS_ERR(filp))
        filp_close(filp, NULL);
    set_fs(oldfs);
    return ret;
}

int SOC_Get_CpuFreq(void)
{
    char buf[16];
    int cpu_freq;
    lidbg_readwrite_file("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq", buf, NULL, 16);
    cpu_freq = simple_strtoul(buf, 0, 0);
    printk(KERN_CRIT"lsw:cpufreq==========================%d\n", cpu_freq);

    return cpu_freq;
}

static int thread_freq(void *data)
{
	int count = 0;
	while(1)
	{
		msleep(1000);
		//lidbg_readwrite_file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", NULL, "787200", sizeof("787200") - 1);
		SOC_Get_CpuFreq();
		count++;
		if(count >= 45)
		{
			lidbg_readwrite_file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", NULL, "1401600", sizeof("1401600") - 1);
			printk(KERN_CRIT"lsw:stoped\n");
			return 1;
		}

	}
    return 1;
}



void freq_ctrl_start(void)
{
	struct task_struct *task;
	printk(KERN_CRIT"create kthread \n");
	task = kthread_create(thread_freq, NULL, "lidbg_thread_freq");
	if(IS_ERR(task))
	{
		printk(KERN_CRIT"Unable to start thread.\n");
	}
	else wake_up_process(task);

}


