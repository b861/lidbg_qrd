

#ifdef SOC_COMPILE
#include "lidbg.h"
#include "fly_soc.h"

#else
#include "lidbg_def.h"

#include "lidbg_enter.h"

#endif
#include <mach/msm_rpcrouter.h>


enum
{
    BATTERY_VOLTAGE_UP = 0,
    BATTERY_VOLTAGE_DOWN,
    BATTERY_VOLTAGE_ABOVE_THIS_LEVEL,
    BATTERY_VOLTAGE_BELOW_THIS_LEVEL,
    BATTERY_VOLTAGE_LEVEL,
    BATTERY_ALL_ACTIVITY,
    VBATT_CHG_EVENTS,
    BATTERY_VOLTAGE_UNKNOWN
};


#define	BATTERY_RPC_PROG		0x30000089
#define	BATTERY_RPC_VER_5_1		0x00050001

#define	BATTERY_REGISTER_PROC		2

#define	BATTERY_CB_TYPE_PROC		1


#define	BATTERY_CB_ID_ALL		1
#define	BATTERY_CB_ID_LOW_V		2
#define	BATTERY_CB_ID_CHG_EVT   3

#define	CHG_RPC_PROG			0x3000001a
#define	CHG_RPC_VER_1_1			0x00010001

#define	CHG_GET_GENERAL_STATUS_PROC	9

#define	RPC_TIMEOUT			7000	/* 5 sec */
#define	INVALID_HANDLER			-1

#define	MSM_BATT_POLLING_TIME		(1 * HZ)

#define	BATTERY_LOW			3500


struct msm_battery_info
{
    struct msm_rpc_endpoint *charger_endpoint;
    struct msm_rpc_client *battery_client;
    u32 charger_api_version;
    u32 battery_api_version;


    u32 voltage_min_design;

    struct workqueue_struct	*battery_queue;
    struct delayed_work battery_work;
    struct mutex update_mutex;
    struct wake_lock charger_cb_wake_lock;

    s32 charger_handler;
    s32 battery_handler;

    u32 battery_voltage;



};

static struct msm_battery_info msm_battery_info =
{
    .battery_handler = INVALID_HANDLER,
    .charger_handler = INVALID_HANDLER,

    .battery_voltage = 3700,

};



struct rpc_reply_charger
{
    struct	rpc_reply_hdr hdr;
    u32 more_data;

    u32 charger_status;
    u32 charger_hardware;
    u32 hide;
    u32 battery_status;
    u32 battery_voltage;
    u32 battery_capacity;
    s32 battery_temp;
    u32 is_charging;
    u32 is_charging_complete;
    u32 is_charging_failed;
};

static struct rpc_reply_charger reply_charger;

#define	be32_to_cpu_self(v)	(v = be32_to_cpu(v))

static int msm_battery_get_charger_status(void)
{
    int rc;
    struct rpc_request_charger
    {
        struct rpc_request_hdr hdr;
        u32 more_data;
    } request_charger;

    request_charger.more_data = cpu_to_be32(1);
    memset(&reply_charger, 0, sizeof(reply_charger));

    rc = msm_rpc_call_reply(msm_battery_info.charger_endpoint,
                            CHG_GET_GENERAL_STATUS_PROC,
                            &request_charger, sizeof(request_charger),
                            &reply_charger, sizeof(reply_charger),
                            msecs_to_jiffies(RPC_TIMEOUT));
    if (rc < 0)
    {
        lidbg("BATT: ERROR: %s, charger rpc call %d, rc=%d\n",
              __func__, CHG_GET_GENERAL_STATUS_PROC, rc);
        return rc;
    }


    be32_to_cpu_self(reply_charger.battery_voltage);


    return 0;
}

void msm_battery_update_psy_status(void)
{


    bool is_awake = true;

    mutex_lock(&msm_battery_info.update_mutex);

    if (msm_battery_get_charger_status())
    {
        goto done;
    }
    lidbg("lidbg_BATT: battery_voltage %d\n", reply_charger.battery_voltage & 0xffff);

done:
    mutex_unlock(&msm_battery_info.update_mutex);

    if(is_awake)
    {
        queue_delayed_work(msm_battery_info.battery_queue,
                           &msm_battery_info.battery_work,
                           MSM_BATT_POLLING_TIME);
    }

    return;
}



struct battery_client_registration_request
{
    /* Voltage at which cb func should be called */
    u32 desired_battery_voltage;
    /* Direction when the cb func should be called */
    u32 voltage_direction;
    /* Registered callback to be called */
    u32 battery_cb_id;
    /* Call back data */
    u32 cb_data;
    u32 more_data;
    u32 battery_error;
};

struct battery_client_registration_reply
{
    u32 handler;
};

static int msm_battery_register_arg_func(struct msm_rpc_client *battery_client,
        void *buffer, void *data)
{
    struct battery_client_registration_request *battery_register_request =
        (struct battery_client_registration_request *)data;

    u32 *request = (u32 *)buffer;
    int size = 0;
    DUMP_FUN;
    *request = cpu_to_be32(battery_register_request->desired_battery_voltage);
    size += sizeof(u32);
    request++;

    *request = cpu_to_be32(battery_register_request->voltage_direction);
    size += sizeof(u32);
    request++;

    *request = cpu_to_be32(battery_register_request->battery_cb_id);
    size += sizeof(u32);
    request++;

    *request = cpu_to_be32(battery_register_request->cb_data);
    size += sizeof(u32);
    request++;

    *request = cpu_to_be32(battery_register_request->more_data);
    size += sizeof(u32);
    request++;

    *request = cpu_to_be32(battery_register_request->battery_error);
    size += sizeof(u32);

    return size;
}

