#ifndef _LIGDBG_FILESERVER__
#define _LIGDBG_FILESERVER__

void lidbg_fileserver_main(int argc, char **argv);

//zone start
enum string_dev_cmd
{
    //file mode under cmd for fileserver_main();
    FS_CMD_FILE_CONFIGMODE,//(1,1,0,1)
    FS_CMD_FILE_LISTMODE,
    FS_CMD_FILE_APPENDMODE,//note:you should add [\n] in your string like["\n###save some state###\n ts=gt801\n"],it's suc

    //after had given you a client_list, you can do with it on your owen purpose;and also, I want supply some;
    FS_CMD_LIST_SPLITKV,//kv:key=value //(1,1,0,0)
    FS_CMD_LIST_SHOW,//(1,1,0,0)
    FS_CMD_LIST_IS_STRINFILE,//(1,1,1,0)
    FS_CMD_LIST_GETVALUE,//(1,1,0,1)
    FS_CMD_LIST_SETVALUE,//(1,1,0,1)
    FS_CMD_LIST_GETLISTSIZE,//not ok
    FS_CMD_COUNT,
};
struct string_dev
{
    struct list_head tmp_list;
    char *yourkey;
    char *yourvalue;
    int *int_value;
    void (*callback)(char *key,char *value);
};
extern int fs_get_intvalue(struct list_head *client_list, char *key,int *int_value,void (*callback)(char *key,char *value));
extern int fs_get_value(struct list_head *client_list, char *key, char **string);
extern int fs_set_value(struct list_head *client_list, char *key, char *string);
extern int fs_find_string(struct list_head *client_list, char *string);
extern int fs_show_list(struct list_head *client_list);
extern int fs_file_log( const char *fmt, ...);
extern int fs_fill_list(char *filename, enum string_dev_cmd cmd, struct list_head *client_list);
extern bool fs_copy_file(char *from, char *to);
extern void fs_log_sync(void);
extern struct list_head lidbg_drivers_list;
extern struct list_head lidbg_core_list;

#define lidbg_fs(fmt,...) do{fs_file_log(fmt,##__VA_ARGS__);}while(0)

#define FS_REGISTER_INT_DRV(name,def_value,callback) name=def_value; \
			fs_get_intvalue(&lidbg_drivers_list, #name,&name,callback); \
			lidbg("config:"#name"=%d\n",name);

#define FS_REGISTER_INT_CORE(name,def_value,callback) name=def_value; \
			fs_get_intvalue(&lidbg_core_list, #name,&name,callback); \
			lidbg("config:"#name"=%d\n",name);


#endif

