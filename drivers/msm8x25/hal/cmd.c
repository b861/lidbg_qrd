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


    // µ÷ÓÃÆäËûÄ£¿éµÄº¯Êý
    if (!strcmp(argv[0], "c"))
    {
        int new_argc;
        char **new_argv;
        new_argc = argc - 2;
        new_argv = argv + 2;

        if(argv[1] == NULL)
            return;


        if(!strcmp(argv[1], "video"))
        {
            if(((struct lidbg_hal *)plidbg_dev) != NULL)
                ((struct lidbg_hal *)plidbg_dev)->soc_func_tbl.pfnlidbg_video_main(new_argc, new_argv);
        }

    }


}
