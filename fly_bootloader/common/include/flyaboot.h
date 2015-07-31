//#include "flymeg.h"

#define FLY_SCREEN_SIZE_1024 1
#define RECOVERY_MODE   0x77665502
#define FASTBOOT_MODE   0x77665500

//recovery of meg
#define FastbootModel 0xf
#define RecoveryModel 0xf0
#define FlyRecoveryModel 0xf00
#define PreRecoveryModel 0xff0
#define FlySystemModel 0x5432
//
#define 		RED_COL 		0XFF0000
#define 		WHITE_COL 		0XFFFFFF
#define 		BLUE_COL 		0X0000FF
#define 		GREEN_COL 		0X00FF00
#define 		BLACK_COL 		0X000000
#define		PINK_COL 		0XFF8080

//#define 		fontsize16		10	
#define 		fontsize		13	
int FLYKEYREBOOT;

void display_fastboot_meg();
