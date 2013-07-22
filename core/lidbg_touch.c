
#include "lidbg.h"

//#define LIDBG_MULTI_TOUCH_SUPPORT

struct input_dev *input = NULL;

#define TOUCH_X_MIN  (0)
#define TOUCH_X_MAX  (RESOLUTION_X-1)
#define TOUCH_Y_MIN  (0)
#define TOUCH_Y_MAX  (RESOLUTION_Y-1)


void lidbg_touch_report(u32 pos_x, u32 pos_y, u32 type)
{

    lidbg("touch - pos_x,pos_y:%d  %d\n", pos_x, pos_y);

    if(type == TOUCH_PRESSED)
        lidbg("touch - press\n");
    else if(type == TOUCH_RELEASED)
        lidbg("touch - release\n");
    else
        lidbg("touch - press&release\n");

    if((type == TOUCH_PRESSED) || (type == TOUCH_RELEASED))
    {

        if(type == TOUCH_PRESSED)
        {
            input_report_abs(input, ABS_X, pos_x);
            input_report_abs(input, ABS_Y, pos_y);
        }
        input_report_key(input, BTN_TOUCH, type);
        input_sync(input);

    }
    else
    {
        input_report_abs(input, ABS_X, pos_x);
        input_report_abs(input, ABS_Y, pos_y);
        input_report_key(input, BTN_TOUCH, TOUCH_PRESSED);
        input_sync(input);

        msleep(10);

        // input_report_abs(input, ABS_X, pos_x);
        //  input_report_abs(input, ABS_Y, pos_y);
        input_report_key(input, BTN_TOUCH, TOUCH_RELEASED);
        input_sync(input);
    }

}


int lidbg_touch_init(void)
{
    int error;
    u32 RESOLUTION_X=1024;
    u32 RESOLUTION_Y=600;
    DUMP_BUILD_TIME;

    input = input_allocate_device();
    if (!input)
    {
        lidbg("input_allocate_device err!\n");
        goto fail;
    }
    input->name = "lidbg-touch_n/input0";
    input->phys = "lidbg-touch_p/input0";

    input->id.bustype = BUS_HOST;
    input->id.vendor = 0x0001;
    input->id.product = 0x0011;
    input->id.version = 0x0101;

   soc_get_screen_res(&RESOLUTION_X,&RESOLUTION_Y);

#ifdef LIDBG_MULTI_TOUCH_SUPPORT
    set_bit(EV_SYN, input->evbit);
    set_bit(EV_KEY, input->evbit);
    set_bit(EV_ABS, input->evbit);
    set_bit(BTN_TOUCH, input->keybit);
    set_bit(BTN_2, input->keybit);

    input_set_abs_params(ts->input_dev, ABS_X, 0, RESOLUTION_X, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_Y, 0, RESOLUTION_Y, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_PRESSURE, 0, GOODIX_TOUCH_WEIGHT_MAX, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_TOOL_WIDTH, GOODIX_TOUCH_WEIGHT_MAX, 15, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_HAT0X, 0, RESOLUTION_X, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_HAT0Y, 0, RESOLUTION_Y, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_MT_POSITION_X, 0, RESOLUTION_X, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_MT_POSITION_Y, 0, RESOLUTION_Y, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0, 150, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_MT_WIDTH_MAJOR, 0, 150, 0, 0);

#else

    input->evbit[0] = BIT(EV_SYN) | BIT_MASK(EV_ABS) | BIT_MASK(EV_KEY);
    input->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
    input_set_abs_params(input, ABS_X, TOUCH_X_MIN, TOUCH_X_MAX, 0, 0);
    input_set_abs_params(input, ABS_Y, TOUCH_Y_MIN, TOUCH_Y_MAX, 0, 0);
    input_set_abs_params(input, ABS_PRESSURE, 0, 1, 0, 0);  //\u538b\u529b
#endif

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


void lidbg_touch_deinit(void)
{
    input_unregister_device(input);
}


void lidbg_touch_main(int argc, char **argv)
{
    u32 type, pos_x, pos_y;

    if(argc < 2)
    {
        lidbg("Usage:\n");
        lidbg("x y type\n");
        return;
    }
    pos_x = simple_strtoul(argv[0], 0, 0);
    pos_y = simple_strtoul(argv[1], 0, 0);
    type = simple_strtoul(argv[2], 0, 0);

    lidbg_touch_report(pos_x, pos_y, type);

}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");


EXPORT_SYMBOL(lidbg_touch_main);
EXPORT_SYMBOL(lidbg_touch_report);

module_init(lidbg_touch_init);
module_exit(lidbg_touch_deinit);

