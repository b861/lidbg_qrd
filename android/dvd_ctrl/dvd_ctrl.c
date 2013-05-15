#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

/*******************************************
 *  获得端口名称
********************************************/
#define TTY_DEV "/dev/tcc-uart3"
#define BAUD (B57600)

int fd, len = 0;

void DVDCmdPrintf(unsigned char *buf, unsigned int len)
{
    unsigned char sendBuf[256];
    unsigned char checksum;
    unsigned int i;
    //	DWORD bufSendLength;


    sendBuf[0] = 0xFF;
    sendBuf[1] = 0x55;
    sendBuf[2] = len;
    checksum = len;
    for(i = 0; i < len; i++)
    {
        sendBuf[3+i] = buf[i];
        checksum += buf[i];
    }
    sendBuf[3+i] = 0xFF - checksum;
    //WriteFile(pForyouDVDInfo->hDVDComm,sendBuf,len+4,&bufSendLength,NULL);
    //SerialSendData(sendBuf, len + 4);
    len = write(fd, sendBuf, len + 4);
}

void control_DVD_IR_CMD(unsigned char IRCMD)
{
    unsigned char buff[] = {0x6B, 0x00};
    buff[1] = IRCMD;
    DVDCmdPrintf(buff, 2);
}



void DVD_VIDEO_SETUP(void)
{
    unsigned char buff[] = {0x03, 0x10, 0x00, 0x00, 0x14, 20, 16, 9, 9};
    //cmd ,yuv+NTSC,16:9,full screen,standard,contrast,brightness,hue,saturation

    DVDCmdPrintf(buff, 9);
}




void DVD_PAL_NTSC(int tmp)
{

    unsigned char buff[] = {0x03, 0x10, 0x00, 0x00, 0x14, 20, 16, 9, 9};
    //cmd ,yuv+NTSC,16:9,full screen,standard,contrast,brightness,hue,saturation

    // CVBS + PAL
    if(tmp == 0)
        buff[1] = (0x0 << 4) | 0x1;
    // CVBS + NTSC
    else if(tmp == 1)
        buff[1] = (0x0 << 4) | 0x0;
    // CVBS + Auto
    else if(tmp == 2)
        buff[1] = (0x0 << 4) | 0x2;

    DVDCmdPrintf(buff, 9);

}






