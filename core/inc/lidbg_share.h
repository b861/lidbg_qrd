
#ifndef _LIGDBG_SHARE__
#define _LIGDBG_SHARE__

#if 0
struct lidbg_fn_t_share
{
    //io
    void (*pfnmod_io_main)(int argc, char **argv);

    int (*pfnsoc_io_output)(unsigned int index, bool status);
    bool (*pfnsoc_io_input)( unsigned int index);

    int (*pfnsoc_io_irq)(struct io_int_config *pio_int_config);
    void (*pfnsoc_irq_enable)(unsigned int irq);
    void (*pfnsoc_irq_disable)(unsigned int irq);
    int (*pfnsoc_io_config)(unsigned int index, bool direction, unsigned int pull, unsigned int drive_strength, bool force_reconfig);
    //ad
    void (*pfnmod_ad_main)(int argc, char **argv);
    unsigned int   (*pfnsoc_ad_read)(unsigned int channel);

    //pwm
    unsigned int   (*pfnsoc_bl_set)(unsigned int bl_level);
    unsigned int   (*pfnsoc_pwm_set)(int pwm_id, int duty_ns, int period_ns);


    //soc init
    void (*pfnlidbg_soc_init)(void);
    void (*pfnlidbg_soc_deinit)(void);
    void (*pfnlidbg_board_init)(void);
    void (*pfnlidbg_board_deinit)(void);

    //soc main
    void (*pfnlidbg_soc_main)(int argc, char **argv);


    //key
    void (*pfnlidbg_key_report)(unsigned int key_value, unsigned int type);
    void (*pfnlidbg_key_main)(int argc, char **argv);

    //i2c
    int (*pfni2c_api_do_send)(int bus_id, char chip_addr, unsigned int  sub_addr, char *buf, unsigned int size);
    int (*pfni2c_api_do_recv)(int bus_id, char chip_addr, unsigned int  sub_addr, char *buf, unsigned int size);
    int (*pfni2c_api_do_recv_no_sub_addr)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
    int (*pfni2c_api_do_recv_sub_addr_2bytes)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
    int (*pfni2c_api_do_recv_sub_addr_3bytes)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
    void (*pfnmod_i2c_main)(int argc, char **argv);

    int (*pfni2c_api_do_recv_SAF7741)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
    int (*pfni2c_api_do_send_TEF7000)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
    int (*pfni2c_api_do_recv_TEF7000)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);

    //servicer
    void (*pfnlidbg_servicer_main)(int argc, char **argv);
    void (*pfnk2u_write)(int cmd);
    int (*pfnk2u_read)(void);
    int (*pfnu2k_read)(void);

    //cmn
    void (*pfnmod_cmn_main)(int argc, char **argv);
    int (*pfncmn_task_kill_select)(char *task_name);
    int (*pfncmn_task_kill_exclude)(char *exclude_process, unsigned int num);

    //mem
    void (*pfnlidbg_mem_main)(int argc, char **argv);
    void (*pfnwrite_phy_addr)(unsigned int PhyAddr, unsigned int WriteMemValue);
    void (*pfnwrite_phy_addr_bit)(unsigned int offset, unsigned int num, unsigned int phy_addr, unsigned int value);
    unsigned int (*pfnread_phy_addr)(unsigned int PhyAddr);
    unsigned int (*pfnread_phy_addr_bit)(unsigned int offset, unsigned int num, unsigned int phy_addr);
    unsigned int (*pfnread_virt_addr)(unsigned int phy_addr);
    void (*pfnwrite_virt_addr)(unsigned int phy_addr, unsigned int value);
    //display
    void (*pfnlidbg_display_main)(int argc, char **argv);
    int  (*pfnsoc_get_screen_res)(unsigned int *screen_x, unsigned int *screen_y);

    //touch
    void (*pfnlidbg_touch_main)(int argc, char **argv);
    void (*pfnlidbg_touch_report)(unsigned int pos_x, unsigned int pos_y, unsigned int type);

    //cmm
    int  (*pfncmn_launch_user)(char bin_path[], char argv1[]);

    //i2c-gpio
    void (*pfnsoc_i2c_gpio_config)(struct platform_device *pdev);



};


struct lidbg_share
{
    struct lidbg_fn_t_share share_func_tbl;
    unsigned char reserve1[128];
    struct lidbg_dev *lidbg_devp;
    unsigned char reserve2[128];
};


#define LIDBG_SHARE_CHECK_READY  (plidbg_share != NULL)

#define LIDBG_SHARE_DEFINE  struct lidbg_share *plidbg_share = NULL

