#include "video_ad.h"
#include "lidbg.h"

LIDBG_DEFINE;

#define NOTIFIER_MAJOR_VIDEO_EVENT (119)
#define NOTIFIER_MINOR_VIDEO_CVBS_OPEN (0)
#define NOTIFIER_MINOR_VIDEO_CVBS_CLOSE (1)
#define NOTIFIER_MINOR_VIDEO_YUV_OPEN (2)
#define NOTIFIER_MINOR_VIDEO_YUV_CLOSE (3)

#define VIDEO_NODE "/dev/px3_vehicle"

static int video_format = 0;

int video_set_brightness(int val){
	int ret = 0;
	unsigned char config[] = {0x0a, 0x0};

	if(val >  0)
		config[1] = val;

	ret = SOC_I2C_Send(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, config, sizeof(config) / sizeof(config[0]));

	return ret;
}

int video_set_contrast(int val){
	int ret = 0;
	unsigned char config[] = {0x08, 0x80};

	if(val >  0)
		config[1] = val;

	ret = SOC_I2C_Send(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, config, sizeof(config) / sizeof(config[0]));

	return ret;
}

int video_set_saturation(int val_u, int val_v){
	int ret = 0;
	unsigned char config_u[] = {0xe3, 0x80};
	unsigned char config_v[] = {0xe4, 0x80};

	if(val_u > 0 ){
		config_u[1] = val_u;
		ret = SOC_I2C_Send(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, config_u, sizeof(config_u) / sizeof(config_u[0]));
	}

	if(val_v > 0){
		config_v[1] = val_v;
		ret = SOC_I2C_Send(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, config_v, sizeof(config_v) / sizeof(config_v[0]));
	}

	return ret;
}

int video_set_hue(int val){
	int ret = 0;
	unsigned char config[] = {0x0b, 0x0};

	if(val >  0)
		config[1] = val;

	ret = SOC_I2C_Send(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, config, sizeof(config) / sizeof(config[0]));

	return ret;
}

int video_reg_get(int reg, int *data)
{
	int ret = 0;

	ret = SOC_I2C_Rec(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, reg, data, 1);
	if(ret < 0){
		lidbg("Error: %s failed.\n",__func__);
		return 0;
	}
	else
		return ret;
}

int video_cvbs_init(void)
{
	int i = 0;
	int ret = 0;
	unsigned char config[] = {0x0f, 0x80};

	lidbg("video cvbs init\n");
#ifdef VIDEO_CHIP_ADV7181D
	for(i=0; i<sizeof(VIDEO_CVBS)/3; i++){
		config[0] = VIDEO_CVBS[i*3 + 1];	//sub_addr
		config[1] = VIDEO_CVBS[i*3 + 2];	//reg val

		ret = SOC_I2C_Send(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, config, 2);
		if(ret < 0)
			lidbg("Video chip init error\n");
	}
#endif

	return ret;
}

int video_yuv_init(void)
{
	int i = 0;
	int ret = 0;
	unsigned char config[] = {0x0f, 0x80};

	lidbg("video yuv init\n");
#ifdef VIDEO_CHIP_ADV7181D
	for(i=0; i<sizeof(VIDEO_YUV)/3; i++){
		config[0] = VIDEO_YUV[i*3 + 1];		//sub_addr
		config[1] = VIDEO_YUV[i*3 + 2];		//reg val

		ret = SOC_I2C_Send(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, config, 2);
	}
#endif

	return ret;
}

int _video_get_format(void)
{
	int val = 0,ret = 0;

	ret = SOC_I2C_Rec(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, 0x10, &val, 1);
	if(ret < 0)
		lidbg("video get format error\n");

	if(val == VIDEO_FORMAT_NTSM_MJ)
		return VIDEO_FORMAT_NTSC;

	if(val == VIDEO_FORMAT_PAL_BGHID)
		return VIDEO_FORMAT_PAL;

	return VIDEO_FORMAT_NTSC;
}

int video_ad_get_format(void)
{
	int ret = 0;

	ret = _video_get_format();

	if(ret == VIDEO_FORMAT_NTSC)
		lidbg("<------ VIDEO_FORMAT_NTSC ------>\n");
	if(ret == VIDEO_FORMAT_PAL)
		lidbg("<------ VIDEO_FORMAT_PAL ------>\n");

	video_format = ret;

	return ret;
}

