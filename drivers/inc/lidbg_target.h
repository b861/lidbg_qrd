#ifndef  __TARGET_DEFINE_
#define __TARGET_DEFINE_

#define GPIO_NOTHING LED_GPIO

#define g_hw g_hw_version_specific[g_var.hw_info.hw_version - 1]  
#define check_gpio(gpio) if(gpio == -1 ) break


struct ad_key_item
{
    int ad_value;
    int send_key;
};

struct ad_key_remap
{
    u32 ch;
	int offset;
	int max;
	struct ad_key_item key_item[5];

};

struct ad_key{
	int chanel;
	int ad_value;
} ;

struct thermal_ctrl
{
	int temp_low;
	int temp_high;
	u32 limit_freq;
	char* limit_freq_string;
	char* limit_gpu_freq_string;

};

struct hw_version_specific
{
//gpio
	int gpio_lcd_reset;
	int gpio_t123_reset;
	int gpio_dsi83_en;

	int gpio_usb_id;
	int gpio_usb_power;
	int gpio_usb_switch;
	
	int gpio_int_gps;

	int gpio_int_button_left1;
	int gpio_int_button_left2;
	int gpio_int_button_right1;
	int gpio_int_button_right2;
	
	int gpio_led1;
	int gpio_led2;

	int gpio_int_mcu_i2c_request;
	int gpio_mcu_wp;
	int gpio_mcu_app;
	
	int gpio_ts_int;
	int gpio_ts_rst;

	int gpio_dvd_rx;
	int gpio_dvd_tx;
	
	int gpio_bt_rx;
	int gpio_bt_tx;

//i2c
	int i2c_bus_dsi83;
	int i2c_bus_bx5b3a;
	int i2c_bus_ts;
	int i2c_bus_gps;
	int i2c_bus_saf7741;
	int i2c_bus_lpc;

//ad
	struct ad_key_remap ad_key[2];
	struct ad_key ad_key_map[32];
//temp
	bool thermal_ctrl_en;
	struct thermal_ctrl cpu_freq_thermal[10];
			//cat sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies
	char * cpu_freq_list;
	char * cpu_freq_temp_node;
	char * gpu_max_freq_node;
	char * cpu_freq_recovery_limit;
	int sensor_num;
	int fan_onoff_temp;
//parameter
	char * fly_parameter_node;

//system_switch
	int system_switch_en;

};
extern struct hw_version_specific g_hw_version_specific[];

#define FLYPARAMETER_NODE g_hw.fly_parameter_node
#define SYSTEM_SWITCH_EN  g_hw.system_switch_en

//touch
#define TS_I2C_BUS      (g_hw.i2c_bus_ts)
#define GTP_RST_PORT    (g_hw.gpio_ts_rst)  
#define GTP_INT_PORT    (g_hw.gpio_ts_int)

//7741
#define SAF7741_I2C_BUS  (g_hw.i2c_bus_saf7741)

//lpc
#define  LPC_I2_ID        (g_hw.i2c_bus_lpc)
#define  MCU_IIC_REQ_GPIO (g_hw.gpio_int_mcu_i2c_request)

//dsi83
#define  DSI83_I2C_BUS  		(g_hw.i2c_bus_dsi83)
#define  DSI83_GPIO_EN          (g_hw.gpio_dsi83_en)

#define  BX5B3A_I2C_BUS  		(g_hw.i2c_bus_bx5b3a)
#define  BX5B3A_GPIO_EN  		DSI83_GPIO_EN

//gps
#define GPS_I2C_BUS (g_hw.i2c_bus_gps)
#define GPS_INT	    (g_hw.gpio_int_gps)

//led
#define LED_GPIO (g_hw.gpio_led1)
#define LED_ON  do{check_gpio(g_hw.gpio_led1);SOC_IO_Output(0, g_hw.gpio_led1, 0); }while(0)
#define LED_OFF  do{check_gpio(g_hw.gpio_led1);SOC_IO_Output(0, g_hw.gpio_led1, 1); }while(0)

//button
#define BUTTON_LEFT_1 (g_hw.gpio_int_button_left1)//k1
#define BUTTON_LEFT_2 (g_hw.gpio_int_button_left2)//k2
#define BUTTON_RIGHT_1 (g_hw.gpio_int_button_right1)//k3
#define BUTTON_RIGHT_2 (g_hw.gpio_int_button_right2)//k4



#include SOC_TARGET_DEFINE_PATH


#endif
