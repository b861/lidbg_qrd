
//futengfei add for bpmsg_to_ap_print  2012.12.12.12:00

//up always online

#define TOTAL_LOGS 1000UL
#define LOG_BYTES  32UL
typedef struct
{
int start_pos;
int end_pos;
char log[TOTAL_LOGS][LOG_BYTES];
u8 write_flag;
}smem_log_deep;


