#define ALOG_TAG "FlyCamera.cpp"
#include <utils/Log.h>
#include <utils/threads.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "QCameraHAL.h"
#include "QCameraHWI.h"
#include "FlyCamera.h"
#include <genlock.h>
#include <gralloc_priv.h>
#include <linux/msm_mdp.h>
#include <cutils/properties.h>
#if 1
#define DEBUGLOG LOGE
#else
#define DEBUGLOG do{}while(0)
#endif

namespace android
{

    typedef struct
    {
        unsigned char reg;
        unsigned char reg_val;
        bool sta;//true is find black line;
        bool flag;//true is neet again find the black line;
        bool this_is_first_open;//true is first open AND is open DVD
    } TW9912Info;

    typedef struct
    {
        unsigned int BlackCoordinateX;//??????????
        unsigned int BlackCoordinateY;
        unsigned int BlackFreamCount;//??x???,????
        bool ThisIsFirstFind;//????????,????,?????????,?????,????????????
        unsigned int BegingFindBlackFreamCount;//??????????
    } SplitScreenInfo;

    char video_channel_status[10] = "0";
    float FlyCameraflymFps = 0;

    static bool ToFindBlackLineAndSetTheTw9912VerticalDelayRegister_is_ok = false;
    static char video_format[10] = "1";
    static char video_show_status[10] = "0";
    static mm_camera_ch_data_buf_t *frame;
    static QCameraHardwareInterface *mHalCamCtrl;
    static unsigned int open_dev_fail_count = 0;
    static unsigned int global_fram_at_one_sec_count = 0;
    static unsigned int Longitudinal_last_fream_count = 9;
    static SplitScreenInfo LongitudinalInformationRemember = {300, 0, 0, true, 0};
    static bool global_need_rePreview = false;
    static pthread_t thread_DetermineImageSplitScreenID = NULL;
    static bool DetermineImageSplitScreen_do_not_or_yes = true;
    static int global_fram_count = 0;
    static nsecs_t Astern_last_time = 0;
    static nsecs_t DVDorAUX_last_time = 0;
    static bool globao_mFlyPreviewStatus;
    static unsigned int rePreview_count = 0;
    static bool global_Fream_is_first = true;
    static int flymFrameCount;
    static int flymLastFrameCount = 0;
    static nsecs_t flymLastFpsTime = 0;
    static nsecs_t flynow;
    static nsecs_t flydiff;
    static int global_tw9912_file_fd;
    static int global_fream_give_up = 0;
    void FlyCameraStar()
    {
        globao_mFlyPreviewStatus = 1;
        global_fream_give_up = 0;
        DEBUGLOG("Flyvideo-mFlyPreviewStatus =%d\n", globao_mFlyPreviewStatus);
        global_tw9912_file_fd = open("/dev/tw9912config", O_RDWR);
        if(global_tw9912_file_fd == -1)
        {
            DEBUGLOG("Flyvideo-:Error FlyCameraStar() tw9912config faild\n");
        }
    }
    void FlyCameraStop()
    {
        rePreview_count = 0;
        ToFindBlackLineAndSetTheTw9912VerticalDelayRegister_is_ok = false; //at next preview ,again run function
        open_dev_fail_count = 0;
        global_fram_at_one_sec_count = 0;
        LongitudinalInformationRemember.ThisIsFirstFind = true;
        LongitudinalInformationRemember.BegingFindBlackFreamCount = 0;
        DetermineImageSplitScreen_do_not_or_yes = true;//at next preview ,allow run function DetermineImageSplitScreen at astren
        globao_mFlyPreviewStatus = 0;
        global_Fream_is_first = true;
        close(global_tw9912_file_fd);
        DEBUGLOG("Flyvideo-mFlyPreviewStatus =%d\n", globao_mFlyPreviewStatus);
    }
    void FlyCameraRelease()
    {
        Longitudinal_last_fream_count = 9; //???????????
    }

