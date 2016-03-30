#ifndef __LIDBG_FLYCAM_PAR_
#define __LIDBG_FLYCAM_PAR_

#define REARVIEW_ID	0
#define DVR_ID				1
#define REAR_BLOCK_ID_MODE				2
#define DVR_BLOCK_ID_MODE				3
#define REAR_GET_RES_ID_MODE				4
#define DVR_GET_RES_ID_MODE				5

typedef enum {
  NR_BITRATE,
  NR_RESOLUTION,
  NR_PATH,
  NR_TIME,
  NR_FILENUM,
  NR_TOTALSIZE,
  NR_START_REC,
  NR_STOP_REC,
  NR_SET_PAR,
  NR_GET_RES,
  NR_SATURATION,
  NR_TONE,
  NR_BRIGHT,
  NR_CONTRAST,
}cam_ctrl_t;

typedef enum {
  NR_STATUS,
  NR_ACCON_CAM_READY,
  NR_DVR_FW_VERSION,
  NR_REAR_FW_VERSION,
  NR_DVR_RES,
  NR_REAR_RES,
  NR_ONLINE_NOTIFY,
  NR_ONLINE_INVOKE_NOTIFY,
}status_ctrl_t;

typedef enum {
  NR_VERSION,
  NR_UPDATE,
}fw_ctrl_t;

#define FLYCAM_FRONT_REC_IOC_MAGIC  'F'
#define FLYCAM_FRONT_ONLINE_IOC_MAGIC  'f'
#define FLYCAM_BACK_REC_IOC_MAGIC  'B'
#define FLYCAM_BACK_ONLINE_IOC_MAGIC  'b'
#define FLYCAM_STATUS_IOC_MAGIC  's'
#define FLYCAM_FW_IOC_MAGIC  'w'

typedef enum {
  RET_SUCCESS,
  RET_NOTVALID,
  RET_NOTSONIX,
  RET_FAIL,
  RET_IGNORE,
  RET_REPEATREQ,
}cam_ioctl_ret_t;

typedef enum {
  RET_ONLINE_INSUFFICIENT_SPACE_STOP = 1,
  RET_ONLINE_DISCONNECT,
  RET_ONLINE_FOUND_SONIX,
  RET_ONLINE_FOUND_NOTSONIX,
}onlineNotify_ret_t;

typedef enum {
  RET_DEFALUT,
  RET_START,
  RET_STOP,
  RET_EXCEED_UPPER_LIMIT,
  RET_DISCONNECT,
  RET_INSUFFICIENT_SPACE_CIRC,
  RET_INSUFFICIENT_SPACE_STOP,
  RET_INIT_INSUFFICIENT_SPACE_STOP,
  RET_DVR_SONIX,
  RET_DVR_NOT_SONIX,
  RET_REAR_SONIX,
  RET_REAR_DISCONNECT,
  RET_REAR_NOT_SONIX,
  RET_DVR_UD_SUCCESS,
  RET_DVR_UD_FAIL,
  RET_DVR_FW_ACCESS_FAIL,
  RET_REAR_UD_SUCCESS,
  RET_REAR_UD_FAIL,
  RET_REAR_FW_ACCESS_FAIL,
}cam_read_ret_t;

//for hub
#define	UDISK_NODE		"1-1.1"
#define	FRONT_NODE		"1-1.2"	
#define	BACK_NODE		"1-1.3"

#endif
