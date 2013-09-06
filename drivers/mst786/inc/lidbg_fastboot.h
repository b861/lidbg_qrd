#ifndef __LIDBG_FASTBOOT_
#define __LIDBG_FASTBOOT_

typedef enum
{
    PM_STATUS_EARLY_SUSPEND_PENDING,
    PM_STATUS_SUSPEND_PENDING,
    PM_STATUS_RESUME_OK,
    PM_STATUS_LATE_RESUME_OK,
    PM_STATUS_READY_TO_PWROFF,
    PM_STATUS_READY_TO_FAKE_PWROFF,
    
} LIDBG_FAST_PWROFF_STATUS;

typedef enum
{
    FLY_ACC_ON,
    FLY_ACC_OFF,
    FLY_READY_TO_SUSPEND,
    FLY_SUSPEND,
} FLY_SYSTEM_STATUS;


//int fastboot_get_status(void);
void fastboot_pwroff(void);


#endif