    void FlyCameraGetInfo(mm_camera_ch_data_buf_t *Frame, QCameraHardwareInterface *mHalCamCtrl_1)
    {
        property_get("fly.video.show.status", video_show_status, "1"); // 1 is show normal, 0 is show black
        property_get("fly.video.channel.status", video_channel_status, "1"); //1:DVD 2:AUX 3:Astren
        property_get("tcc.fly.vin.pal", video_format, "1"); //pal_enabel
        /*
        tcc.fly.vin.pal:
        									DVD : 	NTSC	PAL
        														1		2
        						CAM or AUX: 	NTSC	PAL
        														3			4
        */
        frame = Frame;
        mHalCamCtrl = mHalCamCtrl_1;
        flymFrameCount++;
        flynow = systemTime();
        flydiff = flynow - flymLastFpsTime;
        if (flydiff > ms2ns(250))
        {
            FlyCameraflymFps =  ((flymFrameCount - flymLastFrameCount) * float(s2ns(1))) / flydiff;
            //DEBUGLOG("Preview Frames Per Second fly: %.4f", FlyCameraflymFps);
            flymLastFpsTime = flynow;
            flymLastFrameCount = flymFrameCount;
        }

    }
    static int ToFindBlackLineAndSetTheTw9912VerticalDelayRegister(mm_camera_ch_data_buf_t *frame);
    bool FlyCameraImageDownFindBlackLine()
    {
        if(video_channel_status[0] == '3' && ToFindBlackLineAndSetTheTw9912VerticalDelayRegister_is_ok == false)//onle at Astren and not PAL
        {
            if(ToFindBlackLineAndSetTheTw9912VerticalDelayRegister(frame) == 1)
            {
                DEBUGLOG("Flyvideo-??:??????");
                return 1;
            }
        }
        return 0;
    }
    void FlyCameraThisIsFirstOpenAtDVD()
    {
        if(video_channel_status[0] == '1' && global_Fream_is_first == true)
        {
            int file_fd;
            TW9912Info tw9912_info;
            file_fd = open("/dev/tw9912config", O_RDWR);
            if(file_fd == -1)
            {
                DEBUGLOG("Flyvideo-??,????????“/dev/tw9912config”??");
                // return -1;
                goto OPEN_ERR;
            }
            read(file_fd, (void *)(&tw9912_info), sizeof(TW9912Info));
            if(tw9912_info.this_is_first_open == true)
            {
                DEBUGLOG("Flyvideo-x:?????DVD,??preview\n");
                mHalCamCtrl->stopPreview();
                //      sleep(1);
                mHalCamCtrl->startPreview();
                //      sleep(1);
            }
            tw9912_info.this_is_first_open = false;
            write(file_fd, (const void *)(&tw9912_info), sizeof(TW9912Info));
            close(file_fd);
OPEN_ERR:
            global_Fream_is_first = false;
        }
    }
    void FlyCameraNotSignalAtLastTime()
    {
        if(video_show_status[0] == '0' && rePreview_count > 100)
        {
            //???? ? ??????????????
            DEBUGLOG("Flyvideo-??????,????????,stopPreview()-->\n");
            mHalCamCtrl->stopPreview();
            DEBUGLOG("Flyvideo-??????,????????,startPreview()-->\n");
            mHalCamCtrl->startPreview();
            rePreview_count = 0;
        }
        else if(video_show_status[0] == '0')
        {
            rePreview_count++;
            if(rePreview_count > 10000)rePreview_count = 101;
        }
    }
    void *CameraRestartPreviewThread(void *mHalCamCtrl1)
    {
        QCameraHardwareInterface *mHalCamCtrl = (QCameraHardwareInterface *)mHalCamCtrl1;
        usleep(1000 * 500); //500 ms
        if(globao_mFlyPreviewStatus == 1)
        {
            DEBUGLOG("Flyvideo-DetermineImageSplitScreen:stopPreview()-->\n");
            mHalCamCtrl->stopPreview();
            //	sleep(1);

            DEBUGLOG("Flyvideo-DetermineImageSplitScreen:startPreview()-->\n");
            mHalCamCtrl->startPreview();
            //	sleep(1);
        }
        else
        {
            DEBUGLOG("Flyvideo-????????preview,????HAL???stop?\n");
        }

        LongitudinalInformationRemember.ThisIsFirstFind = true;
        global_need_rePreview = false;
        return NULL;
    }

