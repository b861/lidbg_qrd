#include "video_ad.h"
#include "lidbg.h"

LIDBG_DEFINE;

#define NOTIFIER_MAJOR_VIDEO_EVENT (119)
#define NOTIFIER_MINOR_VIDEO_CVBS_OPEN (0)
#define NOTIFIER_MINOR_VIDEO_CVBS_CLOSE (1)
#define NOTIFIER_MINOR_VIDEO_YUV_OPEN (2)
#define NOTIFIER_MINOR_VIDEO_YUV_CLOSE (3)

#define IMGLEVEL_MIN 0
#define IMGLEVEL_MAX 11
#define IMG_VAL_ERR(x) ((x)>=IMGLEVEL_MIN&&(x)<IMGLEVEL_MAX?0:1)

#define VIDEO_NODE "/dev/px3_vehicle"

static int video_format = 0;
static int cur_video_state = VIDEO_OPS_UNKNOWN;
static int cur_video_format = VIDEO_INPUT_UNKNOW;
struct semaphore video_ops_sem;
static DECLARE_COMPLETION(video_stable_state);

extern int video_unstable_flag;
extern int video_ck_unmatched;

static unsigned char img_config[5][2] =
{
    {0x0, 0x0},
    {0x0, 0x0},

    {0x0, 0x0},
    {0x0, 0x0},
    {0x0, 0x0},
};

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

int video_reg_set(int reg, int data)
{
	int ret = 0;
	unsigned char config[] = {0x0, 0x0};

	config[0] = reg;
	config[1] = data;

	ret = SOC_I2C_Send(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, config, sizeof(config) / sizeof(config[0]));
	if(ret < 0){
		lidbg("Error: %s failed.\n",__func__);
		return 0;
	}
	else
		return ret;
}

int video_reg_get(int reg, char *data)
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
	char val = 0;
	int ret = 0;

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

void _video_black_enable(void)
{
	int ret = 0;
	unsigned char val = 0;

	lidbg("*** video black enable ***\n");

	//black colour
	ret = video_reg_set(0xc0, 0x0);
	ret &= video_reg_set(0xc1, 0x80);
	ret &= video_reg_set(0xc2, 0x80);

	ret = video_reg_get(0xbf, &val);

	val |= 0x01;	//enable force display colour
	ret &= video_reg_set(0xbf, val);
	if(!ret)
		lidbg("_video_black_enable error\n");
}

void _video_black_disable(void)
{
	int ret = 0;
	unsigned char val = 0;

	lidbg("*** video black disable ***\n");

	ret = video_reg_get(0xbf, &val);

	val &= 0xfe;	//disble force display colour
	ret = video_reg_set(0xbf, val);
	lidbg("*** video black disable: 0x%x ***\n", val);

	if(!ret)
		lidbg("_video_black_disable error\n");
}

void _video_dbg(void)
{
	int i = 0;
	char data = 0;
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

//	_video_black_enable();

	return ret;
}

int _video_img_config(int img_type)
{
	int ret = 0;
	unsigned char config[] = {0x00, 0x00};

	switch (img_type)
    {
    case VIDEO_BRIGHTNESS:
		config[0] = img_config[VIDEO_BRIGHTNESS][0];
		config[1] = img_config[VIDEO_BRIGHTNESS][1];
		break;
    case VIDEO_CONTRAST:
		config[0] = img_config[VIDEO_CONTRAST][0];
		config[1] = img_config[VIDEO_CONTRAST][1];
		break;
    case VIDEO_SATURATION_U:
		config[0] = img_config[VIDEO_SATURATION_U][0];
		config[1] = img_config[VIDEO_SATURATION_U][1];
		break;
    case VIDEO_SATURATION_V:
		config[0] = img_config[VIDEO_SATURATION_V][0];
		config[1] = img_config[VIDEO_SATURATION_V][1];
		break;
    case VIDEO_HUE:
		config[0] = img_config[VIDEO_HUE][0];
		config[1] = img_config[VIDEO_HUE][1];
		break;
    default:
        break;
    }

	ret = SOC_I2C_Send(VIDEO_AD_I2C_BUS, VIDEO_AD_I2C_ADDR, config, sizeof(config) / sizeof(config[0]));
	if(ret < 0)
		lidbg("_video_img_config error\n");

	return ret;
}