#define LIDBG_SHARE_GET  \
 	do{\
	 mm_segment_t old_fs;\
	 struct file *fd = NULL;\
	 printk("lidbg:call LIDBG_SHARE_GET by %s\n",__FUNCTION__);\
	 while(1){\
	 	printk("lidbg_share:try open lidbg_share!\n");\
	 	fd = filp_open("/dev/lidbg_share", O_RDWR, 0);\
	 	printk("lidbg_share:get fd=%x\n",(int)fd);\
	    if((fd == NULL)||((int)fd == 0xfffffffe)){printk("lidbg_share:get fd fail!\n");msleep(500);}\
	    else break;\
	 }\
	 BEGIN_KMEM;\
	 fd->f_op->read(fd, (void*)&plidbg_share, 4 ,&fd->f_pos);\
	 END_KMEM;\
	filp_close(fd,0);\
	if(plidbg_share == NULL)\
	{\
		printk("LIDBG_SHARE_GET fail!\n");\
	}\
}while(0)





extern struct lidbg_share *plidbg_share;

static inline int share_check_pt(void)
{
    while (plidbg_share == NULL)
    {
        printk("lidbg:check_pt-plidbg_share==NULL\n");
        msleep(200);
    }
    return 0;
}

//io
#define share_mod_io_main (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnmod_io_main))
#define share_soc_io_output (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnsoc_io_output))
#define share_soc_io_input (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnsoc_io_input))

#define share_soc_io_irq (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnsoc_io_irq))
#define share_soc_irq_enable (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnsoc_irq_enable))
#define share_soc_irq_disable (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnsoc_irq_disable))
#define share_soc_io_config (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnsoc_io_config))

//i2c-gpio
#define share_soc_i2c_gpio_config (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnsoc_i2c_gpio_config))

//ad
#define share_mod_ad_main (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnmod_ad_main))
#define share_soc_ad_read (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnsoc_ad_read))


//pwm
#define share_soc_pwm_set (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnsoc_pwm_set))
#define share_soc_bl_set (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnsoc_bl_set))


//soc init
#define share_lidbg_soc_init (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnlidbg_soc_init)
#define share_lidbg_soc_deinit (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnlidbg_soc_deinit))
#define share_lidbg_board_init (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnlidbg_board_init))
#define share_lidbg_board_deinit (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnlidbg_board_deinit))

//main
#define share_lidbg_soc_main (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnlidbg_soc_main))

//key
#define share_lidbg_key_report (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnlidbg_key_report))
#define share_lidbg_key_main (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnlidbg_key_main))

//i2c
#define share_i2c_api_do_send (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfni2c_api_do_send))
#define share_i2c_api_do_recv (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfni2c_api_do_recv))
#define share_i2c_api_do_recv_no_sub_addr (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfni2c_api_do_recv_no_sub_addr))
#define share_i2c_api_do_recv_sub_addr_2bytes (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfni2c_api_do_recv_sub_addr_2bytes))
#define share_i2c_api_do_recv_sub_addr_3bytes (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfni2c_api_do_recv_sub_addr_3bytes))
#define share_mod_i2c_main (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnmod_i2c_main))

#define share_i2c_api_do_recv_SAF7741 (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfni2c_api_do_recv_SAF7741))
#define share_i2c_api_do_send_TEF7000 (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfni2c_api_do_send_TEF7000))
#define share_i2c_api_do_recv_TEF7000 (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfni2c_api_do_recv_TEF7000))


//servicer
#define share_lidbg_servicer_main (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnlidbg_servicer_main))
#define share_k2u_write (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnk2u_write))
#define share_k2u_read (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnk2u_read))
#define share_u2k_read (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnu2k_read))

//cmn
#define share_mod_cmn_main (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnmod_cmn_main))
#define share_lidbg_mem_main (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnlidbg_mem_main))
#define share_write_phy_addr (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnwrite_phy_addr))
#define share_write_phy_addr_bit (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnwrite_phy_addr_bit))
#define share_read_phy_addr (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnread_phy_addr))
#define share_read_phy_addr_bit (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnread_phy_addr_bit))
#define share_read_virt_addr (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnread_virt_addr))
#define share_write_virt_addr (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnwrite_virt_addr))
#define share_lidbg_display_main (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnlidbg_display_main))
#define share_soc_get_screen_res (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnsoc_get_screen_res))
#define share_cmn_task_kill_select (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfncmn_task_kill_select))
#define share_cmn_task_kill_exclude (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfncmn_task_kill_exclude))
#define share_cmn_launch_user (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfncmn_launch_user))



//touch
#define share_lidbg_touch_main (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnlidbg_touch_main))
#define share_lidbg_touch_report (share_check_pt()?NULL:(plidbg_share->share_func_tbl.pfnlidbg_touch_report))


#endif
#endif

