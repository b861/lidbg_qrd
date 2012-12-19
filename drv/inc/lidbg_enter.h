
#ifndef _LIGDBG_ENTER__
#define _LIGDBG_ENTER__

#define BEGIN_KMEM do{old_fs = get_fs();set_fs(get_ds());}while(0)
#define END_KMEM   do{set_fs(old_fs);}while(0)


struct lidbg_fn_t {
//io
	void (*SOC_IO_Output) (u32 group, u32 index, bool status);
	bool (*SOC_IO_Input) (u32 group, u32 index, u32 pull);
	
//i2c
	int (*SOC_I2C_Send) (int bus_id, char chip_addr, char *buf, unsigned int size);
	int (*SOC_I2C_Rec)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);

};

#define LIDBG_SIZE	0x00001000 //MEM_SIZE_4_KB	/*全局内存最大1K字节*/

/*lidbg设备结构体*/
struct lidbg_dev
{
    struct cdev cdev; /*cdev结构体*/
    unsigned char mem[LIDBG_SIZE]; /*全局内存*/
    struct lidbg_fn_t soc_func_tbl;
};

#if 1

#define LIDBG_DEFINE  struct lidbg_dev *lidbg_devp = NULL

#define LIDBG_GET  \
 	do{\
	 mm_segment_t old_fs;\
	 struct file *fd = NULL;\
	 fd = filp_open("/dev/mlidbg0", O_RDWR, 0);\
	 BEGIN_KMEM;\
	 fd->f_op->write(fd, "c lidbg_get", sizeof("c lidbg_get"), &fd->f_pos);\
	 fd->f_op->read(fd, &lidbg_devp, 4 ,&fd->f_pos);\
	 END_KMEM;\
	filp_close(fd,0);\
	if(lidbg_devp == NULL)\
		printk("LIDBG_GET fail!\n");\
}while(0)


#endif

#ifndef SOC_COMPILE
#define SOC_IO_Output if(lidbg_devp != NULL )(lidbg_devp->soc_func_tbl.SOC_IO_Output)
#define SOC_IO_Input  if(lidbg_devp != NULL )(lidbg_devp->soc_func_tbl.SOC_IO_Input)
#define SOC_I2C_Send  if(lidbg_devp != NULL )(lidbg_devp->soc_func_tbl.SOC_I2C_Send)
#define SOC_I2C_Rec   if(lidbg_devp != NULL )(lidbg_devp->soc_func_tbl.SOC_I2C_Rec)
#endif

#endif