    static bool ToFindBlackLine(mm_camera_ch_data_buf_t *frame)
    {
        unsigned char *piont_y, *piont_y_last;
        unsigned int i, j, i_last = 0;
        unsigned int black_count = 0;
        for(i = 0; i < 719; i++) //???
        {
            piont_y = (unsigned char *)(frame->def.frame->buffer + frame->def.frame->y_off + i); //???
            if(!i) piont_y_last = piont_y;

            //if( (*piont_y) == (*piont_y_last) && (*piont_y) < 0x25)//TW9912 ????? 0x20
            //if( (*piont_y) == (*piont_y_last) && (*piont_y) < 0x2f)//TW9912 ????? 0x20
            if((*piont_y) >= 0x20 && (*piont_y) <= 0x35 )
            {
                black_count++;
                if( (719 - i + black_count) < 700) //??????700?????,?????????
                    return 0;
                if( black_count > 700)//“??"????700???????
                {
                    //FlagBlack(frame);
                    DEBUGLOG("Flyvideo-:??????,????,?????????? = %d ?,?????????0x%.2x", black_count, *piont_y);
                    return 1;
                }
            }
            //else return 0;
            i_last = i;
            piont_y_last = piont_y;
        }
        return 0;
    }
    /*
    static void FlagBlack(mm_camera_ch_data_buf_t *frame)//????
    {
    int i,j;
    static int NumberFlag=0;
    for(j=100;j<120;j++)
    	for(i=NumberFlag*100;i<=(NumberFlag*100+20);i+=4)
    	{
    	*(unsigned long *)(frame->def.frame->buffer+frame->def.frame->y_off+j*720+i) |=0xffffffff;
    	}
    NumberFlag++;
    if(NumberFlag>7)NumberFlag=0;
    }
    */
    static int ToFindBlackLineAndSetTheTw9912VerticalDelayRegister(mm_camera_ch_data_buf_t *frame)
    {
        int file_fd;
        TW9912Info tw9912_info;
        file_fd = open("/dev/tw9912config", O_RDWR);
        if(file_fd == -1)
        {
            if(open_dev_fail_count == 0)
            {
                DEBUGLOG("Flyvideo-??x,????????“/dev/tw9912config”??");
                //ToFindBlackLineAndSetTheTw9912VerticalDelayRegister_is_ok = true;
                DEBUGLOG("Flyvideo-??:??!!????/dev/tw9912config????,????????,???????,???????????(>606)");
                DetermineImageSplitScreen_do_not_or_yes = false;
                open_dev_fail_count ++;
                goto OPEN_ERR;
                //return -1;
            }
            else if( open_dev_fail_count > 60 )
            {
                open_dev_fail_count = 1;
                if(file_fd == -1)
                {
                    DEBUGLOG("Flyvideo-??`,????????“/dev/tw9912config”??");
                    //ToFindBlackLineAndSetTheTw9912VerticalDelayRegister_is_ok = true;
                    DEBUGLOG("Flyvideo-??:??!!????/dev/tw9912config????,????????,???????,???????????(>606)");
                    DetermineImageSplitScreen_do_not_or_yes = false;
                    goto OPEN_ERR;
                    //return -1;
                }
            }
            else
            {
                open_dev_fail_count ++;
                goto OPEN_ERR;
            }
        }

        read(file_fd, (void *)(&tw9912_info), sizeof(TW9912Info));
        if(tw9912_info.flag == true)
        {
            if(ToFindBlackLine(frame) == 1) //????????
            {
                tw9912_info.sta = true;
                tw9912_info.flag = false;
                write(file_fd, (const void *)(&tw9912_info), sizeof(TW9912Info));
                DEBUGLOG("Flyvideo-????????,???????????");
                ToFindBlackLineAndSetTheTw9912VerticalDelayRegister_is_ok = true;
                DetermineImageSplitScreen_do_not_or_yes = true;
            }
            else//????????? ??????
            {
                if(tw9912_info.reg_val > 0x10 && tw9912_info.reg_val <= 0x17)
                {
                    tw9912_info.reg = 0x08;
                    tw9912_info.reg_val -= 1;
                    write(file_fd, (const void *)(&tw9912_info), sizeof(TW9912Info));
                    DEBUGLOG("Flyvideo-?????????,?????????");
                }
                else
                {
                    DEBUGLOG("Flyvideo-??:?????????????,?????????????????,???????");
                    ToFindBlackLineAndSetTheTw9912VerticalDelayRegister_is_ok = true;
                    DEBUGLOG("Flyvideo-??:??!!?????????????,???????");
                    DetermineImageSplitScreen_do_not_or_yes = false;
                }
                return 1;
            }

        }
        else
        {
            DEBUGLOG("Flyvideo-???????? tw9912_info.flag == false ???“?”?????");
            ToFindBlackLineAndSetTheTw9912VerticalDelayRegister_is_ok = true;
        }
        close(file_fd);
OPEN_ERR:
        return 0;
    }
    static bool VideoItselfBlackJudge(mm_camera_ch_data_buf_t *frame)
    {
        unsigned int black_count;
        int i = 0 , j = 0;
        unsigned char *piont_y;
        for(; j < 480; j++)
        {
            i++;
            piont_y = (unsigned char *)(frame->def.frame->buffer + frame->def.frame->y_off + 720 * j + i); //?? ??? ????
            if(*piont_y <= 0x20)//????????
                black_count ++;

            if( (480 - j + black_count) < 450)//??????470?????,?????????
                continue;

            if(black_count > 450)
            {
                DEBUGLOG("Flyvideo-:?");
                return 1;//???????
            }
        }
        return 0;
    }
    /*
    static bool BlackJudge(mm_camera_ch_data_buf_t *frame)
    {
    unsigned char *piont_y,*piont_y_last;
    unsigned int i,j,i_last=0;
    unsigned int black_count=0;
    //FlagBlack(frame);
    //????
    //	for(j=477;j<479;j++)//???
    //	{
    		for(i=0;i<719;i++)//???
    		{
    				piont_y = (unsigned char *)(frame->def.frame->buffer+frame->def.frame->y_off+475*720+ i);
    				//piont_y_last = (unsigned char *)(frame->def.frame->buffer+frame->def.frame->y_off+j*720+ i_last);
    				if(!i) piont_y_last = piont_y;

    				//if( (*piont_y) == (*piont_y_last) && (*piont_y) < 0x25)//TW9912 ????? 0x20
    				if( (*piont_y) == (*piont_y_last) && (*piont_y) < 0x2f)//TW9912 ????? 0x20
    				{
    					black_count++;
    					if( (719-i+black_count)< 280)//??????700?????,?????????
    					return 0;
    					if( black_count > 280)//“??"????700?????
    					{
    						//FlagBlack(frame);
    						DEBUGLOG("Flyvideo-:?????????,?????????= %d ?,?????????0x%.2x",black_count,*piont_y);
    						return 1;
    					}
    				}
    				//else return 0;
    				i_last =i;
    				piont_y_last = piont_y;
    		}
    	//}
    return 0;
    }
    */
    static bool RowsOfDataTraversingTheFrameToFindTheBlackLineForDVDorAUX(mm_camera_ch_data_buf_t *frame)
    {
        unsigned char *piont_y;
        int i = 0, jj = 0;
        unsigned int count;
        unsigned int find_line = 477;//NTSC
        if(video_format[0] == '4' || video_format[0] == '2')
        {
            DEBUGLOG("Flyvideo-:?????:PAL,?????:9~562");
            find_line = 562;//PAL
        }
        else DEBUGLOG("Flyvideo-:?????:NTSC,?????:9~477");
        DEBUGLOG("Flyvideo-??????,????????");
        for(i = 9; i < find_line; i++) //???????,? ??? ????????;?10???10????,??????3???????
        {
            piont_y = (unsigned char *)(frame->def.frame->buffer + frame->def.frame->y_off + 720 * i + 300); //??300????????
            if(*piont_y <= 0x43)//??,???????,???????,???????,??700??,?????????????
                //if((*piont_y) >= 0x20 && (*piont_y) <= 0x35 )
            {
                for(jj = 0; jj < 719; jj++) //??????
                {
                    piont_y = (unsigned char *)(frame->def.frame->buffer + frame->def.frame->y_off + 720 * i + jj);
                    if(*piont_y <= 0x43)
                        //if((*piont_y) >= 0x20 && (*piont_y) <= 0x35 )
                        count++;//??
                    if( (719 - jj + count) < 715) //??????700?????,?????????
                    {
                        //?????(715-jj)??????(count)????700,?????
                        goto Break_The_For;
                    }
                }
Break_The_For:
                if(count >= 715) goto Break_The_Func;//???????????700?,??????,???????
                else count = 0;
            }
        }
        //DEBUGLOG("Astern:????????");
        DEBUGLOG("Flyvideo-?????");
        return 0;
Break_The_Func:
        DEBUGLOG("Flyvideo-:???????,?????? %d ?,?????????= %d ?,?????????0x%.2x", i, count, *piont_y);
        return 1;//??????????
    }
    static bool RowsOfDataTraversingTheFrameToFindTheBlackLine(mm_camera_ch_data_buf_t *frame)
    {
        unsigned char *piont_y;
        int i = 0, jj = 0;
        unsigned int count;
        unsigned int find_line = 477;//NTSC
        if(video_format[0] == '4' || video_format[0] == '2')
        {
            DEBUGLOG("Flyvideo-:?????:PAL,?????:9~562");
            find_line = 562;//PAL
        }
        else DEBUGLOG("Flyvideo-:?????:NTSC,?????:9~477");
        DEBUGLOG("Flyvideo-??????,????????");
        for(i = 9; i < find_line; i++) //???????,? ??? ????????;?10???10????,??????3???????
        {
            piont_y = (unsigned char *)(frame->def.frame->buffer + frame->def.frame->y_off + 720 * i + 300); //??300????????
            //if(*piont_y <= 0x20)//??,???????,???????,???????,??700??,?????????????
            if((*piont_y) >= 0x20 && (*piont_y) <= 0x35 )
            {
                for(jj = 0; jj < 719; jj++) //??????
                {
                    piont_y = (unsigned char *)(frame->def.frame->buffer + frame->def.frame->y_off + 720 * i + jj);
                    //if(*piont_y <= 0x20)
                    if((*piont_y) >= 0x20 && (*piont_y) <= 0x35 )
                        count++;//??
                    if( (719 - jj + count) < 715) //??????700?????,?????????
                    {
                        //?????(715-jj)??????(count)????700,?????
                        goto Break_The_For;
                    }
                }
Break_The_For:
                if(count >= 715) goto Break_The_Func;//???????????700?,??????,???????
                else count = 0;
            }
        }
        //DEBUGLOG("Astern:????????");
        DEBUGLOG("Flyvideo-?????");
        return 0;
Break_The_Func:
        DEBUGLOG("Flyvideo-:???????,?????? %d ?,?????????= %d ?,?????????0x%.2x", i, count, *piont_y);
        return 1;//??????????
    }
    //Determine whether the split-screen
    static bool DetermineImageSplitScreen_Longitudinal(mm_camera_ch_data_buf_t *frame, QCameraHardwareInterface *mHalCamCtrl)
    {
        int i = 0 , j = 30, j_end = 690;
        unsigned char *piont_y;
        unsigned int dete_count = 0;
        Longitudinal_last_fream_count ++;
        if(Longitudinal_last_fream_count > 0xfffe) Longitudinal_last_fream_count = 9;
        if( Longitudinal_last_fream_count < 10)//????,??????,?????preview??
        {
            //DEBUGLOG("Flyvideo-:T");
            return 0;
        }
        if(LongitudinalInformationRemember.ThisIsFirstFind == false)
        {
            //??5???????,????????
            LongitudinalInformationRemember.BegingFindBlackFreamCount++;
            if(LongitudinalInformationRemember.BegingFindBlackFreamCount > 6 || (6 - LongitudinalInformationRemember.BegingFindBlackFreamCount + LongitudinalInformationRemember.BlackFreamCount ) < 5 ) //???????,?? ???????????10?
            {
                DEBUGLOG("Flyvideo-:????????");
                LongitudinalInformationRemember.BegingFindBlackFreamCount = 0;
                LongitudinalInformationRemember.ThisIsFirstFind = true;
            }
        }
        if(LongitudinalInformationRemember.ThisIsFirstFind == false)//??????????,?????????????
        {
            j = LongitudinalInformationRemember.BlackCoordinateX;
            j_end = j;
        }
        for(; j <= j_end; j++)
        {
            piont_y = (unsigned char *)(frame->def.frame->buffer + frame->def.frame->y_off + 720 * 300 + j); //??300?,????
            if(*piont_y <= 0x20)//????????
            {
                for(i = 10; i < 470; i++)
                {
                    piont_y = (unsigned char *)(frame->def.frame->buffer + frame->def.frame->y_off + 720 * i + j);
                    if((*piont_y) <= 0x20)
                    {
                        dete_count++;
                        if( (460 - (i - 10) + dete_count) < 455) //??????400?????,?????????
                        {
                            dete_count = 0;
                            goto THE_FOR;//??????????????
                        }
                        if(dete_count > 454 && VideoItselfBlackJudge(frame) == 1)//??????????????
                            return 0;//?????
                        if(dete_count > 455)//?????????????????,??????????
                        {
                            dete_count = 0;
                            //LOGE("DetermineImageSplitScreen:buf[%d]= 0x%.2x\n",((180*j) + i),(*piont_crcb) );
                            if(LongitudinalInformationRemember.ThisIsFirstFind == true)//???????????
                            {
                                LongitudinalInformationRemember.ThisIsFirstFind = false;//??????
                                LongitudinalInformationRemember.BlackCoordinateX = j;//??????
                                LongitudinalInformationRemember.BlackFreamCount = 1;
                            }
                            else
                            {
                                LongitudinalInformationRemember.BlackFreamCount ++;
                            }
                            DEBUGLOG("Flyvideo-:?%d??,???=%d\n", j, LongitudinalInformationRemember.BlackFreamCount);
                            if(LongitudinalInformationRemember.BlackFreamCount  > 250) //5sec
                            {
                                global_need_rePreview = false;//??CameraRestartPreviewThread????,global_need_rePreview?????true;
                                LongitudinalInformationRemember.ThisIsFirstFind = true;
                            }
                            if(global_need_rePreview == false )
                            {
                                if(LongitudinalInformationRemember.BlackFreamCount >= 5) //?????15??? 14 ????preview
                                {
                                    Longitudinal_last_fream_count = 0;//??????
                                    global_need_rePreview = true;
                                    LongitudinalInformationRemember.BlackFreamCount = 0;
#if 0
                                    DEBUGLOG("Flyvideo-:????,stopPreview()-->\n");
                                    mHalCamCtrl->stopPreview();
                                    DEBUGLOG("Flyvideo-:????,startPreview()-->\n");
                                    mHalCamCtrl->startPreview();
#else
                                    if( pthread_create(&thread_DetermineImageSplitScreenID, NULL, CameraRestartPreviewThread, (void *)mHalCamCtrl) != 0)
                                        DEBUGLOG("Flyvideo-:????,??????????!\n");
                                    else
                                        DEBUGLOG("Flyvideo-:????,??????????!\n");
#endif
                                    global_need_rePreview = false;
                                    LongitudinalInformationRemember.ThisIsFirstFind = true;
                                    LongitudinalInformationRemember.BegingFindBlackFreamCount = 0;
                                    return 1;//??????
                                }
                            }
                            goto BREAK_THE;
                        }
                    }

                }
THE_FOR:
                ;
            }
        }
BREAK_THE:
        return 0;//?????
    }
    static bool DetermineImageSplitScreen(mm_camera_ch_data_buf_t *frame, QCameraHardwareInterface *mHalCamCtrl)
    {
        int i = 0 , j = 0;
        //static char video_channel_status[10]="1"; //1:DVD 2:AUX 3:Astren
        unsigned char *piont_y;
        unsigned char *piont_crcb;
        unsigned int dete_count = 0;



        if(FlyCameraflymFps < 24)
        {
            DEBUGLOG("Flyvideo-x:flymFps = %f", FlyCameraflymFps);
            return 0;
        }
        if(++global_fram_at_one_sec_count < 10)//??????????,??20?????,
        {
            return 0;
        }
        else
            global_fram_at_one_sec_count = 0;
        //??????
        if(DetermineImageSplitScreen_Longitudinal(frame, mHalCamCtrl) == 1)
            return 1;

        // property_get("fly.video.channel.status",video_channel_status,"1");//1:DVD 2:AUX 3:Astren

        /****************************Astren******************************/
        if(video_channel_status[0] == '3')//Astren
            //if(0)
        {
            //DEBUGLOG("DetermineImageSplitScreen:Astren");
            if(DetermineImageSplitScreen_do_not_or_yes == false)
            {
                return 0;//?????
            }
            else
            {
                for(j = 0; j < 4; j++)
                {
                    for(i = 0; i < 20; i++)
                    {
                        piont_y = (unsigned char *)(frame->def.frame->buffer + frame->def.frame->y_off + 180 * j + i);
                        if((*piont_y) < 0x20 || (*piont_y) > 0x35 )
                        {
                            dete_count++;//??????10????????????
                            if(dete_count > 5)
                            {
                                //DEBUGLOG("Flyvideo-????:buf[%d]= 0x%.2x\n",((180*j) + i),(*piont_y) );
                                //DEBUGLOG("DetermineImageSplitScreen:buf[%d]= 0x%.2x\n",((180*j) + i),(*piont_crcb) );
                                global_fram_count ++;//????????
                                if(global_fram_count > 50) //1sec
                                {
                                    global_need_rePreview = false;//??CameraRestartPreviewThread????,global_need_rePreview?????true;
                                    global_fram_count = 0;
                                }
                                if(global_need_rePreview == false && (flynow - Astern_last_time) > ms2ns(2500))//???????2.5s
                                {
                                    Astern_last_time = flynow;
                                    if(global_fram_count >= 1 && RowsOfDataTraversingTheFrameToFindTheBlackLine(frame)) //????????preview
                                    {
                                        global_fram_count = 0;
                                        global_need_rePreview = true;
                                        //DEBUGLOG("Flyvideo-: need rePreview\n");

                                        if( pthread_create(&thread_DetermineImageSplitScreenID, NULL, CameraRestartPreviewThread, (void *)mHalCamCtrl) != 0)
                                            DEBUGLOG("Flyvideo-????,??????????!Astren\n");
                                        else
                                            DEBUGLOG("Flyvideo-????,??????????!Astren\n");
                                        return 1;//??????
                                    }
                                }
                                goto BREAK_THE;
                            }
                        }
                    }
                }
            }
        }
        /****************************DVD or AUX******************************/
        else
        {
            //DEBUGLOG("DetermineImageSplitScreen:DVD or AUX");
            for(j = 0; j < 4; j++)
            {
                for(i = 0; i < 20; i++)
                {
                    piont_crcb = (unsigned char *)(frame->def.frame->buffer + frame->def.frame->cbcr_off + (180 * j + i));
                    if((*piont_crcb) < (0x7f - 3) || (*piont_crcb) > (0x80 + 3) ) //?????????????
                    {
                        //DEBUGLOG("Flyvideo-..");
                        dete_count++;//??????10????????????
                        if(dete_count > 9)
                        {
                            DEBUGLOG("Flyvideo-??????????????");
                            //DEBUGLOG("DetermineImageSplitScreen:buf[%d]= 0x%.2x\n",((180*j) + i),(*piont_crcb) );
                            global_fram_count ++;//????????
                            if(global_fram_count > 50) //1sec
                            {
                                global_need_rePreview = false;//??CameraRestartPreviewThread????,global_need_rePreview?????true;
                                global_fram_count = 0;
                            }
                            if(global_need_rePreview == false)//???????10s
                            {
                                if(global_fram_count >= 1 && RowsOfDataTraversingTheFrameToFindTheBlackLineForDVDorAUX(frame)) //????????preview ,???????
                                {
                                    global_fram_count = 0;
                                    global_need_rePreview = true;
                                    //DEBUGLOG("Flyvideo-: need rePreview\n");

                                    if( pthread_create(&thread_DetermineImageSplitScreenID, NULL, CameraRestartPreviewThread, (void *)mHalCamCtrl) != 0)
                                        DEBUGLOG("Flyvideo-????,??????????!DVD or AUX\n");
                                    else
                                        DEBUGLOG("Flyvideo-????,??????????!DVD or AUX\n");
                                    return 1;//??????
                                }
                            }
                            goto BREAK_THE;
                        }
                    }

                }

            }
        }
BREAK_THE:

        return 0;//?????
    }
    static int FlyCameraReadTw9912StatusRegitsterValue()
    {
        int file_fd;
        unsigned char value, value_1;
        int arg = 0;
        unsigned int cmd;
        if(global_tw9912_file_fd == -1)
            return 0;
        cmd = COPY_TW9912_STATUS_REGISTER_0X01_4USER;
        if (ioctl(global_tw9912_file_fd, cmd, &arg) < 0)
        {
            DEBUGLOG("Flyvideo-: Call cmd COPY_TW9912_STATUS_REGISTER_0X01_4USER fail\n");
            close(file_fd);
            return 0;
        }
        read(global_tw9912_file_fd, (void *)(&value), sizeof(unsigned char));
        //DEBUGLOG("Flyvideo-:0x%.2x\n",value);

        value_1 = value & 0x68;
        if(value_1 != 0x68)
        {
            //global_fream_give_up ++ ;
            if(global_fream_give_up < 10)
            {
                DEBUGLOG("Flyvideo-:Vedio singnal bad\n");
                //memset((void *)(frame->def.frame->buffer+frame->def.frame->y_off),0,720*480);
                //memset((void *)(frame->def.frame->buffer+frame->def.frame->cbcr_off),0,720*480);
                return 1;
            }
            else
            {
                // hope 10 fream ago signal is good
                global_fream_give_up = 0;
                return 0;
            }
        }
        return 0;
    }
    static void FlyCameraAuxBlackLine(void)
    {
        if(video_channel_status[0] == '2')//AUX
        {
            memset((void *)(frame->def.frame->buffer + frame->def.frame->y_off), 0x1e, 720 * 3); //????
            memset((void *)(frame->def.frame->buffer + frame->def.frame->cbcr_off), 0x7f, 720 * 2);

            if(video_format[0] == '4' || video_format[0] == '2')
                ;//pal
            else
            {
                memset((void *)(frame->def.frame->buffer + frame->def.frame->cbcr_off + 720 * 237), 0x80, 720 * 4); //??3?
                memset((void *)(frame->def.frame->buffer + frame->def.frame->y_off + 720 * 475), 0x1e, 720 * 5);
            }
        }
    }
    bool FlyCameraFrameDisplayOrOutDisplay()
    {
        /*bool ret;
        		if(video_channel_status[0] != '1')//is not DVD
        		{
        			ret = FlyCameraReadTw9912StatusRegitsterValue();//1:DVD 2:AUX 3:Astren
        			if(ret == 1) return 1;
        		}
        */

        if(video_channel_status[0] != '3')
        {
            //??????????????
            if( DetermineImageSplitScreen(frame, mHalCamCtrl) ) //????????????
            {
                FlyCameraAuxBlackLine();
                return 1;
            }
            else
                FlyCameraAuxBlackLine();
        }
        else//Astren
        {
            //if(BlackJudge(frame) == 1)//????
            //return processPreviewFrameWithOutDisplay(frame);
            //	DEBUGLOG("Flyvideo-:????,???????????\n");
            //else
            //	{
            if( DetermineImageSplitScreen(frame, mHalCamCtrl) ) //????
                return 1;
            else
                return 0;
            //	}
            //return 0;
        }
        return 0;
    }
}
