struct cmd_item
{
    char *cmd;
    void (*func)(int argc, char **argv);
};

struct cmd_item lidbg_cmd_item[] =
{
    {"wakelock", lidbg_wakelock_stat},
    {"mem", lidbg_mem_main},
    {"i2c", mod_i2c_main},
    {"io", mod_io_main},
    //{"spi",mod_spi_main},
    {"display", lidbg_display_main},
    {"key", lidbg_key_main},
    {"touch", lidbg_touch_main},
    {"soc", lidbg_soc_main},
    {"uart", lidbg_uart_main},
    {"servicer", lidbg_servicer_main},
    {"cmm", mod_cmn_main},
    {"file", lidbg_fileserver_main},
    {"lidbg_trace_msg", trace_msg_main},
    {"mem_log", mem_log_main},
};

void parse_cmd(char *pt)
{
    int argc = 0;
    int i = 0;

    char *argv[32] = {NULL};
#if 0
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
#else
	argc = lidbg_token_string(pt, " ", argv);
#endif

    i = 0;
    if(debug_mask)
    {
        lidbg("cmd:");
        while(i < argc)
        {
            printk("%s ", argv[i]);
            i++;
        }
        lidbg("\n");
    }
    // ½âÎöÃüÁî

    // µ÷ÓÃÆäËûÄ£¿éµÄº¯Êý
    if (!strcmp(argv[0], "c"))
    {
        int new_argc, i;
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

        for(i = 0; i < SIZE_OF_ARRAY(lidbg_cmd_item); i++)
        {
            if (!strcmp(argv[1], lidbg_cmd_item[i].cmd))
            {
                lidbg_cmd_item[i].func(new_argc, new_argv);
                break;
            }
        }
    }
}
