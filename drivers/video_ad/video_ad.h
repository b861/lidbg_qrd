#ifndef __VIDEO_AD__
#define __VIDEO_AD__

#define VIDEO_AD_I2C_BUS		3
#define VIDEO_CHIP_ADV7181D

#define VIDEO_FORMAT_PAL  100
#define VIDEO_FORMAT_NTSC 200

#define DISABLE 0
#define ENABLE 1

struct video_chip_info
{
    char *chip_name;
    int chip_addr;
};

enum
{
    VIDEO_OPS_OPEN,
    VIDEO_OPS_CLOSE,
    VIDEO_OPS_UNKNOWN,
};

enum
{
    VIDEO_INPUT_CVBS,
    VIDEO_INPUT_YUV,
    VIDEO_INPUT_UNKNOW,
};

enum
{
    VIDEO_INPUT_MUX_ADC0,
    VIDEO_INPUT_MUX_ADC1,
    VIDEO_INPUT_MUX_ADC2,
    VIDEO_INPUT_MUX_ADC3,
    VIDEO_INPUT_MUX_UNKNOW,
};

enum
{
    VIDEO_INPUT_CH_AIN1,
    VIDEO_INPUT_CH_AIN2,
    VIDEO_INPUT_CH_AIN3,
    VIDEO_INPUT_CH_AIN4,
    VIDEO_INPUT_CH_AIN5,
    VIDEO_INPUT_CH_AIN6,
    VIDEO_INPUT_CH_AIN7,
    VIDEO_INPUT_CH_AIN8,
    VIDEO_INPUT_CH_AIN9,
    VIDEO_INPUT_CH_AIN10,
    VIDEO_INPUT_CH_UNKNOWN,
};

enum
{
    VIDEO_ADC_AUTO,
    VIDEO_ADC_MANUAL,
    VIDEO_ADC_UNKNOWN,
};

enum
{
    VIDEO_GAIN_AUTO,
    VIDEO_GAIN_MANUAL,
    VIDEO_GAIN_UNKNOWN,
};

enum
{
    VIDEO_VEHICLE,
    VIDEO_DVD,
    VIDEO_TV,
    VIDEO_DVR,
    VIDEO_AUX,
    VIDEO_UNKNOWN,
};

enum
{
    VIDEO_FORMAT_NTSM_MJ,
    VIDEO_FORMAT_NTSC_443,
    VIDEO_FORMAT_PAL_M,
    VIDEO_FORMAT_PAL_60,
    VIDEO_FORMAT_PAL_BGHID,
    VIDEO_FORMAT_SECAM,
    VIDEO_FORMAT_PAL_N,
    VIDEO_FORMAT_SECAM_525,
    VIDEO_FORMAT_UNKNOWN,
};

enum
{
    VIDEO_BRIGHTNESS,
    VIDEO_CONTRAST,
    VIDEO_HUE,
    VIDEO_SATURATION_U,
    VIDEO_SATURATION_V,
};

#ifdef VIDEO_CHIP_ADV7181D
#define  VIDEO_AD_I2C_ADDR			0x21

#define  VIDEO_REG_AIN_SEL			0x00
#define  VIDEO_REG_GAIN_CTRL		0x73

//ADC REG
#define  VIDEO_REG_ADC0_SWITCH		0xC3
#define  VIDEO_REG_ADC1_SWITCH		0xC3
#define  VIDEO_REG_ADC2_SWITCH		0xC4
#define  VIDEO_REG_ADC3_SWITCH		0xF3

#define  VIDEO_REG_ADC0_OFFSET		0
#define  VIDEO_REG_ADC1_OFFSET		4
#define  VIDEO_REG_ADC2_OFFSET		0
#define  VIDEO_REG_ADC3_OFFSET		4

#define VIDEO_ADC_SEL_AIN1	0X09
#define VIDEO_ADC_SEL_AIN2	0X01
#define VIDEO_ADC_SEL_AIN3	0X02
#define VIDEO_ADC_SEL_AIN4	0X0B
#define VIDEO_ADC_SEL_AIN5	0X03
#define VIDEO_ADC_SEL_AIN6	0X04
#define VIDEO_ADC_SEL_AIN7	0X0D
#define VIDEO_ADC_SEL_AIN8	0X05
#define VIDEO_ADC_SEL_AIN9	0X0E
#define VIDEO_ADC_SEL_AIN10	0X06

