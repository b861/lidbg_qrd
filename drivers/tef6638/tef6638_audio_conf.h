#ifndef __TEF6638_DAT2_H__
#define __TEF6638_DAT2_H__

//select Radio 

BYTE TEF6638_Audio_Init_Data[]={
 //Set the Audio UseCase to: 2 
0xC6,2,0x3F,0x20, //(Audio ICC_SYSTEM=$20 ) 
0xC6,2,0x3F,0x20, //(Audio ICC_SYSTEM=$20 )  
0xC6,2,0x3F,0x20, //(Audio ICC_SYSTEM=$20 ) 
0xC6,3,0xC9,0x02,0x01,//(Audio Special Feature=$02 $,0x01,) 
//All devices and sub-systems are initialised,set global settings......
//repare for controlling FM defaults
0xC6,2,0x00,0x00,//(Tuning ICC_MODE=$00 ) 
//Read Current value from all ICC registers
0xC6,3,0xC3,0x01,0x00,//(Audio Special Feature=$,0x01,$00 ) 
0xC6,3,0xC3,0x02,0x04,//(Audio Special Feature=$02 $,0x04,) 
0xC6,6,0xC3,0x03,0x06,0x03,0x02,0x04,//(Audio Special Feature=$,0x03,$,0x06,$,0x03,$02 $,0x04,) 
0xC6,6,0xC3,0x03,0x06,0x03,0x02,0x04,//(Audio Special Feature=$,0x03,$,0x06,$,0x03,$02 $,0x04,) 
0xC6,6,0xC3,0x03,0x06,0x03,0x02,0x04,//(Audio Special Feature=$,0x03,$,0x06,$,0x03,$02 $,0x04,) 
0xC6,6,0xC3,0x03,0x06,0x03,0x02,0x04,//(Audio Special Feature=$,0x03,$,0x06,$,0x03,$02 $,0x04,) 
//Send Delta's for FM
0xC6,2,0x11,0x0A,//(AM/FM ICC_SOFTMUTE_LIM=$,0x0A,) 
0xC6,2,0x16,0x04,//(AM/FM ICC_HIGHCUT_LIM=$,0x04,) 
0xC6,2,0x18,0x75, //(AM/FM ICC_STEREO_TIME=$75 ) 
//Read Default value from all ICC registers
//repare for controlling AM defaults
0xC6,2,0x00,0x02, //(Tuning ICC_MODE=$02 ) 
//Read Current value from all ICC registers
//Send Delta's for AM
0xC6,2,0x03,0x0A,//(AM/FM ICC_TUNER=$,0x0A,) 
0xC6,2,0x06,0x0A,//(AM/FM ICC_SIGNAL=$,0x0A,) 
0xC6,2,0x0D,0x05,//(AM/FM ICC_SOFTMUTE_TIME=$,0x05,) 
0xC6,2,0x0F,0x40, //(AM/FM ICC_SOFTMUTE_LEV=$40 ) 
0xC6,2,0x11,0x04,//(AM/FM ICC_SOFTMUTE_LIM=$,0x04,) 
0xC6,2,0x12,0x12, //(AM/FM ICC_HIGHCUT_TIME=$12 ) 
0xC6,2,0x14,0x5B, //(AM/FM ICC_HIGHCUT_LEV=$5B ) 
0xC6,2,0x17,0x05,//(AM/FM ICC_HIGHCUT_LOC=$,0x05,) 
//Activate Device
//Set the ICC Tuner Band to: FM and frequency to: 89300 kHz
0xC6,4,0x00,0x10,0x22,0xE2,//(Tuning ICC_MODE=$10 $22 $,0xE2,) 
0xC6,3,0xC3,0x01,0x00,//(Audio Special Feature=$,0x01,$00 ) 
0xC6,3,0xC3,0x02,0x04,//(Audio Special Feature=$02 $,0x04,) 
0xC6,6,0xC3,0x03,0x06,0x03,0x02,0x04,//(Audio Special Feature=$,0x03,$,0x06,$,0x03,$02 $,0x04,) 
0xC6,6,0xC3,0x03,0x06,0x03,0x02,0x04,//(Audio Special Feature=$,0x03,$,0x06,$,0x03,$02 $,0x04,) 
0xC6,6,0xC3,0x03,0x06,0x03,0x02,0x04,//(Audio Special Feature=$,0x03,$,0x06,$,0x03,$02 $,0x04,) 
0xC6,6,0xC3,0x03,0x06,0x03,0x02,0x04,//(Audio Special Feature=$,0x03,$,0x06,$,0x03,$02 $,0x04,) 
//ost-Initialise the HERO subsystems.......
//Initialise the Audio modules.............
//Set All audio scaling coefficients......
0xC6,6,0xF2,0x02,0x8C,0x08,0x00,0x91, 
0xC6,5,0xF2,0x43,0x02,0x04,0x00,
0xC6,5,0xF2,0x43,0x03,0x04,0x00,
0xC6,5,0xF2,0x43,0x04,0x04,0x00,
0xC6,5,0xF2,0x43,0x05,0x04,0x00,
0xC6,6,0xF2,0x02,0x8F,0x20,0x01,0x22,
0xC6,5,0xF2,0x42,0xFD,0x07,0xFF, 
0xC6,5,0xF2,0x42,0xFE,0x07,0xFF, 
0xC6,5,0xF2,0x42,0xFF,0x07,0xFF, 
0xC6,5,0xF2,0x43,0x00,0x07,0xFF, 
0xC6,5,0xF2,0x43,0x01,0x07,0xFF, 
0xC6,5,0xF2,0x43,0x06,0x01,0x00,
//Init DS Controls.....
0xC6,3,0xA9,0x0A,0x05,//(Audio Special Feature=$,0x0A,$,0x05,) 
0xC6,3,0xA9,0x0B,0x05,//(Audio Special Feature=$,0x0B,$,0x05,) 
0xC6,3,0xA9,0x0C,0x05,//(Audio Special Feature=$,0x0C,$,0x05,) 
0xC6,3,0xA9,0x14,0x05,//(Audio Special Feature=$,0x14,$,0x05,) 
0xC6,3,0xA9,0x15,0x05,//(Audio Special Feature=$15 $,0x05,) 
0xC6,3,0xA9,0x16,0x05,//(Audio Special Feature=$16 $,0x05,) 
0xC6,3,0xA9,0x17,0x05,//(Audio Special Feature=$17 $,0x05,) 
0xC6,3,0xA9,0x18,0x00,//(Audio Special Feature=$18 $00 ) 
0xC6,3,0xA9,0x1F,0x02, //(Audio Special Feature=$,0x1F,$02 ) 
0xC6,3,0xA9,0x00,0x00,//(Audio Special Feature=$00 $00 ) 
0xC6,3,0xA9,0x01,0x05,//(Audio Special Feature=$,0x01,$,0x05,) 
0xC6,3,0xA9,0x02,0x00,//(Audio Special Feature=$02 $00 ) 
0xC6,3,0xA9,0x03,0x00,//(Audio Special Feature=$,0x03,$00 ) 
0xC6,3,0xA9,0x1E,0x00,//(Audio Special Feature=$,0x1E,$00 ) 
0xC6,3,0xA9,0x28,0x00,//(Audio Special Feature=$28 $00 ) 
0xC6,3,0xA9,0x32,0x00,//(Audio Special Feature=$32 $00 ) 
0xC6,3,0xA9,0x33,0x00,//(Audio Special Feature=$,0x33,$00 ) 
0xC6,6,0xF2,0x05,0xCA,0x00,0x03,0x2B,  
0xC6,6,0xF2,0x04,0x87,0x00,0x02,0xF2, 
0xC6,6,0xF2,0x04,0x88,0x00,0x02,0xF4, 
0xC6,6,0xF2,0x04,0x89,0x00,0x02,0xF2, 
0xC6,6,0xF2,0x04,0x8A,0x00,0x02,0xF2, 
0xC6,6,0xF2,0x04,0x8A,0x00,0x02,0xF2, 
0xC6,6,0xF2,0x04,0x8A,0x00,0x02,0xF2, 
0xC6,2,0x20,0x08,//(Audio ICC__INUT=$08,AIN0 ) 
0xC6,2,0x28,0x00,//(Audio ICC_S_INUT=$00 ) 
0xC6,3,0xC2,0x03,0x00,//(Audio Special Feature=$,0x03,$00 ) 
0xC6,4,0xC2,0x04,0x05,0x05,//(Audio Special Feature=$,0x04,$,0x05,$,0x05,) 
0xC6,4,0xC2,0x04,0x05,0x05,//(Audio Special Feature=$,0x04,$,0x05,$,0x05,) 
//Init Graphical Spectrum Analyser
0xC6,6,0xF2,0x05,0xF3,0x00,0x03,0x2E, 
0xC6,5,0xF2,0x46,0x96,0x01,0x1B,  
0xC6,5,0xF2,0x46,0x99,0x00,0x01, 
0xC6,5,0xF2,0x46,0x95,0x02,0x37,  
0xC6,5,0xF2,0x46,0x98,0x0C,0x02,  
0xC6,5,0xF2,0x46,0x94,0x05,0x22,  
0xC6,5,0xF2,0x46,0x97,0x07,0xFD, 
0xC6,5,0xF2,0x46,0x9C,0x02,0x32,  
0xC6,5,0xF2,0x46,0x9F,0x00,0x02,  
0xC6,5,0xF2,0x46,0x9B,0x04,0x64, 
0xC6,5,0xF2,0x46,0x9E,0x0C,0x04, 
0xC6,5,0xF2,0x46,0x9A,0x01,0x04, 
0xC6,5,0xF2,0x46,0x9D,0x07,0xFB,  
0xC6,5,0xF2,0x46,0xA2,0x04,0x50,  
0xC6,5,0xF2,0x46,0xA5,0x00,0x04, 
0xC6,5,0xF2,0x46,0xA1,0x00,0xA0,  
0xC6,5,0xF2,0x46,0xA4,0x0C,0x09,  
0xC6,5,0xF2,0x46,0xA0,0x05,0x07, 
0xC6,5,0xF2,0x46,0xA3,0x07,0xF5, 
0xC6,5,0xF2,0x46,0xA8,0x00,0x09,  
0xC6,5,0xF2,0x46,0xA7,0x0C,0x12,  
0xC6,5,0xF2,0x46,0xA6,0x07,0xE9,  
0xC6,5,0xF2,0x46,0xAB,0x00,0x12,  
0xC6,5,0xF2,0x46,0xAA,0x0C,0x24,  
0xC6,5,0xF2,0x46,0xA9,0x07,0xC8, 
0xC6,5,0xF2,0x46,0xAE,0x00,0x23,  
0xC6,5,0xF2,0x46,0xAD,0x0C,0x46,  
0xC6,5,0xF2,0x46,0xAC,0x07,0x6A,  
0xC6,5,0xF2,0x46,0xB1,0x00,0x44,  
0xC6,5,0xF2,0x46,0xB0,0x0C,0x88,  
0xC6,5,0xF2,0x46,0xAF,0x06,0x4A,  
0xC6,5,0xF2,0x46,0xB4,0x00,0x80, 
0xC6,5,0xF2,0x46,0xB3,0x0C,0xFF,  
0xC6,5,0xF2,0x46,0xB2,0x02,0xED,  
0xC6,5,0xF2,0x46,0xB7,0x00,0xEA,  
0xC6,5,0xF2,0x46,0xB6,0x0D,0xD4,  
0xC6,5,0xF2,0x46,0xB5,0x0B,0xFB,  
0xC6,5,0xF2,0x46,0x93,0x01,0x00, 
0xC6,7,0xF2,0x46,0xCA,0x02,0x2C,0x08,0x44,  
0xC6,7,0xF2,0x46,0xCC,0x06,0xA3,0x00,0x84, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x50,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x5E, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x66, 
//Init Volume
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0xCA,
0xC6,5,0xF2,0x43,0x8D,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0x8E,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0x8F,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0x90,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0x91,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0x88,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0x89,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0x8C,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0x9B,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0x9C,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0x9D,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0x9E,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0xA1,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0xA2,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0x9F,0x0F,0xFF, 
0xC6,5,0xF2,0x43,0x93,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0x94,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0x95,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0x96,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0x97,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0x92,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0x98,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0x99,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0x9A,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0xA0,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0x8A,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0x8B,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0xAB,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0xAC,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0xAD,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0xAE,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0xA3,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0xA4,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0xA5,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0xA6,0x0F,0xFF,  
0xC6,5,0xF2,0x43,0xAF,0x07,0xFA, 
0xC6,5,0xF2,0x43,0xB0,0x00,0x06, 
0xC6,5,0xF2,0x43,0x24,0x04,0x00, 
0xC6,5,0xF2,0x43,0x25,0x04,0x00, 
0xC6,5,0xF2,0x43,0x26,0x04,0x00, 
0xC6,5,0xF2,0x43,0x27,0x04,0x00, 
0xC6,5,0xF2,0x43,0x28,0x04,0x00, 
0xC6,5,0xF2,0x43,0x29,0x04,0x00, 
0xC6,5,0xF2,0x43,0x2A,0x04,0x00, 
0xC6,5,0xF2,0x43,0x2B,0x04,0x00, 
0xC6,5,0xF2,0x43,0x2C,0x04,0x00, 
0xC6,5,0xF2,0x43,0x2D,0x04,0x00, 
0xC6,6,0xF2,0x02,0x89,0x00,0x00,0x00,
//Init Balance
0xC6,5,0xF2,0x42,0xF9,0x07,0xFF,  
0xC6,5,0xF2,0x42,0xFA,0x07,0xFF, 
0xC6,5,0xF2,0x42,0xF7,0x07,0xFF,  
0xC6,5,0xF2,0x42,0xF8,0x07,0xFF,  
//Init Fader
0xC6,5,0xF2,0x42,0xF5,0x07,0xFF,  
0xC6,5,0xF2,0x42,0xF6,0x07,0xFF,  
// Init the compressor expander
0xC6,5,0xF2,0x43,0x83,0x00,0x00, 
0xC6,6,0xF2,0x04,0xE3,0x80,0x00,0x00, 
0xC6,5,0xF2,0x43,0x85,0x00,0x1D,  
0xC6,5,0xF2,0x43,0x86,0x00,0x1D,  
0xC6,5,0xF2,0x43,0x84,0x07,0xC6, 
0xC6,7,0xF2,0x43,0x75,0x05,0x96,0x07,0xFF,  
0xC6,7,0xF2,0x43,0x73,0x02,0x6A,0x00,0x00, 
//Init Loudness
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0xB0,  
0xC6,5,0xF2,0x45,0x99,0x07,0xFF,  
0xC6,5,0xF2,0x45,0x98,0x07,0xFF,  
0xC6,7,0xF2,0x45,0x9C,0x02,0xB6,0x08,0x09,  
0xC6,7,0xF2,0x45,0x9E,0x02,0x73,0x00,0x12,  
0xC6,7,0xF2,0x45,0xA3,0x02,0xB6,0x08,0x09,  
0xC6,7,0xF2,0x45,0xA5,0x02,0x73,0x00,0x12, 
0xC6,5,0xF2,0x45,0xAE,0x04,0x00, 
//Init Tone
0xC6,25,0xF2,0x42,0xCE,0x03,0xED,0x07,0x63,0x00,0x00,0x00,0x00,0x00,0x09,0x00,0x4E,0x00,0x09,0x00,0x4E,0x00,0x00,0x00,0x00,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x06,0x05, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0B, 
0xC6,25,0xF2,0x42,0xCE,0x03,0xED,0x07,0x63,0x00,0x00,0x00,0x00,0x00,0x09,0x00,0x4E,0x00,0x09,0x00,0x4E,0x00,0x00,0x00,0x00,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x06,0x22,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0B, 
0xC6,17,0xF2,0x42,0xCE,0x07,0x85,0x00,0x09,0x0C,0x6E,0x03,0x62,0x00,0x37,0x01,0xB1,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x05,0xFD, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,17,0xF2,0x42,0xCE,0x07,0x85,0x00,0x09,0x0C,0x6E,0x03,0x62,0x00,0x37,0x01,0xB1,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x06,0x1A,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,15,0xF2,0x42,0xCE,0x01,0xD5,0x00,0x00,0x02,0xEA,0x0D,0x16,0x00,0x00,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x06,0x10,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x06, 
0xC6,15,0xF2,0x42,0xCE,0x01,0xD5,0x00,0x00,0x02,0xEA,0x0D,0x16,0x00,0x00,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x06,0x2D,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x06, 
0xC6,5,0xF2,0x42,0xDA,0x07,0xFF,  
0xC6,5,0xF2,0x42,0xDC,0x08,0x01, 
0xC6,5,0xF2,0x42,0xDB,0x08,0x01, 
0xC6,5,0xF2,0x42,0xDD,0x07,0xFF,  
0xC6,5,0xF2,0x42,0xDF,0x08,0x01,
0xC6,5,0xF2,0x42,0xDE,0x08,0x01, 
//Init Graphic Equaliser
0xC6,17,0xF2,0x42,0xCE,0x05,0x13,0x00,0x03,0x02,0x26,0x0C,0x07,0x05,0x6F,0x07,0xF8,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x05,0x3F,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,5,0xF2,0x43,0x48,0x07,0xFF,  
0xC6,17,0xF2,0x42,0xCE,0x03,0x13,0x00,0x0E,0x06,0x27,0x0C,0x1C,0x03,0x4A,0x07,0xE2,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x05,0x46,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07,
0xC6,5,0xF2,0x43,0x48,0x07,0xFF,  
0xC6,17,0xF2,0x42,0xCE,0x01,0xB1,0x00,0x37,0x03,0x62,0x0C,0x6E,0x00,0x09,0x07,0x85,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x05,0x4D,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,5,0xF2,0x43,0x48,0x07,0xFF,  
0xC6,11,0xF2,0x42,0xCE,0x00,0xBE,0x0D,0x7C,0x05,0xD9,0x00,0x00,
0xC6,6,0xF2,0x02,0xB8,0x00,0x05,0x54,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x04, 
0xC6,5,0xF2,0x43,0x48,0x07,0xFF, 
0xC6,11,0xF2,0x42,0xCE,0x01,0xE8,0x0F,0xD1,0x0E,0xF4,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x05,0x58,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x04, 
0xC6,5,0xF2,0x43,0x48,0x07,0xFF,
//Init Ph/Nav
0xC6,23,0xF2,0x42,0xCE,0x01,0xA2,0x00,0x44,0x01,0xA2,0x00,0x44,0x00,0x00,0x00,0x00,0x04,0xBB,0x03,0x77,0x00,0x00,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x06,0x37,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,23,0xF2,0x42,0xCE,0x03,0xB4,0x00,0x80,0x03,0xB4,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x98,0x02,0xFF,0x00,0x00,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x06,0x41,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,23,0xF2,0x42,0xCE,0x01,0xA2,0x00,0x44,0x01,0xA2,0x00,0x44,0x00,0x00,0x00,0x00,0x04,0xBB,0x03,0x77,0x00,0x00,0x00,0x00,
0xC6,6,0xF2,0x02,0xB8,0x00,0x06,0x4B,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A,
0xC6,23,0xF2,0x42,0xCE,0x03,0xB4,0x00,0x80,0x03,0xB4,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x98,0x02,0xFF,0x00,0x00,0x00,0x00,
0xC6,6,0xF2,0x02,0xB8,0x00,0x06,0x55,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
// Init HShift
0xC6,5,0xF2,0x43,0x4D,0x07,0xFF,
0xC6,5,0xF2,0x43,0x4E,0x07,0xFF,
0xC6,5,0xF2,0x43,0x4F,0x07,0xFF,
0xC6,5,0xF2,0x43,0x50,0x07,0xFF,
0xC6,5,0xF2,0x43,0x51,0x07,0xFF,
0xC6,5,0xF2,0x43,0x52,0x07,0xFF,
//Init SrcScal
0xC6,5,0xF2,0x42,0xFB,0x04,0x00, 
0xC6,5,0xF2,0x43,0x54,0x04,0x00, 
//Init arametric Equaliser
0xC6,17,0xF2,0x42,0xCE,0x01,0xF2,0x00,0x07,0x03,0xE4,0x0C,0x0E,0x02,0x75,0x07,0xF1,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x03,0xE7,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,17,0xF2,0x42,0xCE,0x03,0x13,0x00,0x0E,0x06,0x27,0x0C,0x1C,0x03,0x4A,0x07,0xE2,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x03,0xEE, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,17,0xF2,0x42,0xCE,0x02,0xF7,0x00,0x1C,0x05,0xEE,0x0C,0x38,0x00,0x35,0x07,0xC4,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x03,0xF5, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,17,0xF2,0x42,0xCE,0x01,0xB1,0x00,0x37,0x03,0x62,0x0C,0x6E,0x00,0x09,0x07,0x85,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x03,0xFC,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x00,0xE2,0x09,0x01,0x03,0x9D,0x03,0x2E,0x07,0x1E,0x06,0xFE,0x04,0x62,0x0C,0xD1, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x59,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x04,0x6C,0x0A,0x26,0x06,0x70,0x02,0x83,0x03,0x94,0x05,0xD9,0x01,0x8F,0x0D,0x7C,  
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x63,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x05,0xB7,0x0C,0xA2,0x05,0x58,0x01,0x7E,0x02,0x49,0x03,0x5D,0x02,0xA8,0x0E,0x81,  
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x6D,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,17,0xF2,0x42,0xCE,0x01,0xF2,0x00,0x07,0x03,0xE4,0x0C,0x0E,0x02,0x75,0x07,0xF1,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x03, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,17,0xF2,0x42,0xCE,0x03,0x13,0x00,0x0E,0x06,0x27,0x0C,0x1C,0x03,0x4A,0x07,0xE2,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x0A, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,17,0xF2,0x42,0xCE,0x02,0xF7,0x00,0x1C,0x05,0xEE,0x0C,0x38,0x00,0x35,0x07,0xC4,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x11,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,17,0xF2,0x42,0xCE,0x01,0xB1,0x00,0x37,0x03,0x62,0x0C,0x6E,0x00,0x09,0x07,0x85,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x18,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x00,0xE2,0x09,0x01,0x03,0x9D,0x03,0x2E,0x07,0x1E,0x06,0xFE,0x04,0x62,0x0C,0xD1, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x77, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x04,0x6C,0x0A,0x26,0x06,0x70,0x02,0x83,0x03,0x94,0x05,0xD9,0x01,0x8F,0x0D,0x7C,  
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x81,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x05,0xB7,0x0C,0xA2,0x05,0x58,0x01,0x7E,0x02,0x49,0x03,0x5D,0x02,0xA8,0x0E,0x81,  
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x8B,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,17,0xF2,0x42,0xCE,0x01,0xF2,0x00,0x07,0x03,0xE4,0x0C,0x0E,0x02,0x75,0x07,0xF1,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x1F, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,17,0xF2,0x42,0xCE,0x03,0x13,0x00,0x0E,0x06,0x27,0x0C,0x1C,0x03,0x4A,0x07,0xE2,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x26,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,17,0xF2,0x42,0xCE,0x02,0xF7,0x00,0x1C,0x05,0xEE,0x0C,0x38,0x00,0x35,0x07,0xC4,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x2D,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,17,0xF2,0x42,0xCE,0x01,0xB1,0x00,0x37,0x03,0x62,0x0C,0x6E,0x00,0x09,0x07,0x85,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x34,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x00,0xE2,0x09,0x01,0x03,0x9D,0x03,0x2E,0x07,0x1E,0x06,0xFE,0x04,0x62,0x0C,0xD1, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x95,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x04,0x6C,0x0A,0x26,0x06,0x70,0x02,0x83,0x03,0x94,0x05,0xD9,0x01,0x8F,0x0D,0x7C,  
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x9F,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x05,0xB7,0x0C,0xA2,0x05,0x58,0x01,0x7E,0x02,0x49,0x03,0x5D,0x02,0xA8,0x0E,0x81,  
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0xA9,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,17,0xF2,0x42,0xCE,0x01,0xF2,0x00,0x07,0x03,0xE4,0x0C,0x0E,0x02,0x75,0x07,0xF1,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x3B, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,17,0xF2,0x42,0xCE,0x03,0x13,0x00,0x0E,0x06,0x27,0x0C,0x1C,0x03,0x4A,0x07,0xE2,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x42,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,17,0xF2,0x42,0xCE,02,0xF7,0x00,0x1C,0x05,0xEE,0x0C,0x38,0x00,35,0x07,0xC4,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x49,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,17,0xF2,0x42,0xCE,0x01,0xB1,0x00,0x37,0x03,0x62,0x0C,0x6E,0x00,0x09,0x07,0x85,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0x50,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x00,0xE2,0x09,0x01,0x03,0x9D,0x03,0x2E,0x07,0x1E,0x06,0xFE,0x04,0x62,0x0C,0xD1, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0xB3,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x04,0x6C,0x0A,0x26,0x06,0x70,0x02,0x83,0x03,0x94,0x05,0xD9,0x01,0x8F,0x0D,0x7C,  
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0xBD,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x05,0xB7,0x0C,0xA2,0x05,0x58,0x01,0x7E,0x02,0x49,0x03,0x5D,0x02,0xA8,0x0E,0x81,  
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0xC7,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
//Init Super osition Module B
0xC6,6,0xF2,0x07,0x53,0x00,0x03,0x0E,  
0xC6,6,0xF2,0x07,0x54,0x00,0x03,0x0E,  
0xC6,6,0xF2,0x07,0x55,0x00,0x03,0x0E,  
0xC6,6,0xF2,0x07,0x56,0x00,0x03,0x0E,  
0xC6,6,0xF2,0x07,0x57,0x00,0x02,0xE0, 
0xC6,6,0xF2,0x07,0x58,0x00,0x02,0xE1,  
0xC6,6,0xF2,0x07,0x59,0x00,0x03,0x2A,  
0xC6,6,0xF2,0x07,0x5A,0x00,0x03,0x2B,  
0xC6,5,0xF2,0x46,0xEE,0x00,0x00, 
0xC6,5,0xF2,0x46,0xF2,0x00,0x00, 
0xC6,5,0xF2,0x46,0xEF,0x00,0x00, 
0xC6,5,0xF2,0x46,0xF3,0x00,0x00, 
0xC6,5,0xF2,0x46,0xF0,0x00,0x00, 
0xC6,5,0xF2,0x46,0xF4,0x00,0x00, 
0xC6,5,0xF2,0x46,0xF1,0x00,0x00, 
0xC6,5,0xF2,0x46,0xF5,0x00,0x00, 
0xC6,5,0xF2,0x46,0xF6,0x00,0x00, 
0xC6,5,0xF2,0x46,0xFA,0x00,0x00, 
0xC6,5,0xF2,0x46,0xF7,0x00,0x00, 
0xC6,5,0xF2,0x46,0xFB,0x00,0x00, 
0xC6,5,0xF2,0x46,0xF8,0x00,0x00, 
0xC6,5,0xF2,0x46,0xFC,0x00,0x00, 
0xC6,5,0xF2,0x46,0xF9,0x00,0x00, 
0xC6,5,0xF2,0x46,0xFD,0x00,0x00, 
0xC6,5,0xF2,0x46,0xFE,0x00,0x00, 
0xC6,5,0xF2,0x46,0xFF,0x00,0x00, 
0xC6,5,0xF2,0x47,0x00,0x00,0x00, 
0xC6,5,0xF2,0x47,0x01,0x00,0x00, 
0xC6,5,0xF2,0x47,0x02,0x00,0x00, 
0xC6,5,0xF2,0x47,0x03,0x00,0x00, 
0xC6,5,0xF2,0x47,0x04,0x00,0x00, 
0xC6,5,0xF2,0x47,0x05,0x00,0x00, 
//Init Sine generator
0xC6,9,0xF2,0x45,0xF5,0x07,0xEB,0x01,0xFD,0x00,0x01, 
0xC6,5,0xF2,0x45,0xFB,0x0F,0x33, 
0xC6,5,0xF2,0x45,0xFC,0x0F,0x33, 
//Init Noise generator
0xC6,21,0xF2,0x45,0x76,0x03,0x33,0x07,0xFF,0x08,0x14,0x04,0xCD,0x07,0xFF,0x09,0x9A,0x00,0x00,0x01,0x50,0x00,0x00, 
0xC6,5,0xF2,0x45,0x7F,0x00,0xCD, 
0xC6,5,0xF2,0x45,0x80,0x00,0xCD, 
//Init Delay Lines
0xC6,6,0xF2,0x06,0xE3,0x00,0x00,0x01, 
0xC6,6,0xF2,0x06,0xE9,0x7F,0xFF,0xFF,
0xC6,6,0xF2,0x06,0xE4,0x00,0x00,0x01, 
0xC6,6,0xF2,0x06,0xE9,0x7F,0xFF,0xFF,
0xC6,6,0xF2,0x06,0xE5,0x00,0x00,0x01, 
0xC6,6,0xF2,0x06,0xE9,0x7F,0xFF,0xFF,
0xC6,6,0xF2,0x06,0xE6,0x00,0x00,0x01, 
0xC6,6,0xF2,0x06,0xE9,0x7F,0xFF,0xFF,
0xC6,6,0xF2,0x06,0xE7,0x00,0x00,0x01, 
0xC6,6,0xF2,0x06,0xE9,0x7F,0xFF,0xFF,
0xC6,6,0xF2,0x06,0xE8,0x00,0x00,0x01, 
0xC6,6,0xF2,0x06,0xE9,0x7F,0xFF,0xFF,
//Init Limiter
0xC6,6,0xF2,0x06,0x76,0x00,0x00,0x00, 
0xC6,6,0xF2,0x06,0x77,0x00,0x00,0x00, 
0xC6,6,0xF2,0x06,0x7F,0x7F,0xFF,0xFF,
0xC6,7,0xF2,0x46,0x82,0x03,0x49,0x08,0x05, 
0xC6,7,0xF2,0x46,0x84,0x02,0xD2,0x00,0x0A, 
0xC6,6,0xF2,0x06,0x78,0x00,0x00,0x00, 
0xC6,6,0xF2,0x06,0x80,0x7F,0xFF,0xFF,
0xC6,7,0xF2,0x46,0x86,0x03,0x49,0x08,0x05, 
0xC6,7,0xF2,0x46,0x88,0x02,0xD2,0x00,0x0A, 
0xC6,6,0xF2,0x06,0x79,0x00,0x00,0x00, 
0xC6,6,0xF2,0x06,0x81,0x7F,0xFF,0xFF,
0xC6,7,0xF2,0x46,0x8A,0x03,0x49,0x08,0x05, 
0xC6,7,0xF2,0x46,0x8C,0x02,0xD2,0x00,0x0A, 
0xC6,6,0xF2,0x06,0x7A,0x00,0x00,0x00, 
0xC6,6,0xF2,0x06,0x82,0x7F,0xFF,0xFF,
0xC6,7,0xF2,0x46,0x8E,0x03,0x49,0x08,0x05, 
0xC6,7,0xF2,0x46,0x90,0x02,0xD2,0x00,0x0A, 
0xC6,6,0xF2,0x04,0x96,0x03,0x00,0x01, 
0xC6,6,0xF2,0x04,0x97,0x00,0x00,0x00, 
0xC6,6,0xF2,0x04,0x98,0x00,0x00,0x00, 
0xC6,6,0xF2,0x04,0x99,0x00,0x00,0x00, 
0xC6,6,0xF2,0x04,0x9A,0x00,0x00,0x00, 
0xC6,7,0xF2,0x43,0x61,0x05,0x56,0x03,0xFA, 
0xC6,7,0xF2,0x43,0x5F,0x01,0xB6,0x04,0x00, 
0xC6,7,0xF2,0x43,0x65,0x05,0x56,0x03,0xFA, 
0xC6,7,0xF2,0x43,0x63,0x01,0xB6,0x04,0x00, 
0xC6,7,0xF2,0x43,0x69,0x05,0x56,0x03,0xFA, 
0xC6,7,0xF2,0x43,0x67,0x01,0xB6,0x04,0x00, 
0xC6,7,0xF2,0x43,0x6D,0x05,0x56,0x03,0xFA, 
0xC6,7,0xF2,0x43,0x6B,0x01,0xB6,0x04,0x00, 
0xC6,6,0xF2,0x04,0x8E,0x80,0x00,0x00, 
0xC6,6,0xF2,0x04,0x90,0x80,0x00,0x00, 
0xC6,6,0xF2,0x04,0x92,0x80,0x00,0x00, 
0xC6,6,0xF2,0x04,0x94,0x80,0x00,0x00, 
//Init Chime
//Enable Chime plus DC
0xC6,7,0xF2,0x43,0xCF,0x01,0xFD,0x07,0xEB, 
0xC6,7,0xF2,0x43,0xD4,0x01,0xFD,0x07,0xEB, 
0xC6,7,0xF2,0x43,0xD9,0x01,0xFD,0x07,0xEB, 
0xC6,7,0xF2,0x43,0xDE,0x01,0xFD,0x07,0xEB, 
0xC6,5,0xF2,0x43,0xD1,0x0D,0xFE, 
0xC6,5,0xF2,0x43,0xD6,0x0D,0xFE, 
0xC6,5,0xF2,0x43,0xDB,0x0D,0xFE, 
0xC6,5,0xF2,0x43,0xE0,0x0D,0xFE, 
0xC6,5,0xF2,0x43,0xE3,0x02,0x02,  
0xC6,6,0xF2,0x05,0x77,0x01,0x29,0x37,  
0xC6,6,0xF2,0x05,0x78,0x01,0x29,0x37,  
0xC6,6,0xF2,0x05,0x84,0x00,0x00,0x00, 
0xC6,5,0xF2,0x43,0xCD,0x00,0x01, 
0xC6,6,0xF2,0x05,0x79,0x7E,0xD8,0x21,  
0xC6,6,0xF2,0x05,0x7B,0x00,0x03,0x3B, 
0xC6,6,0xF2,0x05,0x7C,0x00,0x03,0x3B, 
0xC6,6,0xF2,0x05,0x7D,0x00,0x03,0x3B, 
0xC6,6,0xF2,0x05,0x7E,0x00,0x03,0x3B, 
0xC6,6,0xF2,0x05,0x7F,0x00,0x03,0x3B, 
0xC6,6,0xF2,0x05,0x80,0x00,0x03,0x3B, 
0xC6,6,0xF2,0x05,0x7A,0x00,0x00,0x03, 
0xC6,6,0xF2,0x05,0x83,0x40,0x26,0xE7,  
0xC6,6,0xF2,0x05,0x84,0x00,0x00,0x00, 
0xC6,5,0xF2,0x43,0xD2,0x00,0x80, 
0xC6,5,0xF2,0x43,0xD3,0x00,0x00, 
0xC6,5,0xF2,0x43,0xD7,0x00,0x80, 
0xC6,5,0xF2,0x43,0xD8,0x00,0x00, 
0xC6,5,0xF2,0x43,0xDC,0x00,0x80, 
0xC6,5,0xF2,0x43,0xDD,0x00,0x00, 
0xC6,5,0xF2,0x43,0xE1,0x00,0x80, 
0xC6,5,0xF2,0x43,0xE2,0x00,0x00, 
0xC6,6,0xF2,0x05,0x81,0x00,0x00,0x00, 
0xC6,5,0xF2,0x43,0xE4,0x08,0x00, 
0xC6,5,0xF2,0x43,0xE5,0x0F,0xFF,
0xC6,5,0xF2,0x43,0xE6,0x0F,0xFF,
0xC6,6,0xF2,0x05,0x78,0x05,0xCE,0x14, 
0xC6,6,0xF2,0x05,0x79,0x00,0xA5,0x1F, 
0xC6,6,0xF2,0x05,0x7B,0x00,0x00,0x53,  
0xC6,6,0xF2,0x05,0x7C,0x00,0x04,0x2D,  
0xC6,7,0xF2,0x43,0xCF,0x03,0x43,0x07,0xD1, 
0xC6,5,0xF2,0x43,0xD2,0x00,0x80, 
0xC6,6,0xF2,0x05,0x78,0x05,0xCE,0x14, 
0xC6,6,0xF2,0x05,0x79,0x00,0xA5,0x1F, 
0xC6,6,0xF2,0x05,0x7B,0x00,0x00,0x53,  
0xC6,6,0xF2,0x05,0x7C,0x00,0x02,0x74,  
0xC6,7,0xF2,0x43,0xCF,0x03,0x43,0x07,0xD1, 
0xC6,5,0xF2,0x43,0xD2,0x00,0x80, 
0xC6,6,0xF2,0x05,0x78,0x05,0xCE,0x14, 
0xC6,6,0xF2,0x05,0x79,0x00,0xA5,0x1F, 
0xC6,6,0xF2,0x05,0x7B,0x00,0x00,0x53,  
0xC6,6,0xF2,0x05,0x7C,0x00,0x00,0xB9, 
0xC6,7,0xF2,0x43,0xCF,0x03,0x43,0x07,0xD1, 
0xC6,5,0xF2,0x43,0xD2,0x00,0x80, 
0xC6,6,0xF2,0x05,0x78,0x05,0xCE,0x14, 
0xC6,6,0xF2,0x05,0x79,0x00,0xA5,0x1F, 
0xC6,6,0xF2,0x05,0x7B,0x80,0x00,0x00, 
0xC6,6,0xF2,0x05,0x7C,0x00,0x00,0xB9, 
0xC6,7,0xF2,0x43,0xCF,0x03,0x43,0x07,0xD1, 
0xC6,5,0xF2,0x43,0xD2,0x00,0x80, 
0xC6,5,0xF2,0x43,0xD4,0x08,0x00, 
0xC6,5,0xF2,0x43,0xD5,0x08,0x00, 
0xC6,5,0xF2,0x43,0xD6,0x08,0x00, 
0xC6,5,0xF2,0x43,0xD7,0x08,0x00, 
0xC6,6,0xF2,0x05,0x7A,0x00,0x1D,0xB9, 
0xC6,5,0xF2,0x43,0xD1,0x08,0x00, 
0xC6,6,0xF2,0x05,0x78,0x05,0xCE,0x14, 
0xC6,6,0xF2,0x05,0x79,0x00,0xA5,0x1F, 
0xC6,6,0xF2,0x05,0x7B,0x00,0x00,0x53,  
0xC6,6,0xF2,0x05,0x7C,0x00,0x04,0x2D,  
0xC6,7,0xF2,0x43,0xCF,0x03,0x43,0x07,0xD1, 
0xC6,5,0xF2,0x43,0xD2,0x00,0x80, 
0xC6,6,0xF2,0x05,0x78,0x05,0xCE,0x14, 
0xC6,6,0xF2,0x05,0x79,0x00,0xA5,0x1F, 
0xC6,6,0xF2,0x05,0x7B,0x00,0x00,0x53, 
0xC6,6,0xF2,0x05,0x7C,0x00,0x02,0x74, 
0xC6,7,0xF2,0x43,0xCF,0x03,0x43,0x07,0xD1, 
0xC6,5,0xF2,0x43,0xD2,0x00,0x80, 
0xC6,6,0xF2,0x05,0x78,0x05,0xCE,0x14, 
0xC6,6,0xF2,0x05,0x79,0x00,0xA5,0x1F, 
0xC6,6,0xF2,0x05,0x7B,0x00,0x00,0x53,  
0xC6,6,0xF2,0x05,0x7C,0x00,0x00,0xB9, 
0xC6,7,0xF2,0x43,0xCF,0x03,0x43,0x07,0xD1, 
0xC6,5,0xF2,0x43,0xD2,0x00,0x80, 
0xC6,6,0xF2,0x05,0x78,0x05,0xCE,0x14, 
0xC6,6,0xF2,0x05,0x79,0x00,0xA5,0x1F, 
0xC6,6,0xF2,0x05,0x7B,0x80,0x00,0x00, 
0xC6,6,0xF2,0x05,0x7C,0x00,0x00,0xB9, 
0xC6,7,0xF2,0x43,0xCF,0x03,0x43,0x07,0xD1, 
0xC6,5,0xF2,0x43,0xD2,0x00,0x80, 
0xC6,5,0xF2,0x43,0xD4,0x08,0x00, 
0xC6,5,0xF2,0x43,0xD5,0x08,0x00, 
0xC6,5,0xF2,0x43,0xD6,0x08,0x00, 
0xC6,5,0xF2,0x43,0xD7,0x08,0x00, 
0xC6,6,0xF2,0x05,0x7A,0x00,0x1D,0xB9, 
0xC6,5,0xF2,0x43,0xD1,0x08,0x00, 
0xC6,6,0xF2,0x05,0x83,0x4B,0x15,0xB5,  
//Init olyphonic Chime Generator
//Chime generator is enabled
0xC6,7,0xF2,0x46,0x5F,0x07,0xFD,0x01,0xBA,  
0xC6,5,0xF2,0x46,0x61,0x00,0xE6,  
0xC6,7,0xF2,0x46,0x62,0x07,0xD1,0x06,0x3A,  
0xC6,5,0xF2,0x46,0x64,0x00,0x20,  
0xC6,7,0xF2,0x46,0x65,0x07,0xFB,0x07,0xAB,  
0xC6,5,0xF2,0x46,0x67,0x02,0x88,  
0xC6,7,0xF2,0x46,0x68,0x07,0xC1,0x00,0xD0,  
0xC6,5,0xF2,0x46,0x6A,0x00,0x40,  
0xC6,7,0xF2,0x46,0x6B,0x07,0xF8,0x05,0x8F,  
0xC6,5,0xF2,0x46,0x6D,0x04,0x00, 
0xC6,7,0xF2,0x46,0x6E,0x07,0x8F,0x04,0x24,  
0xC6,5,0xF2,0x46,0x70,0x00,0x1C, 
0xC6,6,0xF2,0x06,0xB3,0x00,0x0A,0x67,  
0xC6,5,0xF2,0x46,0x71,0x02,0x66,  
0xC6,5,0xF2,0x46,0x72,0x05,0x99,  
0xC6,6,0xF2,0x06,0xB4,0x00,0x00,0x00, 
0xC6,5,0xF2,0x46,0x73,0x01,0x45,  
0xC6,6,0xF2,0x06,0xB5,0x17,0x38,0x4F, 
0xC6,6,0xF2,0x06,0xB6,0x00,0x03,0x04, 
0xC6,7,0xF2,0x46,0x74,0x07,0xFF,0x05,0xE0, 
0xC6,6,0xF2,0x06,0xB7,0x00,0x01,0xF0,  
0xC6,5,0xF2,0x46,0x76,0x00,0x01, 
0xC6,6,0xF2,0x06,0xB8,0x17,0x38,0x4F, 
0xC6,6,0xF2,0x06,0xB9,0x00,0x03,0x04, 
0xC6,7,0xF2,0x46,0x77,0x07,0xFF,0x05,0xE0, 
0xC6,6,0xF2,0x06,0xBA,0x00,0x03,0x7D,  
0xC6,5,0xF2,0x46,0x79,0x00,0x01, 
0xC6,6,0xF2,0x06,0xBB,0x17,0x38,0x4F, 
0xC6,6,0xF2,0x06,0xBC,0x00,0x03,0x04, 
0xC6,7,0xF2,0x46,0x7A,0x07,0xFF,0x05,0xE0, 
0xC6,7,0xF2,0x46,0x7C,0x07,0x4E,0x03,0x53,  
0xC6,6,0xF2,0x06,0xBD,0x00,0x00,0x01, 
0xC6,6,0xF2,0x06,0xBE,0x00,0x67,0x5C,  
0xC6,6,0xF2,0x06,0xBF,0x00,0x00,0x00, 
//Init FastMute
0xC6,6,0xF2,0x04,0xA0,0x00,0x00,0x00, 
0xC6,6,0xF2,0x04,0xA1,0x00,0x00,0x00, 
//Init General urpose Filters 1
0xC6,6,0xF2,0x05,0xD3,0x00,0x02,0xF2, 
0xC6,6,0xF2,0x05,0xD4,0x00,0x02,0xF3, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x04,0xF9,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x05,0x03, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
//Init General urpose Filters 2
0xC6,6,0xF2,0x05,0xD5,0x00,0x02,0xF2, 
0xC6,23,0xF2,0x42,0xCE,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x05,0x0D, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x0A, 
//Init SubWoofer
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0xE2, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0xEE, 
//Init QD module
0xC6,6,0xF2,0x05,0xE2,0x00,0x02,0xF2, 
0xC6,6,0xF2,0x05,0xE3,0x00,0x02,0xF3, 
0xC6,6,0xF2,0x05,0xE2,0x00,0x02,0xF2, 
0xC6,6,0xF2,0x05,0xE3,0x00,0x02,0xF3, 
0xC6,5,0xF2,0x45,0x81,0x05,0xF2, 
0xC6,5,0xF2,0x45,0x82,0x08,0x02,  
0xC6,5,0xF2,0x45,0x83,0x00,0xEB, 
0xC6,5,0xF2,0x45,0x84,0x00,0x05, 
0xC6,6,0xF2,0x03,0x3B,0x00,0x00,0x6E,  
0xC6,5,0xF2,0x45,0x81,0x05,0xF2, 
0xC6,5,0xF2,0x45,0x82,0x08,0x02,  
0xC6,5,0xF2,0x45,0x83,0x00,0xEB, 
0xC6,5,0xF2,0x45,0x84,0x00,0x05, 
0xC6,6,0xF2,0x03,0x3B,0x00,0x00,0x6E,  
0xC6,5,0xF2,0x45,0x81,0x05,0xF2, 
0xC6,5,0xF2,0x45,0x82,0x08,0x02,  
0xC6,5,0xF2,0x45,0x83,0x00,0xEB, 
0xC6,5,0xF2,0x45,0x84,0x00,0x05, 
0xC6,6,0xF2,0x03,0x3B,0x00,0x00,0x6E,  
0xC6,6,0xF2,0x05,0xE4,0x00,0x02,0xF4, 
0xC6,6,0xF2,0x05,0xE5,0x00,0x02,0xF5, 
0xC6,6,0xF2,0x05,0xE4,0x00,0x02,0xF4, 
0xC6,6,0xF2,0x05,0xE5,0x00,0x02,0xF5, 
0xC6,5,0xF2,0x45,0x85,0x05,0xF2, 
0xC6,5,0xF2,0x45,0x86,0x08,0x02,  
0xC6,5,0xF2,0x45,0x87,0x00,0xEB, 
0xC6,5,0xF2,0x45,0x88,0x00,0x05, 
0xC6,6,0xF2,0x03,0x3D,0x00,0x00,0x6E,  
0xC6,5,0xF2,0x45,0x85,0x05,0xF2, 
0xC6,5,0xF2,0x45,0x86,0x08,0x02,  
0xC6,5,0xF2,0x45,0x87,0x00,0xEB, 
0xC6,5,0xF2,0x45,0x88,0x00,0x05, 
0xC6,6,0xF2,0x03,0x3D,0x00,0x00,0x6E,  
0xC6,5,0xF2,0x45,0x85,0x05,0xF2, 
0xC6,5,0xF2,0x45,0x86,0x08,0x02,  
0xC6,5,0xF2,0x45,0x87,0x00,0xEB, 
0xC6,5,0xF2,0x45,0x88,0x00,0x05, 
0xC6,6,0xF2,0x03,0x3D,0x00,0x00,0x6E,  
//Init IO flags
0xC6,4,0xC0,0x04,0x11,0x05,//(Audio Special Feature=$,0x04,$11 $,0x05,) 
0xC6,4,0xC0,0x07,0x11,0x06,//(Audio Special Feature=$,0x07,$11 $,0x06,) 
0xC6,4,0xC0,0x08,0x11,0x06,//(Audio Special Feature=$,0x08,$11 $,0x06,) 
0xC6,4,0xC0,0x05,0x11,0x17,//(Audio Special Feature=$,0x05,$11 $17 ) 
0xC6,4,0xC0,0x03,0x00,0x0A,//(Audio Special Feature=$,0x03,$00 $,0x0A,) 
0xC6,4,0xC0,0x02,0x00,0x0B,//(Audio Special Feature=$02 $00 $,0x0B,) 
0xC6,4,0xC0,0x01,0x00,0x0C,//(Audio Special Feature=$,0x01,$00 $,0x0C,) 
//Init ClickClack module
0xC6,6,0xF2,0x06,0x06,0x00,0x00,0x00, 
0xC6,6,0xF2,0x06,0x07,0x00,0x00,0x00, 
0xC6,5,0xF2,0x43,0x1E,0x03,0xE5,  
0xC6,5,0xF2,0x43,0x1F,0x02,0x00, 
0xC6,5,0xF2,0x42,0xF3,0x07,0xFF,
0xC6,5,0xF2,0x42,0xF4,0x07,0xFF,
0xC6,5,0xF2,0x46,0x0F,0x00,0x00, 
0xC6,5,0xF2,0x43,0x45,0x07,0xFF,
0xC6,17,0xF2,0x42,0xCE,0x07,0x85,0x00,0x09,0x0C,0x6E,0x03,0x62,0x00,0x37,0x01,0xB1,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x05,0xFD, 
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,5,0xF2,0x43,0x45,0x07,0xFF,
0xC6,15,0xF2,0x42,0xCE,0x01,0xD5,0x00,0x00,0x02,0xEA,0x0D,0x16,0x00,0x00,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x06,0x10,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x06, 
0xC6,5,0xF2,0x43,0x45,0x07,0xFF,
0xC6,5,0xF2,0x42,0xF1,0x07,0xFF,
0xC6,5,0xF2,0x42,0xF2,0x07,0xFF,
0xC6,5,0xF2,0x43,0x20,0x02,0x00, 
0xC6,5,0xF2,0x43,0x21,0x00,0xCD, 
0xC6,5,0xF2,0x42,0xF9,0x07,0xFF,
0xC6,5,0xF2,0x42,0xFA,0x07,0xFF,
0xC6,5,0xF2,0x46,0x2C,0x00,0x00, 
0xC6,5,0xF2,0x43,0x49,0x07,0xFF,
0xC6,17,0xF2,0x42,0xCE,0x07,0x85,0x00,0x09,0x0C,0x6E,0x03,0x62,0x00,0x37,0x01,0xB1,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x06,0x1A,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x07, 
0xC6,5,0xF2,0x43,0x49,0x07,0xFF,
0xC6,15,0xF2,0x42,0xCE,0x01,0xD5,0x00,0x00,0x02,0xEA,0x0D,0x16,0x00,0x00,0x00,0x00, 
0xC6,6,0xF2,0x02,0xB8,0x00,0x06,0x2D,  
0xC6,6,0xF2,0x02,0xB9,0x00,0x00,0x06, 
0xC6,5,0xF2,0x43,0x49,0x07,0xFF,
0xC6,5,0xF2,0x43,0x1D,0x02,0x00, 
0xC6,5,0xF2,0x43,0x1C,0x02,0x00, 
//Init Mute
0xC6,5,0xF2,0x43,0x3B,0x08,0x00, 
0xC6,6,0xF2,0x02,0x93,0x00,0x0B,0xE3, 
0xC6,6,0xF2,0x02,0x92,0x00,0x76,0xE3, 
0xC6,5,0xF2,0x43,0x3C,0x08,0x00, 
0xC6,6,0xF2,0x02,0x96,0x00,0x0B,0xE3, 
0xC6,6,0xF2,0x02,0x95,0x00,0x76,0xE3, 
0xC6,5,0xF2,0x43,0x3D,0x08,0x00, 
0xC6,6,0xF2,0x02,0x99,0x00,0x0B,0xE3, 
0xC6,6,0xF2,0x02,0x98,0x00,0x76,0xE3, 
0xC6,5,0xF2,0x43,0x3E,0x08,0x00, 
0xC6,6,0xF2,0x02,0x9C,0x00,0x0B,0xE3, 
0xC6,6,0xF2,0x02,0x9B,0x00,0x76,0xE3, 
0xC6,5,0xF2,0x43,0x3F,0x08,0x00, 
0xC6,5,0xF2,0x43,0x40,0x08,0x00, 
0xC6,5,0xF2,0x43,0x41,0x08,0x00, 
0xC6,5,0xF2,0x43,0x42,0x08,0x00, 
0xC6,5,0xF2,0x43,0x43,0x08,0x00, 
0xC6,5,0xF2,0x43,0x44,0x08,0x00, 
0xC6,6,0xF2,0x02,0x9F,0x00,0x17,0xC7,  
0xC6,6,0xF2,0x02,0x9E,0x00,0xED,0xC6  
//Ready Initialising the Audio modules.............
//Found type: Radio Audio remium
//System fully initialised,waiting for User Event........
};
#endif
