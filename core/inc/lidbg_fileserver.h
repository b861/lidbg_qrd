#ifndef _LIGDBG_FILESERVER__
#define _LIGDBG_FILESERVER__

#define __LOG_BUF_LEN	(1 << CONFIG_LOG_BUF_SHIFT)

/*
    FS增加抓取kmsg功能+解析cmd功能+动态刷新state功能

    抓取kmsg功能:配置SD中core.conf的fs_kmsg_en=0为1，被阻塞的kmsg线程将启动；重新设置为0后线程将重新被阻塞
    解析cmd功能:0c key back 2 第一字符0表示执行指令的次数（最多9次）
    动态刷新state:动态刷出acc_off_times=0(state.conf)等需要动态显示的内容，\
    刷新率由core.config中的fs_updatestate_ms给出,如其为零，刷新线程将阻塞
    另：删除了fileserver_main函数
*/

//zone start
enum string_dev_cmd
{
    FS_CMD_FILE_CONFIGMODE,
    FS_CMD_FILE_LISTMODE,

    //after had given you a client_list, you can do with it on your owen purpose;and also, I want supply some;
    FS_CMD_LIST_SPLITKV,//kv:key=value
    FS_CMD_LIST_SHOW,
    FS_CMD_LIST_IS_STRINFILE,
    FS_CMD_LIST_GETVALUE,
    FS_CMD_LIST_SETVALUE,//not malloc mem
    FS_CMD_LIST_SETVALUE2,//malloc mem
    FS_CMD_LIST_SAVEINFO,
    FS_CMD_LIST_SAVE2FILE,
    FS_CMD_LIST_GETLISTSIZE,//not ok
    FS_CMD_COUNT,
};
struct string_dev
{
    struct list_head tmp_list;
    char *yourkey;
    char *yourvalue;
    char *filedetec;
    int *int_value;
    bool have_warned;
    void (*callback)(char * key, char * value);
    void (*cb_filedetec)(char * filename );
};
void lidbg_fileserver_main(int argc, char **argv);
extern void fs_file_separator(char *file2separator);
extern void fs_regist_filedetec(char *filename, void (*cb_filedetec)(char *filename ));
extern void fs_enable_kmsg( bool enable );
extern void fs_save_state(void);
extern void fs_log_sync(void);
extern int get_machine_id(void);
extern int fs_dump_kmsg(char *tag, int size );
extern int fs_regist_state(char *key, int *value);
extern int fs_get_intvalue(struct list_head *client_list, char *key,int *int_value,void (*callback)(char *key,char *value));
extern int fs_get_value(struct list_head *client_list, char *key, char **string);
extern int fs_set_value(struct list_head *client_list, char *key, char *string);
extern int fs_find_string(struct list_head *client_list, char *string);
extern int fs_string2file(char *filename, const char *fmt, ... );
extern int fs_show_list(struct list_head *client_list);
extern int fs_file_log( const char *fmt, ...);
extern int fs_fill_list(char *filename, enum string_dev_cmd cmd, struct list_head *client_list);
extern bool fs_copy_file(char *from, char *to);
extern struct list_head lidbg_drivers_list;
extern struct list_head lidbg_core_list;


#define lidbg_fs_log(path,fmt,...) do{	char buf[32];\
								lidbg_get_current_time(buf,NULL);\
								fs_string2file(path,"[%s] ",buf);\
								fs_string2file(path,fmt,##__VA_ARGS__);\
								}while(0)


#define lidbg_fs(fmt,...) do{	char buf[32];\
								lidbg_get_current_time(buf,NULL);\
								fs_file_log("[%s] ",buf);\
								fs_file_log(fmt,##__VA_ARGS__);\
								}while(0)

#define FS_REGISTER_INT(intvalue,key,def_value,callback) intvalue=def_value; \
			if(fs_get_intvalue(&lidbg_drivers_list, key,&intvalue,callback)<0) \
				fs_get_intvalue(&lidbg_core_list, key,&intvalue,callback);\
			lidbg("config:%s=%d\n",key,intvalue);
//zone end


#endif