static int msm_battery_register_ret_func(struct msm_rpc_client *battery_client,
        void *buffer, void *data)
{
    struct battery_client_registration_reply *data_ptr, *buffer_ptr;
    DUMP_FUN;
    data_ptr = (struct battery_client_registration_reply *)data;
    buffer_ptr = (struct battery_client_registration_reply *)buffer;

    data_ptr->handler = be32_to_cpu(buffer_ptr->handler);
    return 0;
}

static int msm_battery_register(u32 desired_battery_voltage, u32 voltage_direction,
                                u32 battery_cb_id, u32 cb_data, s32 *handle)
{
    struct battery_client_registration_request battery_register_request;
    struct battery_client_registration_reply battery_register_reply;
    void *request;
    void *reply;
    int rc;

    battery_register_request.desired_battery_voltage = desired_battery_voltage;
    battery_register_request.voltage_direction = voltage_direction;
    battery_register_request.battery_cb_id = battery_cb_id;
    battery_register_request.cb_data = cb_data;
    battery_register_request.more_data = 1;
    battery_register_request.battery_error = 0;
    request = &battery_register_request;

    reply = &battery_register_reply;


    rc = msm_rpc_client_req(msm_battery_info.battery_client,
                            BATTERY_REGISTER_PROC,
                            msm_battery_register_arg_func, request,
                            msm_battery_register_ret_func, reply,
                            msecs_to_jiffies(RPC_TIMEOUT));

    if (rc < 0)
    {
        lidbg("BATT: ERROR: %s, vbatt register, rc=%d\n", __func__, rc);
        return rc;
    }

    *handle = battery_register_reply.handler;

    return 0;
}

struct battery_client_deregister_request
{
    u32 handler;
};

struct battery_client_deregister_reply
{
    u32 battery_error;
};





static int msm_battery_cb_func(struct msm_rpc_client *client,
                               void *buffer, int in_size)
{
    int rc = 0;
    struct rpc_request_hdr *request;
    u32 procedure;
    u32 accept_status;

    DUMP_FUN;

    request = (struct rpc_request_hdr *)buffer;
    procedure = be32_to_cpu(request->procedure);

    switch (procedure)
    {
    case BATTERY_CB_TYPE_PROC:
        accept_status = RPC_ACCEPTSTAT_SUCCESS;
        break;

    default:
        accept_status = RPC_ACCEPTSTAT_PROC_UNAVAIL;
        lidbg("BATT: ERROR: %s, procedure (%d) not supported\n",
              __func__, procedure);
        break;
    }

    msm_rpc_start_accepted_reply(msm_battery_info.battery_client,
                                 be32_to_cpu(request->xid), accept_status);

    rc = msm_rpc_send_accepted_reply(msm_battery_info.battery_client, 0);
    if (rc)
        lidbg("BATT: ERROR: %s, sending reply, rc=%d\n", __func__, rc);

    if (accept_status == RPC_ACCEPTSTAT_SUCCESS)
    {
        wake_lock_timeout(&msm_battery_info.charger_cb_wake_lock, 10 * HZ);
        msm_battery_update_psy_status();
    }

    return rc;
}





static void msm_battery_worker(struct work_struct *work)
{
    msm_battery_update_psy_status();
}


static int __devinit msm_battery_probe(void)
{
    int rc;
    msm_battery_info.voltage_min_design = BATTERY_LOW;


    msm_battery_info.battery_queue = create_singlethread_workqueue( "battery_queue");


    INIT_DELAYED_WORK(&msm_battery_info.battery_work, msm_battery_worker);
    mutex_init(&msm_battery_info.update_mutex);

#if 1
    rc = msm_battery_register(msm_battery_info.voltage_min_design,
                              BATTERY_VOLTAGE_BELOW_THIS_LEVEL,
                              BATTERY_CB_ID_LOW_V,
                              BATTERY_VOLTAGE_BELOW_THIS_LEVEL,
                              &msm_battery_info.battery_handler);


    rc = msm_battery_register(msm_battery_info.voltage_min_design,
                              VBATT_CHG_EVENTS,
                              BATTERY_CB_ID_CHG_EVT,
                              VBATT_CHG_EVENTS,
                              &msm_battery_info.charger_handler);
#endif

    wake_lock_init(&msm_battery_info.charger_cb_wake_lock, WAKE_LOCK_SUSPEND,  "msm_lidbg_cb");
    queue_delayed_work(msm_battery_info.battery_queue,  &msm_battery_info.battery_work, 0);



    return 0;
}


static int  msm_battery_init_rpc(void)
{
    int rc = 0;


    msm_battery_info.charger_endpoint =
        msm_rpc_connect_compatible(CHG_RPC_PROG, CHG_RPC_VER_1_1, 0);

    msm_battery_info.charger_api_version = CHG_RPC_VER_1_1;

    msm_battery_info.battery_client = msm_rpc_register_client(
                                          "battery", BATTERY_RPC_PROG, BATTERY_RPC_VER_5_1, 1,
                                          msm_battery_cb_func);

    msm_battery_info.battery_api_version = BATTERY_RPC_VER_5_1;


    return 0;
}

static int __init msm_battery_init(void)
{
    int rc = 0;

    DUMP_BUILD_TIME;

    lidbg("BATT: %s, enter\n", __func__);


    rc = msm_battery_init_rpc();


    msm_battery_probe();


    lidbg("BATT: %s, exit\n", __func__);
    return 0;
}

static void __exit msm_battery_exit(void)
{
}

module_init(msm_battery_init);
module_exit(msm_battery_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Kiran Kandi, Qualcomm Innovation Center, Inc.");
MODULE_DESCRIPTION("Battery driver for Qualcomm MSM chipsets.");
MODULE_VERSION("2.0");
MODULE_ALIAS("platform:msm_battery");
