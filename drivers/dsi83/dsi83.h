#ifndef __SN68DSI83QZER_H__
#define __SN68DSI83QZER_H__

#include <asm/uaccess.h>
#include <linux/file.h>
#include <linux/syscalls.h>
#include <asm/system.h>

#define GET_INODE_FROM_FILEP(filp) ((filp)->f_path.dentry->d_inode)

inline int  kernel_read_write_file(const char *filename, char *rbuf,
	const char *wbuf, size_t length)
{
	int ret = 0;
	struct file *filp = (struct file *)-ENOENT;
	mm_segment_t oldfs;
	oldfs = get_fs();
	set_fs(KERNEL_DS);

	do {
		int mode = (wbuf) ? O_RDWR : O_RDONLY;
		filp = filp_open(filename, mode, S_IRUSR);

		if (IS_ERR(filp) || !filp->f_op) {
			ret = -ENOENT;
			break;
		}

		if (!filp->f_op->write || !filp->f_op->read) {
			filp_close(filp, NULL);
			ret = -ENOENT;
			break;
		}

		if (length == 0) {
			/* Read the length of the file only */
			struct inode    *inode;

			inode = GET_INODE_FROM_FILEP(filp);
			if (!inode) {
				printk(
					"kernel_readwrite_file: Error 2\n");
				ret = -ENOENT;
				break;
			}
			ret = i_size_read(inode->i_mapping->host);
			break;
		}

		if (wbuf) {
			ret = filp->f_op->write(
				filp, wbuf, length, &filp->f_pos);
			if (ret < 0) {
				printk(
					"kernel_readwrite_file: Error 3\n");
				break;
			}
		} else {
			ret = filp->f_op->read(
				filp, rbuf, length, &filp->f_pos);
			if (ret < 0) {
				printk(
					"kernel_readwrite_file: Error 4\n");
				break;
			}
		}
	} while (0);

	if (!IS_ERR(filp))
		filp_close(filp, NULL);

	set_fs(oldfs);

	return ret;
}

#define SN65_Sequence_seq2() kernel_read_write_file("/dev/sn65dsi83", NULL, "seq2", sizeof("seq2")-1)
#define SN65_Sequence_seq3() kernel_read_write_file("/dev/sn65dsi83", NULL, "seq3", sizeof("seq3")-1)
#define SN65_Sequence_seq4() kernel_read_write_file("/dev/sn65dsi83", NULL, "seq4", sizeof("seq4")-1)
#define SN65_Sequence_seq6() kernel_read_write_file("/dev/sn65dsi83", NULL, "seq6", sizeof("seq6")-1)
#define SN65_Sequence_seq7() kernel_read_write_file("/dev/sn65dsi83", NULL, "seq7", sizeof("seq7")-1)
#define SN65_Sequence_seq8() kernel_read_write_file("/dev/sn65dsi83", NULL, "seq8", sizeof("seq8")-1)
#define SN65_Dump() 		  kernel_read_write_file("/dev/sn65dsi83", NULL, "dump", sizeof("dump")-1)
#define SN65_Trace_Err() 		  kernel_read_write_file("/dev/sn65dsi83", NULL, "trace_err_status", sizeof("trace_err_status")-1)

#endif
