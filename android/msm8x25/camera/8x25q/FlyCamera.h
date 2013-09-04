#ifndef FLYCAMERA_HAL_H
#define FLYCAMERA_HAL_H
namespace android {
extern void FlyCameraStar();
extern void FlyCameraStop();
extern void FlyCameraRelease();
extern void FlyCameraGetInfo(mm_camera_ch_data_buf_t *Frame,QCameraHardwareInterface *mHalCamCtrl_1);
extern void FlyCameraNotSignalAtLastTime();
extern void FlyCameraThisIsFirstOpenAtDVD();
extern bool FlyCameraImageDownFindBlackLine();
extern bool FlyCameraFrameDisplayOrOutDisplay();

extern float FlyCameraflymFps;
extern char video_channel_status[10];
}; // namespace android

#endif