#define  VIDEO_REG_ADC_SWITCH2		0xC4

static unsigned char VIDEO_YUV[] =
{
    0x42, 0x05, 0x01,	// PRIM_MODE = 001b COMP
    0x42, 0x06, 0x06,	// VID_STD for 525P 2x1
    //		0x42,0xC3,0x56,	// ADC1 to Ain8, ADC0 to Ain10,
    //		0x42,0xC4,0xB4,	// Enables manual override of mux & ADC2 to Ain6.
    0x42, 0xC3, 0x00,	// reset ADC1 , ADC0 set
    0x42, 0xC4, 0xB0,	// Enables manual override of mux & reset ADC2 set
    0x42, 0x1D, 0x47,	// Enable 28.63636MHz crystal

    0x42, 0x0F, 0x02,	// mtc power ctl

    0x42, 0x3A, 0x11,	// Set Latch Clock 01b. Power down ADC3.
    0x42, 0x3B, 0x81,	// Enable Internal Bias
    0x42, 0x3C, 0x3B,	// PLL QPUMP to 011b
    0x42, 0x6B, 0x83,	// 422 8bit out
    0x42, 0xC9, 0x00,	// SDR mode
    0x42, 0x73, 0xCF,	// Enable Manual Gain and set CH_A gain
    0x42, 0x74, 0xA3,	// Set CH_A and CH_B Gain - 0FAh
    0x42, 0x75, 0xE8,	// Set CH_B and CH_C Gain
    0x42, 0x76, 0xFA,	// Set CH_C Gain
    0x42, 0x7B, 0x1C,	// Turn off EAV and SAV Codes. Set BLANK_RGB_SEL.

    0x42, 0x7C, 0x90,	// mtc

    0x42, 0x85, 0x19,	// Turn off SSPD and force SOY
    0x42, 0x86, 0x0B,	// Enable STDI Line Count Mode

    0x42, 0xBF, 0x06,	// Blue Screen Free Run Colour
    0x42, 0xC0, 0x40,	// default color
    0x42, 0xC1, 0xF0,	// default color
    0x42, 0xC2, 0x80,	// Default color
    0x42, 0xC5, 0x01,	//
    0x42, 0xC9, 0x08,	// Enable 8-bit mode using psuedo DDR
    0x42, 0x0E, 0x80,	// ADI recommended sequence
    0x42, 0x52, 0x46,	// ADI recommended sequence
    0x42, 0x54, 0x80,	// ADI Recommended Setting
    0x42, 0x57, 0x01,	// ADI recommended sequence
    0x42, 0xF6, 0x3B,	// ADI Recommended Setting
    0x42, 0x0E, 0x00,	// ADI recommended sequence
    0x42, 0x67, 0x2F,	// DPP Filters
};


