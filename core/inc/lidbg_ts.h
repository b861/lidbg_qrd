#ifndef _LIGDBG_TOUCH__
#define _LIGDBG_TOUCH__


#define TOUCH_RELEASED    0
#define TOUCH_PRESSED      1
#define TOUCH_PRESSED_RELEASED    2

void lidbg_touch_main(int argc, char **argv);
void lidbg_touch_report(u32 pos_x, u32 pos_y, u32 type);



#endif

