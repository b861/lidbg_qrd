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
	if(debug_mask)
	{
	    lidbg("cmd:");
	    while(i < argc)
	    {
	        printk("%s ", argv[i]);
	        i++;
	    }
	    printk("\n");
	}
    // ½âÎöÃüÁî

    // µ÷ÓÃÆäËûÄ£¿éµÄº¯Êý
    if (!strcmp(argv[0], "c"))
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
            *(u32 *)(((struct lidbg_dev *)global_lidbg_devp)->mem) = (u32)NULL;//(u32)(struct lidbg_dev *)global_lidbg_devp;

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

        else if(!strcmp(argv[1], "file"))
        {
            lidbg_fileserver_main(new_argc, new_argv);
        }

		else if (!strcmp(argv[1], "wakelock"))
		{
			lidbg_wakelock_stat(new_argc, new_argv);
		}

#if 1
        else if(!strcmp(argv[1], "video"))
        {
         //   if(((struct lidbg_dev *)global_lidbg_devp) != NULL)
        //        ((struct lidbg_dev *)global_lidbg_devp)->soc_func_tbl.pfnlidbg_video_main(new_argc, new_argv);
        }
#endif
    }

	else if (!strcmp(argv[0], "appcmd"))
	{
	    if (!strcmp(argv[1], "*158#001"))
	    {
	        k2u_write(LOG_LOGCAT);
	    }
	    else if (!strcmp(argv[1], "*158#002"))
	    {
	        k2u_write(LOG_DMESG);
	    }
	    else if (!strcmp(argv[1], "*158#003"))
	    {
			lidbg_chmod("/data");
	    }
	    else if (!strcmp(argv[1], "*158#999"))
	    {
			fs_call_apk();
	    }
	}

}
