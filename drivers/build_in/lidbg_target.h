//#define PLATFORM_MSM8226 1
//#define PLATFORM_MSM8974 1
#define PLATFORM_MSM8909 1
#define ANDROID_5_LATER

#ifdef PLATFORM_MSM8226
#define GPIO_WP (35)
#define GPIO_MAP_OFFSET  (0)
#define LPC_I2C_BUS  (0)
#endif

#ifdef PLATFORM_MSM8974
#define GPIO_WP (79)
#define GPIO_MAP_OFFSET  (0)
#define LPC_I2C_BUS  (0)
#endif

#ifdef PLATFORM_MSM8909
#define GPIO_WP (33)
#define GPIO_MAP_OFFSET  (911)
#define LPC_I2C_BUS  (3)
#endif