int main(int argc , char **argv)
{

    unsigned char buff[1024];
    struct termios oldtio;
    unsigned int port, baud;
    char *ptty;

    //printf("Uart Init -16-\n");

    fd = open( TTY_DEV, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == (-1))
    {
        perror("Can't Open Uart\n");
        return 0;
    }
    //else
    //	printf("Open Uart Success !\n");


    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("tcgetattr fail!\n");
        return 0;
    }

    cfsetispeed(&oldtio, BAUD);
    cfsetospeed(&oldtio, BAUD);

    oldtio.c_cflag |= CS8;//8 bit
    oldtio.c_cflag &= ~CSTOPB;// 1 stop
    oldtio.c_cflag &= ~PARENB;//N
    oldtio.c_lflag &= ~(ICANON | ECHO | ECHOE);//


    tcflush(fd, TCIFLUSH);
    if((tcsetattr(fd, TCSANOW, &oldtio)) != 0)
    {
        perror("tcsetattr fail!\n");
        return 0;
    }

    printf("DVD CMD = %s\n", argv[1]);

    /*

    0x00 => IR_NUM0（ 数字0）
    0x01 => IR_NUM1（ 数字1）
    0x02 => IR_NUM2（ 数字2）
    0x03 => IR_NUM3（ 数字3）
    0x04 => IR_NUM4（ 数字4）
    0x05 => IR_NUM5（ 数字5）

    0x06 => IR_NUM6（ 数字6）
    0x07 => IR_NUM7（ 数字7）

    0x08 => IR_NUM8（ 数字8）
    0x09 => IR_NUM9（ 数字9）
    0x0a => IR_ADD10（保留，不使用）
    0x0b => IR_ADD20（保留，不使用）
    0x0c => IR_ADD5 （保留，不使用）
    0x0d => IR_UP(方向上)

    0x0e => IR_LEFT(方向左)
    0x0f => IR_RIGHT(方向右)
    0x10 => IR_DOWN(方向下)
    0x11 => IR_ENTER（确认键）
    0x12 => IR_PAGE_UP（保留，不使用）
    0x13 => IR_PAGE_DOWN（保留，不使用）
    0x14 => IR_PLAY（播放键）
    0x15 => IR_PAUSE（暂停键）
    0x17 => IR_STOP（停止键）
    0x18 => IR_NEXT（上一曲）
    0x19 => IR_PREV（下一曲）

    0x1a => IR_REPEAT（重复键）
    0x1f => IR_INTRO（浏览键）
    0x20 => IR_RESUME（重新播放）
    0x21 => IR_FF（快进）
    0x27 => IR_FR（快退）
    0x2d => IR_SF（慢进）
    0x33 => IR_SR（慢退）
    0x39 => IR_STEPF（步进）
    0x3a => IR_STEPR
    0x3c => IR_SET_A（AB重复）
    0x3e => IR_TITLE_MENU（保留，不使用）
    0x3f => IR_ROOT_MENU（返回/退出DVD Menu）
    0x40 => IR_RETURN
    0x44 => IR_AUDIO（音频切换）
    0x45 => IR_SUB_TITLE（字幕切换）
    0x47 => IR_ANGLE（角度切换）
    0x48 => IR_DISPLAY（显示播放信息）
    0x49 => IR_REMAIN（保留，不使用）
    0x4a => IR_GOTO（搜索键）
    0x4b => IR_MARK（书签键）
    0x4c => IR_CLEAR（清除）
    0x4d => IR_PBC（VCD PBC On/Off）
    0x4e => IR_DIGEST
    0x4f => IR_PROGRAM（编程键）
    0x50 => IR_RANDOM（无效）

    0x52 => IR_MEMORY（记忆）
    0x53 => IR_PLAY_MODE（播放模式切换）
    0x54 => IR_PAL_NTSC（N/P切换）
    0x55 => IR_TV_MODE

    0x59 => IR_ZOOM_IN
    0x5a => IR_ZOOM_OUT（放大）
    0x5c => IR_CAPTURE（保留，不使用）
    0x60 => IR_SETUP（设置）
    0x61 => IR_SURROUND（保留，不使用）
    0x62 => UART_IR_PM_NORMAL
    0x63 => UART_IR_PM_SHUFFLE
    0x64 => UART_IR_PM_RANDOM
    0x65 => UART_IR_PM_INTRO
    0x66 => IR_EQUALIZER（保留，不使用）
    0x67 => IR_REPEAT_OFF
    0x68 => IR_REPEAT_SINGLE
    0x69 => IR_REPEAT_FOLDER
    0x6a => IR_REPEAT_ALL
    0x6b => IR_MUTE（静音）
    0x6c => IR_VOLUME_UP（音量＋）
    0x6d => IR_VOLUME_DOWN（音量－）
    0x75 => IR_FUNCTION（保留，不使用）
    0x79 => IR_POWER（保留，不使用）
    0x7c => IR_EJECT（进/出仓）
    0x7e => IR_AUDIO_ONLY（保留，不使用）
    0x95 => IR_DISC	（碟片）
    0x96 => IR_USB	（U盘）
    0x97 => IR_SD		（卡）
    */

    ///////////////////////////////////////////
    if(!strcmp(argv[1] , "out"))
    {

        control_DVD_IR_CMD(0x7c);
    }
    else if(!strcmp(argv[1] , "pre"))
    {

        control_DVD_IR_CMD(0x19);
    }
    else if(!strcmp(argv[1] , "next"))
    {

        control_DVD_IR_CMD(0x18);
    }
    else if(!strcmp(argv[1] , "play"))
    {

        control_DVD_IR_CMD(0x14);
    }
    else if(!strcmp(argv[1] , "pause"))
    {

        control_DVD_IR_CMD(0x15);
    }
    else if(!strcmp(argv[1] , "stop"))
    {

        control_DVD_IR_CMD(0x17);
    }

    else if(!strcmp(argv[1] , "up"))
    {

        control_DVD_IR_CMD(0x0d);
    }
    else if(!strcmp(argv[1] , "left"))
    {

        control_DVD_IR_CMD(0x0e);
    }
    else if(!strcmp(argv[1] , "right"))
    {

        control_DVD_IR_CMD(0x0f);
    }
    else if(!strcmp(argv[1] , "down"))
    {

        control_DVD_IR_CMD(0x10);
    }
    else if(!strcmp(argv[1] , "enter"))
    {

        control_DVD_IR_CMD(0x11);
    }
    else if(!strcmp(argv[1] , "menu"))
    {

        control_DVD_IR_CMD(0x3f);
    }
    else if(!strcmp(argv[1] , "ff"))
    {

        control_DVD_IR_CMD(0x21);
    }
    else if(!strcmp(argv[1] , "fr"))
    {

        control_DVD_IR_CMD(0x27);
    }
    else if(!strcmp(argv[1] , "pal"))
    {
        DVD_PAL_NTSC(0);
    }
    else if(!strcmp(argv[1] , "ntsc"))
    {
        DVD_PAL_NTSC(1);
    }


    //////////////////////////////////////////
    close(fd);
    return 0;
}
