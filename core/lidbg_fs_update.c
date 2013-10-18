
#include "lidbg.h"
#include "lidbg_fs.h"

//zone below [fs.update.tools]

//zone end


//zone below [fs.update.driver]
int update_ko(const char *ko_list, const char *fromdir, const char *todir)
{
    struct file *filep;
    struct inode *inode = NULL;
    mm_segment_t old_fs;
    char *token, *file_ptr = NULL, *file_ptmp;
    char file_from[256];
    char file_to[256];
    int all_purpose;
    unsigned int file_len;

    filep = filp_open(ko_list, O_RDONLY , 0);
    if(IS_ERR(filep))
    {
        printk("[futengfei]err.open:<%s>\n", ko_list);
        return -1;
    }
    printk("[futengfei]succeed.open:<%s>\n", ko_list);

    fs_remount_system();

    old_fs = get_fs();
    set_fs(get_ds());

    inode = filep->f_dentry->d_inode;
    file_len = inode->i_size;
    file_len = file_len + 2;

    file_ptr = (unsigned char *)kzalloc(file_len, GFP_KERNEL);
    if(file_ptr == NULL)
    {
        printk( "[futengfei]err.vmalloc:<cannot kzalloc memory!>\n");
        return -1;
    }

    if(g_mem_dbg)
        fs_string2file(DEBUG_MEM_FILE, "free.%s=%d \n", __func__, file_len);

    filep->f_op->llseek(filep, 0, 0);
    all_purpose = filep->f_op->read(filep, file_ptr, file_len, &filep->f_pos);
    if(all_purpose <= 0)
    {
        printk( "[futengfei]err.f_op->read:<read file data failed>\n");
        return -1;
    }
    set_fs(old_fs);
    filp_close(filep, 0);

    file_ptr[file_len - 1] = '\0';
    file_ptmp = file_ptr;
    while((token = strsep(&file_ptmp, "\n")) != NULL )
    {
        if( token[0] != '#' && strlen(token) > 2)
        {
            memset(file_from, '\0', sizeof(file_from));
            memset(file_to, '\0', sizeof(file_to));
            sprintf(file_from, "%s/%s", fromdir, token);
            sprintf(file_to, "%s/%s", todir, token);
            FS_WARN("<cp:%s,%s>\n", file_from, file_to);
            fs_copy_file(file_from, file_to);
        }
    }
    kfree(file_ptr);
    return 1;
}
//zone end


//zone below [fs.update.interface]
int fs_update(const char *ko_list, const char *fromdir, const char *todir)
{
    return update_ko(ko_list, fromdir, todir);
}
//zone end

void lidbg_fs_update_init(void)
{

}

EXPORT_SYMBOL(fs_update);