void _video_img_set(int flag, int img_type)
{
	if(flag){
		switch (img_type)
		{
			case VIDEO_BRIGHTNESS:
				_video_img_config(VIDEO_BRIGHTNESS);
				break;
			case VIDEO_CONTRAST:
				_video_img_config(VIDEO_CONTRAST);
				break;
			case VIDEO_SATURATION_U:
				_video_img_config(VIDEO_SATURATION_U);
				_video_img_config(VIDEO_SATURATION_V);
				break;
			case VIDEO_HUE:
				_video_img_config(VIDEO_HUE);
				break;
			default:
				break;
		}
	}
}

void _video_img_format(int img_level, int img_type)
{
	switch (cur_video_format)
	{
		case VIDEO_INPUT_CVBS:
			switch (img_type)
			{
				case VIDEO_BRIGHTNESS:
					img_config[VIDEO_BRIGHTNESS][0] = img_cvbs[VIDEO_BRIGHTNESS][0];
					img_config[VIDEO_BRIGHTNESS][1] = VIDEO_IMAGE_CVBS[VIDEO_BRIGHTNESS][img_level];
					lidbg("video CVBS img level[%d], brightness[reg-0x%x]::: 0x%x \n", img_level, img_config[VIDEO_BRIGHTNESS][0], img_config[VIDEO_BRIGHTNESS][1]);
					break;
				case VIDEO_CONTRAST:
					img_config[VIDEO_CONTRAST][0] = img_cvbs[VIDEO_CONTRAST][0];
					img_config[VIDEO_CONTRAST][1] = VIDEO_IMAGE_CVBS[VIDEO_CONTRAST][img_level];
					lidbg("video CVBS img level[%d], hue[reg-0x%x]::: 0x%x \n", img_level, img_config[VIDEO_CONTRAST][0], img_config[VIDEO_CONTRAST][1]);
					break;
				case VIDEO_SATURATION_U:
					img_config[VIDEO_SATURATION_U][0] = img_cvbs[VIDEO_SATURATION_U][0];
					img_config[VIDEO_SATURATION_V][0] = img_cvbs[VIDEO_SATURATION_V][0];
					img_config[VIDEO_SATURATION_U][1] = VIDEO_IMAGE_CVBS[VIDEO_SATURATION_U][img_level];
					img_config[VIDEO_SATURATION_V][1] = VIDEO_IMAGE_CVBS[VIDEO_SATURATION_V][img_level];
					lidbg("video CVBS img level[%d], saturation_u[reg-0x%x]::: 0x%x \n", img_level, img_config[VIDEO_SATURATION_U][0], img_config[VIDEO_SATURATION_U][1]);
					lidbg("video CVBS img level[%d], saturation_v[reg-0x%x]::: 0x%x \n", img_level, img_config[VIDEO_SATURATION_V][0], img_config[VIDEO_SATURATION_V][1]);
					break;
				case VIDEO_HUE:
					img_config[VIDEO_HUE][0] = img_cvbs[VIDEO_HUE][0];
					img_config[VIDEO_HUE][1] = VIDEO_IMAGE_CVBS[VIDEO_HUE][img_level];
					lidbg("video CVBS img level[%d], hue[reg-0x%x]::: 0x%x \n", img_level, img_config[VIDEO_HUE][0], img_config[VIDEO_HUE][1]);
					break;
				default:
					break;
			}
			break;
		case VIDEO_INPUT_YUV:
			switch (img_type)
			{
				case VIDEO_BRIGHTNESS:
					img_config[VIDEO_BRIGHTNESS][0] = img_yuv[VIDEO_BRIGHTNESS][0];
					img_config[VIDEO_BRIGHTNESS][1] = VIDEO_IMAGE_YUV[VIDEO_BRIGHTNESS][img_level];
					lidbg("video YUV img level[%d], brightness[reg-0x%x]::: 0x%x \n", img_level, img_config[VIDEO_BRIGHTNESS][0], img_config[VIDEO_BRIGHTNESS][1]);
					break;
				case VIDEO_CONTRAST:
					img_config[VIDEO_CONTRAST][0] = img_yuv[VIDEO_CONTRAST][0];
					img_config[VIDEO_CONTRAST][1] = VIDEO_IMAGE_YUV[VIDEO_CONTRAST][img_level];
					lidbg("video YUV img level[%d], contrast[reg-0x%x]::: 0x%x \n", img_level, img_config[VIDEO_CONTRAST][0], img_config[VIDEO_CONTRAST][1]);
					break;
				case VIDEO_SATURATION_U:
					img_config[VIDEO_SATURATION_U][0] = img_yuv[VIDEO_SATURATION_U][0];
					img_config[VIDEO_SATURATION_V][0] = img_yuv[VIDEO_SATURATION_V][0];
					img_config[VIDEO_SATURATION_U][1] = VIDEO_IMAGE_YUV[VIDEO_SATURATION_U][img_level];
					img_config[VIDEO_SATURATION_V][1] = VIDEO_IMAGE_YUV[VIDEO_SATURATION_V][img_level];
					lidbg("video YUV img level[%d], saturation_u[reg-0x%x]::: 0x%x \n", img_level, img_config[VIDEO_SATURATION_U][0], img_config[VIDEO_SATURATION_U][1]);
					lidbg("video YUV img level[%d], saturation_v[reg-0x%x]::: 0x%x \n", img_level, img_config[VIDEO_SATURATION_V][0], img_config[VIDEO_SATURATION_V][1]);
					break;
				case VIDEO_HUE:
					img_config[VIDEO_HUE][0] = img_yuv[VIDEO_HUE][0];
					img_config[VIDEO_HUE][1] = VIDEO_IMAGE_YUV[VIDEO_HUE][img_level];
					lidbg("video YUV img level[%d], hue[reg-0x%x]::: 0x%x \n", img_level, img_config[VIDEO_HUE][0], img_config[VIDEO_HUE][1]);
					break;
				default:
					break;
			}
		break;
		case VIDEO_INPUT_UNKNOW:
			lidbg("video img unknown intput format \n");
			break;
		default:
			break;
	}
}

