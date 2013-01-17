

#include "lidbg.h"

#ifdef _LIGDBG_SHARE__
LIDBG_SHARE_DEFINE;
#endif

struct input_dev *input = NULL;

typedef unsigned char  KEYCODE_T;

#if 1
static KEYCODE_T lidbg_keycode[] =
{

    KEY_MENU,   KEY_HOME,  KEY_BACK,
    KEY_DOWN,   KEY_UP,  KEY_RIGHT, KEY_LEFT,
    KEY_VOLUMEDOWN, KEY_VOLUMEUP, KEY_PAUSE, KEY_MUTE,
    KEY_POWER, KEY_SLEEP, KEY_WAKEUP,
    KEY_ENTER, KEY_END
};
#endif

void lidbg_key_report(u32 key_value, u32 type)
{

    lidbg("key - key_value:%d\n", key_value);


    if(type == KEY_PRESSED)
        lidbg("key - press\n");
    else if(type == KEY_RELEASED)
        lidbg("key - release\n");
    else
        lidbg("key - press&release\n");

    if((type == KEY_PRESSED) || (type == KEY_RELEASED))
    {
        input_report_key(input, key_value, type);
        input_sync(input);

    }
    else
    {
        input_report_key(input, key_value, KEY_PRESSED);
        input_sync(input);
        input_report_key(input, key_value, KEY_RELEASED);
        input_sync(input);
    }



}


static void share_set_func_tbl(void)
{
    //io
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnlidbg_key_main = lidbg_key_main;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnlidbg_key_report = lidbg_key_report;
}


int lidbg_key_init(void)
{
    //unsigned int type = button->type ?: EV_KEY;
    int error;
    int i;
    DUMP_BUILD_TIME;

#ifdef _LIGDBG_SHARE__
    LIDBG_SHARE_GET;
    share_set_func_tbl();
#endif


    input = input_allocate_device();
    if (!input)
    {

        lidbg("input_allocate_device err!\n");

        goto fail;
    }
    input->name = "lidbg_key";
    //input->phys = "lidbg_key";
    //input->dev.parent = &pdev->dev;

    input->id.bustype = BUS_HOST;
    //input->id.vendor = 0x0001;
    //input->id.product = 0x0001;
    //input->id.version = 0x0100;


    __set_bit(EV_KEY, input->evbit);

#if 0
    for (i = 0; i < ARRAY_SIZE(lidbg_keycode); i++)
    {

        //set_bit(lidbg_keycode[i] & KEY_MAX, input->keybit);
        input_set_capability(input, EV_KEY, lidbg_keycode[i] & KEY_MAX);

    }
#else
    //<linux/input.h>
    for (i = 1; i < KEY_MAX; i++)
    {

        //set_bit(i & KEY_MAX, input->keybit);
        input_set_capability(input, EV_KEY, i);

    }


#endif
    //clear_bit(KEY_RESERVED, zlgkpd->input->keybit);

    error = input_register_device(input);
    if (error)
    {
        lidbg("input_register_device err!\n");

        goto fail;
    }
    return 0;
fail:
    input_free_device(input);
    return 0;

}



void lidbg_key_deinit(void)
{

    input_unregister_device(input);
    //?input_free_device(input);
    // return 0;


}


void lidbg_key_main(int argc, char **argv)
{
    u32 key_value = 0;
    u32 type;

    if(argc < 2)
    {
        lidbg("Usage:\n");
        lidbg("key type\n");
        return;

    }
    type = simple_strtoul(argv[1], 0, 0);

    if(!strcmp(argv[0], "home"))
    {
        key_value = KEY_HOME ;
    }
    else if(!strcmp(argv[0], "back"))
    {
        key_value = KEY_BACK ;
    }

    else if(!strcmp(argv[0], "up"))
    {
        key_value = KEY_UP ;

    }
    else if(!strcmp(argv[0], "down"))
    {
        key_value = KEY_DOWN ;

    }
    else if(!strcmp(argv[0], "menu"))
    {
        key_value = KEY_MENU ;

    }
    else if(!strcmp(argv[0], "right"))
    {
        key_value = KEY_RIGHT ;

    }
    else if(!strcmp(argv[0], "left"))
    {
        key_value = KEY_LEFT ;

    }
    else if(!strcmp(argv[0], "voldown"))
    {
        key_value = KEY_VOLUMEDOWN ;

    }
    else if(!strcmp(argv[0], "volup"))
    {
        key_value = KEY_VOLUMEUP ;

    }

    else if(!strcmp(argv[0], "pause"))
    {
        key_value = KEY_PAUSE ;

    }
    else if(!strcmp(argv[0], "mute"))
    {
        key_value = KEY_MUTE ;

    }

    else if(!strcmp(argv[0], "sleep"))
    {
        key_value = KEY_SLEEP ;

    }

    else if(!strcmp(argv[0], "power"))
    {
        key_value = KEY_POWER ;

    }
    else if(!strcmp(argv[0], "enter"))
    {
        key_value = KEY_ENTER;

    }
    else if(!strcmp(argv[0], "end"))
    {
        key_value = KEY_END;

    }
    else
    {
        key_value = simple_strtoul(argv[0], 0, 0);
    }

    lidbg_key_report(key_value, type);

}



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");

#ifndef _LIGDBG_SHARE__
EXPORT_SYMBOL(lidbg_key_main);
EXPORT_SYMBOL(lidbg_key_report);
#endif
module_init(lidbg_key_init);
module_exit(lidbg_key_deinit);