void _video_dbg(void)
{
	int i = 0;
	int data = 0;
	int ret = 0;

	lidbg("^_^ video dbg ^_^\n");
#ifdef VIDEO_CHIP_ADV7181D
	for(i=0; i<sizeof(VIDEO_CVBS)/3; i++){
		ret = SOC_I2C_Rec(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, VIDEO_CVBS[i*3 + 1], &data, 1);
		if(ret < 0)
			lidbg("Video chip READ error\n");
		else
			lidbg("reg[0x%x]: 0x%x\n", VIDEO_CVBS[i*3 + 1], data);
	}
#endif
}

int video_chip_init(int video_type)
{
	int ret = 0;

	lidbg("><><>< vehicle chip init ><><><\n");

	switch(video_type){
		case VIDEO_VEHICLE:
		case VIDEO_DVR:
		case VIDEO_AUX:
		case VIDEO_TV:
			ret = video_cvbs_init();
			break;
		case VIDEO_DVD:
			ret = video_yuv_init();
			break;
		default:
			break;
	}

	if(ret < 0)
		lidbg("Vehicle chip init error\n");

	return ret;
}

int _video_ad_set(char *config, char *par1, char *par2)
{
	int ret = -1;

	if(!strcmp(config, "brightness")){
		int brightness = simple_strtoul(par1, 0, 0);

		ret = video_set_brightness(brightness);
		if(ret)
			lidbg("Error: vehicle set brightness\n");
	}else if(!strcmp(config, "contrast")){
		int contrast = simple_strtoul(par1, 0, 0);

		ret = video_set_contrast(contrast);
		if(ret)
			lidbg("Error: vehicle set contrast\n");
	}else if(!strcmp(config, "saturation")){
		int contrast_u = simple_strtoul(par1, 0, 0);
		int contrast_v = simple_strtoul(par2, 0, 0);

		ret = video_set_saturation(contrast_u, contrast_v);
		if(ret)
			lidbg("Error: vehicle set saturation\n");
	}else if(!strcmp(config, "hue")){
		int hue = simple_strtoul(par1, 0, 0);

		ret = video_set_hue(hue);
		if(ret)
			lidbg("Error: vehicle set hue\n");
	}else
		lidbg("Error: undefined[%s] vehicle value to set\n", config);

	return ret;
}

int _video_ad_get(char *reg_add, char *reg_size)
{
	int ret = -1;
	int i = 0;
	int data = 0;

	int reg = simple_strtoul(reg_add, 0, 0);
	int size = simple_strtoul(reg_size, 0, 0);

	for(i=0; i<size; i++){
		ret = video_reg_get(reg, &data);
		if(ret < 0)
			lidbg("Vehicle ad get errot, ret=%d \n", ret);
		else
			lidbg("Vehicle reg get 0x%x = %d\n", reg, data);
	}

	return ret;
}

int _video_ad_init(char *video_type)
{
	int ret = -1;

	if(!strcmp(video_type, "vehicle"))
		ret = video_chip_init(VIDEO_VEHICLE);
	else if(!strcmp(video_type, "dvd"))
		ret = video_chip_init(VIDEO_DVD);
	else if(!strcmp(video_type, "tv"))
		ret = video_chip_init(VIDEO_TV);
	else if(!strcmp(video_type, "dvr"))
		ret = video_chip_init(VIDEO_DVR);
	else if(!strcmp(video_type, "aux"))
		ret = video_chip_init(VIDEO_AUX);
	else
		lidbg("Error: vehicle chip init unknown type\n");

	return ret;
}

int _video_ad_hw_reset(void)
{
	int ret = 0;

	lidbg("video ad hw reset\n");

	SOC_IO_Output(0, RK30_PIN3_PB5, 0);
	msleep(100);
	SOC_IO_Output(0, RK30_PIN3_PB5, 1);
	msleep(100);

	return ret;
}

int _video_ad_sft_reset(void)
{
	int ret = 0;
	unsigned char config[] = {0x0, 0x0};

	lidbg("video ad software reset\n");
#ifdef VIDEO_CHIP_ADV7181D
		config[0] = 0x80;	//sub_addr
		config[1] = 0x0f;	//reg val

		ret = SOC_I2C_Send(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, config, 2);
		if(ret < 0)
			lidbg("Video chip reset error\n");
#endif
	return ret;
}

int _video_ad_check(void)
{
	char val = 0;
	int ret = 0;

#ifdef VIDEO_CHIP_ADV7181D
		ret = SOC_I2C_Rec(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, 0xc3, &val, 1);
		if(ret < 0)
			lidbg("Video chip check error\n");
		else
			lidbg("reg[0xc3]''''''''''''''': 0x%x\n",val);
#endif
	return ret;
}

