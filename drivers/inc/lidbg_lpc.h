
#ifndef __FLY_LPC_
#define __FLY_LPC_


#define LPC_CMD_LCD_ON  do{\
		u8 buff[] = {0x02, 0x0d, 0x1};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)
#define LPC_CMD_LCD_OFF   do{\
		u8 buff[] = {0x02, 0x0d, 0x0};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)

#define LPC_CMD_USB5V_ON   do{\
		u8 buff[] = {0x02, 0x14, 0x1};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
        lidbg("LPC_CMD_USB5V_ON\n");\
				}while(0)
#define LPC_CMD_USB5V_OFF   do{\
		u8 buff[] = {0x02, 0x14, 0x0};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
        lidbg("LPC_CMD_USB5V_OFF\n");\
				}while(0)

#define LPC_CMD_NO_RESET   do{\
		u8 buff[] = {0x00,0x02,0x01};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)

#define LPC_CMD_FAN_ON  do{\
		u8 buff[] = {0x02, 0x01, 0x1};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
        lidbg("LPC_CMD_FAN_ON\n");\
				}while(0)
#define LPC_CMD_FAN_OFF  do{\
		u8 buff[] = {0x02, 0x01, 0x0};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
        lidbg("LPC_CMD_FAN_OFF\n");\
				}while(0)

#define LPC_CMD_PING_TEST(x)  do{\
		u8 buff[] = {0x00, 0x44, x ,x,x,x,x};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)

#define LPC_CMD_ACC_SWITCH_START  do{\
		u8 buff[] = {0x00, 0x07, 0x1};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
        lidbg("LPC_CMD_ACC_SWITCH_START\n");\
				}while(0)

#define LPC_CMD_ACC_NO_RESET   do{\
		u8 buff[] = {0x00,0x03,0x02};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)

#define LPC_CMD_RADIORST_L  do{\
		u8 buff[] = {0x02, 0x0a, 0x00};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)

#define LPC_CMD_RADIORST_H  do{\
		u8 buff[] = {0x02, 0x0a, 0x01};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)

#define LPC_CMD_RADIO_INIT  do{\
		u8 buff[] = {0x10, 0x01, 0x01};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)

#define LPC_CMD_RADIO_INIT2  do{\
		u8 buff[] = {0x02, 0x0b, 0x01};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)
				
#define LPC_CMD_RADIO_INIT3  do{\
		u8 buff[] = {0x02, 0x0c, 0x01};\
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));\
				}while(0)
				
		//{0x0D, 0x00, 0x6F, 0x00, 0x00, 0x1D};//set channel
		//{0x0D, 0x10, 0x2D, 0x00, 0x01, 0x98};//set channel
        	//{0x0D, 0x10, 0x50, 0x08,0x00};//volume
        	//{0x0D, 0x10, 0x51, 0x02,0x63};//volume
        	//{0x0D, 0x10, 0x6D, 0x08, 0x00};//no mute
#define LPC_CMD_RADIO_SET  do{\
		u8 buff0[] = {0x10,0x10,0x0D, 0x00, 0x6F, 0x00, 0x00, 0x1D};\
		u8 buff1[] = {0x10,0x10,0x0D, 0x10, 0x2D, 0x01, 0x98};\
        	u8 buff2[] = {0x10,0x10,0x0D, 0x10, 0x50, 0x08,0x00};\
        	u8 buff3[] = {0x10,0x10,0x0D, 0x10, 0x51, 0x02,0x63};\
        	u8 buff4[] = {0x10,0x10,0x0D, 0x10, 0x6D, 0x08, 0x00};\
        SOC_LPC_Send(buff0, SIZE_OF_ARRAY(buff0));\
        SOC_LPC_Send(buff1, SIZE_OF_ARRAY(buff1));\
        SOC_LPC_Send(buff2, SIZE_OF_ARRAY(buff2));\
        SOC_LPC_Send(buff3, SIZE_OF_ARRAY(buff3));\
        SOC_LPC_Send(buff4, SIZE_OF_ARRAY(buff4));\
				}while(0)
#define LPC_PRINT(x,y,z)  do{lpc_linux_sync(x,y,z);}while(0)
void lpc_linux_sync(bool print, int mint, char *extra_info);

#endif
