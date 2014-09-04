
#ifndef __FLY_LPC_
#define __FLY_LPC_


#define LPC_CMD_LCD_ON  do{    \
		u8 buff[] = {0x02, 0x0d, 0x1};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)
#define LPC_CMD_LCD_OFF   do{   \
		u8 buff[] = {0x02, 0x0d, 0x0};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)

#define LPC_CMD_USB5V_ON   do{   \
		u8 buff[] = {0x02, 0x14, 0x1};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
        lidbg("LPC_CMD_USB5V_ON\n");\
				}while(0)
#define LPC_CMD_USB5V_OFF   do{   \
		u8 buff[] = {0x02, 0x14, 0x0};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
        lidbg("LPC_CMD_USB5V_OFF\n");\
				}while(0)

#define LPC_CMD_NO_RESET   do{   \
		u8 buff[] = {0x00,0x02,0x01};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)

#define LPC_CMD_FAN_ON  do{    \
		u8 buff[] = {0x02, 0x01, 0x1};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
        lidbg("LPC_CMD_FAN_ON\n");\
				}while(0)
#define LPC_CMD_FAN_OFF  do{    \
		u8 buff[] = {0x02, 0x01, 0x0};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
        lidbg("LPC_CMD_FAN_OFF\n");\
				}while(0)

#define LPC_CMD_PING_TEST(x)  do{    \
		u8 buff[] = {0x00, 0x44, x ,x,x,x,x};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)

#define LPC_CMD_ACC_SWITCH_START  do{    \
		u8 buff[] = {0x00, 0x07, 0x1};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
        lidbg("LPC_CMD_ACC_SWITCH_START\n");\
				}while(0)


#define LPC_PRINT(x,y,z)  do{lpc_linux_sync(x,y,z);}while(0)
void lpc_linux_sync(bool print,int mint,char *extra_info);

#endif
