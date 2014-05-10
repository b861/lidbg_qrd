#ifndef _SEM_CMD_H
#define _SEM_CMD_H


#define SEM_MAGIC 's' //定义幻数
#define SEM_MAX_NR 2 //定义命令的最大序数

//ioctl
#define SEM_LOCK _IO(SEM_MAGIC, 1) 		//用户加锁
#define SEM_UNLOCK _IO(SEM_MAGIC, 2)	//用户解锁

#define READ_PAGEID_FORJAVA _IOR(SEM_MAGIC, 3 ,int) //JAVA读取页面接口
#define WRITE_PAGEID_FORJAVA _IOW(SEM_MAGIC, 4 ,short)	//JAVA写页面接口

#define READ_PAGEID_FORC _IOR(SEM_MAGIC, 5 ,int) //C读取页面接口
#define WRITE_PAGEID_FORC _IOW(SEM_MAGIC, 6 ,short)	//C写页面接口

#endif /*_SEM_CMD_H*/