int video_ops(int video_ops, int video_input_format)
{
	switch (video_ops)
	{
	case VIDEO_OPS_OPEN:
		if(video_input_format == VIDEO_INPUT_CVBS)
			lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_VIDEO_EVENT, NOTIFIER_MINOR_VIDEO_CVBS_OPEN));
		else if(video_input_format == VIDEO_INPUT_YUV)
			lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_VIDEO_EVENT, NOTIFIER_MINOR_VIDEO_YUV_OPEN));
		else
			lidbg("Error: video ops[%d], format[%d]\n", video_ops, video_input_format);
		break;
	case VIDEO_OPS_CLOSE:
		if(video_input_format == VIDEO_INPUT_CVBS)
			lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_VIDEO_EVENT, NOTIFIER_MINOR_VIDEO_CVBS_CLOSE));
		else if(video_input_format == VIDEO_INPUT_YUV)
			lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_VIDEO_EVENT, NOTIFIER_MINOR_VIDEO_YUV_CLOSE));
		else
			lidbg("Error: video ops[%d], format[%d]\n", video_ops, video_input_format);
		break;
	default:
		break;
	}
}

int _video_cmds_do(char *cmd)
{
	int ret = 0;
	int vidoe_ops = VIDEO_OPS_UNKNOWN;
	int video_type = VIDEO_UNKNOWN;
	int video_format = VIDEO_INPUT_UNKNOW;

	if(!strcmp(cmd, "vehicle_open")){
		vidoe_ops = VIDEO_OPS_OPEN;
		video_type = VIDEO_VEHICLE;
		video_format = VIDEO_INPUT_CVBS;
	}
	else if(!strcmp(cmd, "vehicle_close")){
		vidoe_ops = VIDEO_OPS_CLOSE;
		video_type = VIDEO_VEHICLE;
		video_format = VIDEO_INPUT_CVBS;
	}
	else if(!strcmp(cmd, "dvd_open")){
		vidoe_ops = VIDEO_OPS_OPEN;
		video_type = VIDEO_DVD;
		video_format = VIDEO_INPUT_YUV;

	}
	else if(!strcmp(cmd, "dvd_close")){
		vidoe_ops = VIDEO_OPS_CLOSE;
		video_type = VIDEO_DVD;
		video_format = VIDEO_INPUT_YUV;
	}
	else if(!strcmp(cmd, "tv_open")){
		vidoe_ops = VIDEO_OPS_OPEN;
		video_type = VIDEO_TV;
		video_format = VIDEO_INPUT_CVBS;
	}
	else if(!strcmp(cmd, "tv_close")){
		vidoe_ops = VIDEO_OPS_CLOSE;
		video_type = VIDEO_TV;
		video_format = VIDEO_INPUT_CVBS;
	}
	else if(!strcmp(cmd, "dvr_open")){
		vidoe_ops = VIDEO_OPS_OPEN;
		video_type = VIDEO_DVR;
		video_format = VIDEO_INPUT_CVBS;
	}else if(!strcmp(cmd, "dvr_close")){
		vidoe_ops = VIDEO_OPS_CLOSE;
		video_type = VIDEO_DVR;
		video_format = VIDEO_INPUT_CVBS;
	}
	else if(!strcmp(cmd, "aux_open")){
		vidoe_ops = VIDEO_OPS_OPEN;
		video_type = VIDEO_AUX;
		video_format = VIDEO_INPUT_CVBS;
	}else if(!strcmp(cmd, "aux_close")){
		vidoe_ops = VIDEO_OPS_CLOSE;
		video_type = VIDEO_AUX;
		video_format = VIDEO_INPUT_CVBS;
	}
	else{
		vidoe_ops = VIDEO_OPS_UNKNOWN;
		video_type = VIDEO_UNKNOWN;
		video_format = VIDEO_INPUT_UNKNOW;

		printk("***** Unknown video type:%s ******\n", cmd);
		return 1;
	}

//	if(vidoe_ops == VIDEO_OPS_OPEN)
//		ret = video_chip_init(video_type);

	video_ops(vidoe_ops, video_format);

	return 0;
}

ssize_t  video_ad_write (struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    char *cmd[8] = {NULL};
    int cmd_num  = 0;
    char cmd_buf[512];
    memset(cmd_buf, '\0', 512);
    
    if(copy_from_user(cmd_buf, buf, size))
    {
        lidbg("copy_from_user ERR\n");
    }
    if(cmd_buf[size - 1] == '\n')
        cmd_buf[size - 1] = '\0';

    cmd_num = lidbg_token_string(cmd_buf, " ", cmd);

	printk("***** video_ad_write:%s ******\n", cmd[0]);

	if(!strcmp(cmd[0], "video_init")){
		_video_ad_init(cmd[1]);
	}
	else if(!strcmp(cmd[0], "video_set")){
		_video_ad_set(cmd[1], cmd[2], cmd[3]);
	}
	else if(!strcmp(cmd[0], "video_get")){
		_video_ad_get(cmd[1], cmd[2]);
	}
	else if(!strcmp(cmd[0], "video_reset")){
		_video_ad_hw_reset();
	}
	else if(!strcmp(cmd[0], "video_check")){
		_video_ad_check();
	}
	else if(!strcmp(cmd[0], "video_dbg")){
		_video_dbg();
	}
	else if(!_video_cmds_do(cmd[0])){
		printk("***** video cmd ::: %s ******\n", cmd[0]);
	}
	else
		lidbg("Error: undefined vehicle cmd[%s]\n", cmd[0]);

	 return size;
}