int _video_ad_img(char *config, char *par)
{
	int ret = -1;
	int img_type = -1;
	int set_flag = 0;

	if(!strcmp(config, "brightness")){
		int brightness_val = simple_strtoul(par, 0, 0);

		if(!IMG_VAL_ERR(brightness_val)){
			set_flag = 1;
			img_type = VIDEO_BRIGHTNESS;
			_video_img_format(brightness_val, img_type);
		}else
			lidbg("Error video img invalid brightness level %d\n", brightness_val);
	}else if(!strcmp(config, "contrast")){
		int contras_val = simple_strtoul(par, 0, 0);

		if(!IMG_VAL_ERR(contras_val)){
			set_flag = 1;
			img_type = VIDEO_CONTRAST;
			_video_img_format(contras_val, img_type);
		}else
			lidbg("Error video img invalid contrast level %d\n", contras_val);
	}else if(!strcmp(config, "saturation")){
		int saturation_val = simple_strtoul(par, 0, 0);

		if(!IMG_VAL_ERR(saturation_val)){
			set_flag = 1;
			img_type = VIDEO_SATURATION_U;
			_video_img_format(saturation_val, img_type);
		}else
			lidbg("Error video img invalid saturation level %d\n", saturation_val);
	}else if(!strcmp(config, "hue")){
		int hue_val = simple_strtoul(par, 0, 0);

		if(!IMG_VAL_ERR(hue_val)){
			set_flag = 1;
			img_type = VIDEO_HUE;
			_video_img_format(hue_val, img_type);
		}else
			lidbg("Error video img invalid hue level %d\n", hue_val);
	}else
		lidbg("Error: undefined[%s] vehicle value to set\n", config);

	_video_img_set(set_flag, img_type);

	return ret;
}


int _video_ad_reg_set(char *reg_add, char *data)
{
	int ret = -1;

	int reg = simple_strtoul(reg_add, 0, 0);
	int val = simple_strtoul(data, 0, 0);

	ret = video_reg_set(reg, val);
	if(ret < 0)
		lidbg("Vehicle ad set errot, ret=%d \n", ret);
	else
		lidbg("Vehicle reg set 0x%x = 0x%x\n", reg, val);

	return ret;
}

