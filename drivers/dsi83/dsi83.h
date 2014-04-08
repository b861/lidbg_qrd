#ifndef __SN68DSI83QZER_H__
#define __SN68DSI83QZER_H__

#include <asm/uaccess.h>
#include <linux/file.h>
#include <linux/syscalls.h>
#include <asm/system.h>

static inline bool is_file_exist(char *file)
{
    struct file *filep;
    filep = filp_open(file, O_RDONLY , 0);
    if(IS_ERR(filep))
        return false;
    else
    {
        filp_close(filep, 0);
        return true;
    }
}

static inline int node_write(char *filename, char *wbuff)
{
    struct file *filep;
    mm_segment_t old_fs;
    unsigned int file_len = 1;
    printk(KERN_CRIT "%s:in.%s.%d\n", __func__,filename,is_file_exist(filename));
    filep = filp_open(filename,  O_RDWR, 0);
    if(IS_ERR(filep))
    {
        printk(KERN_CRIT "lsw.%s:filp_open\n", __func__);
        return -1;
    }

    old_fs = get_fs();
    set_fs(get_ds());

    if(wbuff)
        filep->f_op->write(filep, wbuff, strlen(wbuff), &filep->f_pos);
    set_fs(old_fs);
    filp_close(filep, 0);
    return file_len;
}

#define SN65_Sequence_seq2()	 do{node_write("/dev/sn65dsi83","seq2");}while(0)
#define SN65_Sequence_seq3()	 do{node_write("/dev/sn65dsi83","seq3");}while(0)
#define SN65_Sequence_seq4()	 do{node_write("/dev/sn65dsi83","seq4");}while(0)
#define SN65_Sequence_seq6()	 do{node_write("/dev/sn65dsi83","seq6");}while(0)
#define SN65_Sequence_seq7()	 do{node_write("/dev/sn65dsi83","seq7");}while(0)
#define SN65_Sequence_seq8()	 do{node_write("/dev/sn65dsi83","seq8");}while(0)
#define SN65_Dump()		 		 do{node_write("/dev/sn65dsi83","dump");}while(0)
#define SN65_Trace_Err()	 	 do{node_write("/dev/sn65dsi83","trace_err_status");}while(0)

#endif

