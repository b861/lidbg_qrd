void parse_cmd(char *pt)
{
    int argc = 0;
    int i = 0;
    
    char *argv[32] = {NULL};

    // »ñÈ¡ÃüÁî ´æÈëargvÊý×é
    argv[0] = pt;

    while(*pt != '\0')
    {
        while(*pt != ' ')
        {
            pt++;
            if((*pt == '\0') || (*pt == 0xa)) //½áÎ²ÊÇ0xa
                break;

        }
        *pt = '\0';
        pt++;
        argc++;
        argv[argc] = pt;
    }

    argv[argc] = NULL;

    i = 0;
    lidbg("cmd:");
    while(i < argc)
    {
        printk("%s ", argv[i]);
        i++;
    }
    printk("\n");

    // ½âÎöÃüÁî

    // µ÷ÓÃÆäËûÇý¶¯
    if(!strcmp(argv[0], "d"))
    {

        struct file *file = NULL;
        mm_segment_t old_fs;
        ssize_t result;
        ssize_t ret;
        char buf[512];
        memset(buf, 0x56, 512);
        //lidbg("d\n");

        file = filp_open(argv[1],  O_RDWR, 0);
        if(IS_ERR(file))
        {
            lidbg("open device io error");
        }

        BEGIN_KMEM;

        if (!strcmp(argv[2], "write"))
        {
            ret = file->f_op->write(file, &argv[3], argc - 3, &file->f_pos); //Î´²âÊÔ
        }
        else if(!strcmp(argv[2], "read"))
        {
            result = file->f_op->read(file, &argv[3], argc - 3, &file->f_pos);
        }
        else if(!strcmp(argv[2], "ioctl"))
        {
#if 0
            result = file->f_op->ioctl(file->f_dentry->d_inode, file,
                                       /*strtoul*/simple_strtoul(argv[3], 0, 0), /*strtoulÓÃ»§Ì¬*/simple_strtoul(argv[4], 0, 0));//cmd, arg
#endif
        }

        END_KMEM;
        filp_close(file, 0);

    }
    // µ÷ÓÃÆäËûÄ£¿éµÄº¯Êý
    else if (!strcmp(argv[0], "c"))
    {
        int new_argc;
        char **new_argv;
        new_argc = argc - 2;
        new_argv = argv + 2;

        if(argv[1] == NULL)
            return;

        if(!strcmp(argv[1], "lidbg_get"))
        {
            lidbg("lidbg_devp addr = %x\n", (u32)(struct lidbg_dev *)global_lidbg_devp);
            *(u32 *)(((struct lidbg_dev *)global_lidbg_devp)->mem) = (u32)(struct lidbg_dev *)global_lidbg_devp;

        }

        if(!strcmp(argv[1], "mem"))
        {
            lidbg_mem_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "i2c"))
        {
            mod_i2c_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "io"))
        {
            mod_io_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "ad"))
        {
            mod_ad_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "spi"))
        {
            //mod_spi_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "display"))
        {
            lidbg_display_main(new_argc, new_argv);
        }

        else if(!strcmp(argv[1], "key"))
        {
            lidbg_key_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "touch"))
        {
            lidbg_touch_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "soc"))
        {
            lidbg_soc_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "uart"))
        {
             lidbg_uart_main(new_argc, new_argv);
        }

        else if(!strcmp(argv[1], "device"))
        {
            //  lidbg_device_main(new_argc, new_argv);
        }

        else if(!strcmp(argv[1], "servicer"))
        {
            lidbg_servicer_main(new_argc, new_argv);
        }

        else if(!strcmp(argv[1], "cmm"))
        {
            mod_cmn_main(new_argc, new_argv);
        }
#if 1
        else if(!strcmp(argv[1], "video"))
        {
            if(((struct lidbg_dev *)global_lidbg_devp) != NULL)
                ((struct lidbg_dev *)global_lidbg_devp)->soc_func_tbl.pfnlidbg_video_main(new_argc, new_argv);
        }
#endif
    }


}