static unsigned char VIDEO_CVBS[] =
{
    //		0x42,0xc3,0x09,
    //		0x42,0xc4,0x80,

    //		0x42,0x00,0x0B, //; CVBS input on AIN1
    0x42, 0xc3, 0x00, //reset ADC1 , ADC0 set
    0x42, 0xc4, 0x00, //reset ADC2 set
    0x42, 0x00, 0x00, //reset CVBS input set

    0x42, 0x03, 0x0C, //; 8 Bit Mode
    0x42, 0x04, 0x75, //0x77, //; Enable SFL

    0x42, 0x0C, 0x02, // mtc blue
    0x42, 0x0D, 0x88, // mtc blue

    0x42, 0x17, 0x41, //; select SH1
    0x42, 0x1D, 0x47, //; Enable 28MHz Crystal
    0x42, 0x31, 0x12, //0x02, //; Clears NEWAV_MODE, SAV/EAV  to suit ADV video encoders

    0x42, 0x32, 0xC1,
    0x42, 0x33, 0x04,

    0x42, 0x34, 0x01,
    0x42, 0x35, 0xF0,
    0x42, 0x36, 0x08,
    0x42, 0x37, 0x81,

    0x42, 0x0F, 0x0A,	// mtc power ctl

    0x42, 0x3A, 0x17, //; Set Latch Clock & power down ADC 1 & ADC2 & ADC3
    0x42, 0x3B, 0x81, //; Enable internal Bias
    0x42, 0x3D, 0xA2, //; MWE Enable Manual Window, Colour Kill Threshold to 2
    0x42, 0x3E, 0x6A, //; BLM optimisation
    0x42, 0x3F, 0xA0, //; BGB
    0x42, 0x86, 0x0B, //; Enable stdi_line_count_mode
    0x42, 0xF3, 0x01, //; Enable Anti Alias Filter on ADC0
    0x42, 0xF9, 0x03, //; Set max v lock range

    0x42, 0x6B, 0x80,

    0x42, 0x0E, 0x80, //; ADI Recommended Setting
    0x42, 0x52, 0x46, //; ADI Recommended Setting
    0x42, 0x54, 0x80, //; ADI Recommended Setting
    0x42, 0x7F, 0xFF, //; ADI Recommended Setting
    0x42, 0x81, 0x30, //; ADI Recommended Setting
    0x42, 0x90, 0xC9, //; ADI Recommended Setting
    0x42, 0x91, 0x40, //; ADI Recommended Setting
    0x42, 0x92, 0x3C, //; ADI Recommended Setting
    0x42, 0x93, 0xCA, //; ADI Recommended Setting
    0x42, 0x94, 0xD5, //; ADI Recommended Setting
    0x42, 0xB1, 0xFF, //; ADI Recommended Setting
    0x42, 0xB6, 0x08, //; ADI Recommended Setting
    0x42, 0xC0, 0x9A, //; ADI Recommended Setting
    0x42, 0xCF, 0x50, //; ADI Recommended Setting
    0x42, 0xD0, 0x4E, //; ADI Recommended Setting
    0x42, 0xD1, 0xB9, //; ADI Recommended Setting
    0x42, 0xD6, 0xDD, //; ADI Recommended Setting
    0x42, 0xD7, 0xE2, //; ADI Recommended Setting
    0x42, 0xE5, 0x51, //; ADI Recommended Setting
    0x42, 0xF6, 0x3B, //; ADI Recommended Setting
    0x42, 0x0E, 0x00, //; ADI Recommended Setting

    0x42, 0x08, 0x8c,
    0x42, 0x0a, 0xcc,
};

const unsigned char img_cvbs[5][2] =
{
    {0x0a, 0xcc}, //BRIGHTNESS
    {0x08, 0x8c}, //CONTRAST

    {0x0b, 0x00}, //HUE
    {0xe3, 0x80}, //VIDEO_SATURATION_U
    {0xe4, 0x80}, //VIDEO_SATURATION_V
};

const unsigned char VIDEO_IMAGE_CVBS[5][11] =
{
    /*---0-------1-------2-------3-------4-------5-------6-------7-------8-------9-------10---*/
    {	0x80,		0X90,		0x9f,		0xae,		0xbd,		0xcc,		0xdb,		0xea,		0xf9,		0x09,		0x18,}, 	//BRIGHTNESS
    {	0x19,		0x30,		0x47,		0x5e,		0x75,		0x8c,		0xa3,		0xba,		0xd1,		0xe8,		0xff,},	//CONTRAST
    {	0x96,		0xaf,		0xc8,		0xe1,		0xf8,		0x00,		0x16,		0x32,		0x4b,		0x64,		0x7d,},	//HUE
    {	0x03,		0x1c,		0x35,		0x4e,		0x67,		0x80,		0x99,		0xb2,		0xcb,		0xe4,		0xfd,},	//VIDEO_SATURATION_U
    {	0x03,		0x1c,		0x35,		0x4e,		0x67,		0x80,		0x99,		0xb2,		0xcb,		0xe4,		0xfd,},	//VIDEO_SATURATION_V
};

const unsigned short VIDEO_IMAGE_YUV[5][11] =
{
    /*---0-------1-------2-------3-------4-------5-------6-------7-------8-------9-------10---*/
    {	0x00,		0X32,		0x64,		0x96,		0xc8,		0xfa,		0x12c,	0x15e,	0x190,	0x1c2,	0x1f4,}, 	//BRIGHTNESS
    {	0x00,		0X32,		0x64,		0x96,		0xc8,		0xfa,		0x12c,	0x15e,	0x190,	0x1c2,	0x1f4,},	//CONTRAST
    {	0x00,		0X32,		0x64,		0x96,		0xc8,		0xfa,		0x12c,	0x15e,	0x190,	0x1c2,	0x1f4,},	//HUE
    {	0x00,		0X32,		0x64,		0x96,		0xc8,		0xfa,		0x12c,	0x15e,	0x190,	0x1c2,	0x1f4,},	//VIDEO_SATURATION_U
};
#endif

#endif