int _video_ad_reg_get(char *reg_add, char *reg_size)
{
	int ret = -1;
	int i = 0;
	char data = 0;

	int reg = simple_strtoul(reg_add, 0, 0);
	int size = simple_strtoul(reg_size, 0, 0);

	for(i=0; i<size; i++){
		ret = video_reg_get(reg, &data);
		if(ret < 0)
			lidbg("Vehicle ad get errot, ret=%d \n", ret);
		else
			lidbg("Vehicle reg get 0x%x = 0x%x\n", reg, data);
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

void video_ops(int video_ops, int video_input_format)
{
	switch (video_ops)
	{
	case VIDEO_OPS_OPEN:
		LCD_OFF;
		if(cur_video_format == VIDEO_INPUT_CVBS)
			fs_file_write(VIDEO_NODE, false, "cvbs_open", 0, strlen("cvbs_open"));
		else if(cur_video_format == VIDEO_INPUT_YUV)
			fs_file_write(VIDEO_NODE, false, "yuv_open", 0, strlen("yuv_open"));
		else{
			lidbg("ERROR: Video open event unknown cur video format !!!\n");
			break;
		}
		complete(&video_stable_state);

		break;
	case VIDEO_OPS_CLOSE:
		LCD_ON;
		if(cur_video_format == VIDEO_INPUT_CVBS)
			fs_file_write(VIDEO_NODE, false, "cvbs_close", 0, strlen("cvbs_close"));
		else if(cur_video_format == VIDEO_INPUT_YUV)
			fs_file_write(VIDEO_NODE, false, "yuv_close", 0, strlen("yuv_close"));
		else{
			lidbg("ERROR: Video close event unknown cur video format !!!\n");
			break;
		}
		break;
	default:
		break;
	}
}

int _video_cmds_do(char *cmd)
{
//	int ret = 0;
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

//	if(vidoe_ops == VIDEO_OPS_OPEN){
//		ret = video_chip_init(video_type);
//		_video_black_enable();
//	}
	cur_video_state = vidoe_ops;
	cur_video_format = video_format;
	video_ops(vidoe_ops, video_format);

	return 0;
}

ssize_t  video_ad_write (struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	char *cmd[8] = {NULL};
	int cmd_num  = 0;
	char cmd_buf[512];
	memset(cmd_buf, '\0', 512);

	down(&video_ops_sem);
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
		_video_ad_img(cmd[1], cmd[2]);
	}
	else if(!strcmp(cmd[0], "video_reg_set")){
		_video_ad_reg_set(cmd[1], cmd[2]);
	}
	else if(!strcmp(cmd[0], "video_reg_get")){
		_video_ad_reg_get(cmd[1], cmd[2]);
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
	up(&video_ops_sem);

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

static int thread_video_state(void *data)
{
	int lcd_state = 0;
	int video_stable = 0;
	int video_state_check_cnt = 0;

	while(!kthread_should_stop()){
		if(!wait_for_completion_interruptible(&video_stable_state))
			lcd_state = 0;
			video_stable = 0;
			video_state_check_cnt = 0;
			lidbg(" *** video signal stability check ***\n");
			while(1){
				msleep(50);
//				lidbg(" *** %d %d %d***\n", video_unstable_flag, video_state_check_cnt, video_stable);
				if(cur_video_format == VIDEO_INPUT_YUV){
					if((video_unstable_flag == 0) && (lcd_state == 0)){
							video_stable++;
							video_state_check_cnt = 0;

							if(video_stable > 5){
								lcd_state = 1;
								video_stable = 0;
								LCD_ON;
								break;
							}
					}
				}else if(cur_video_format == VIDEO_INPUT_CVBS){
					if((video_unstable_flag == 0) && (video_ck_unmatched == 0) && (lcd_state == 0)){
							video_stable++;
							video_state_check_cnt = 0;

							if(video_stable > 10){
								lcd_state = 1;
								video_stable = 0;
								LCD_ON;
								break;
							}
					}
				}else
					break;

				if(video_state_check_cnt > 100){
					lidbg(" *** video signal stability check timeout ***\n");
					video_state_check_cnt = 0;
					LCD_ON;
					break;
				}
				video_state_check_cnt ++;
			}
	}

	return 1;
}

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
	int ret = 0;
	DUMP_FUN;

	kthread_run(thread_video_state, NULL, "ftf_pmtask");

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

	sema_init(&video_ops_sem, 1);
	CREATE_KTHREAD(thread_video_state, NULL);

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


