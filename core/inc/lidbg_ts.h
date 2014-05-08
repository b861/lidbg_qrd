#ifndef _LIGDBG_TOUCH__
#define _LIGDBG_TOUCH__


#define TOUCH_RELEASED    0
#define TOUCH_PRESSED      1
#define TOUCH_PRESSED_RELEASED    2

typedef enum
{
	TOUCH_UP,
	TOUCH_DOWN,
	TOUCH_SYNC,
} touch_type;

struct lidbg_ts_runmode
{
	bool x2y;
	int revert;
	bool mode;
};

struct lidbg_ts_data
{
	struct input_dev *input_dev;

	u16 abs_x_max;
	u16 abs_y_max;

	u32 x[10];
	u32 y[10];
	u32 w[10];
	u32 id[10];
	
	u8 touch_num;
	u16 touch_index;

	u32 touch_cnt;

};

void lidbg_touch_main(int argc, char **argv);
void lidbg_touch_report(struct lidbg_ts_data * pdata);
void lidbg_touch_handle(touch_type t,int id, int x, int y,int w, struct lidbg_ts_runmode ts_mode);


#endif