ssize_t  video_ad_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{
    char video_state[128 + 8] = {0};
    sprintf(video_state, "%s%d", "video_format", video_format);
    if (copy_to_user(buffer, video_state, strlen(video_state)))
    {
        lidbg("Video read ERR\n");
    }

    lidbg("video_ad_read:::[%s]\n", video_state);
    return size;
}

int video_ad_open (struct inode *inode, struct file *filp)
{
    return 0;
}

static  struct file_operations video_ad_fops =
{
    .owner = THIS_MODULE,
    .write = video_ad_write,
    .read = video_ad_read,
    .open = video_ad_open,
};

static int video_ad_ops_suspend(struct device *dev)
{
	DUMP_FUN;
    	return 0;
}

static int video_ad_ops_resume(struct device *dev)
{
	DUMP_FUN;

	return 0;
}

static int video_event(struct notifier_block *this, unsigned long event, void *ptr)
{
	DUMP_FUN;

	switch (event)
	{
		case NOTIFIER_VALUE(NOTIFIER_MAJOR_VIDEO_EVENT, NOTIFIER_MINOR_VIDEO_CVBS_OPEN):
			fs_file_write(VIDEO_NODE, false, "cvbs_open", 0, strlen("cvbs_open"));
			break;

		case NOTIFIER_VALUE(NOTIFIER_MAJOR_VIDEO_EVENT, NOTIFIER_MINOR_VIDEO_CVBS_CLOSE):
			fs_file_write(VIDEO_NODE, false, "cvbs_close", 0, strlen("cvbs_close"));
			break;

		case NOTIFIER_VALUE(NOTIFIER_MAJOR_VIDEO_EVENT, NOTIFIER_MINOR_VIDEO_YUV_OPEN):
			fs_file_write(VIDEO_NODE, false, "yuv_open", 0, strlen("yuv_open"));
			break;

		case NOTIFIER_VALUE(NOTIFIER_MAJOR_VIDEO_EVENT, NOTIFIER_MINOR_VIDEO_YUV_CLOSE):
			fs_file_write(VIDEO_NODE, false, "yuv_close", 0, strlen("yuv_close"));
			break;

		default:
			break;
	}

	return NOTIFY_DONE;
}

static struct notifier_block video_notifier =
{
    .notifier_call = video_event,
};

static struct miscdevice video_dev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "vehicle",
	.fops  		= &video_ad_fops,
};

static struct dev_pm_ops video_ad_pm_ops =
{
    .suspend	= video_ad_ops_suspend,
    .resume	= video_ad_ops_resume,
};

static int video_ad_probe(struct platform_device *pdev)
{       
	DUMP_FUN;
	int ret = 0;

	register_lidbg_notifier(&video_notifier);

	/* register misc device*/
	ret = misc_register(&video_dev);
	if (ret)
	{
		printk("ERROR: could not register vechile devices\n");
		return ret;
	}
        
	return 0;	
}

static int video_ad_remove(struct platform_device *pdev)
{
	misc_deregister(&video_dev);

	return 0;
}

static struct platform_device video_ad_devices =
{
    .name			= "video_ad",
    .id 			= 0,
};

static struct platform_driver video_ad_driver =
{
    .probe = video_ad_probe,
    .remove = video_ad_remove,
    .driver = {
        .name = "video_ad",
        .owner = THIS_MODULE,
	.pm = &video_ad_pm_ops,

    },
};

static int __devinit video_ad_init(void)
{ 
	DUMP_BUILD_TIME;
	LIDBG_GET;
	
    platform_device_register(&video_ad_devices);
    platform_driver_register(&video_ad_driver);
    return 0;

}

static void __exit video_ad_exit(void){}
module_init(video_ad_init);
module_exit(video_ad_exit);

MODULE_AUTHOR("fly, <fly@gmail.com>");
MODULE_DESCRIPTION("Devices Driver");
MODULE_LICENSE("GPL");


