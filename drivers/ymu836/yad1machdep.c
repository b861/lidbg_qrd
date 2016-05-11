/****************************************************************************
 *
 *		Copyright(c) 2013-2014 Yamaha Corporation. All rights reserved.
 *
 *		Module		: yad1machdep.c
 *
 *		Description	: Yamaha Audio engine Device version 1
 *						device driver machine dependence part.
 *
 *		Version		: 1.1.0		2014.07.25
 *
 ****************************************************************************/

#include "yad1types.h"
#include "yad1driver.h"
#include "yad1machdep.h"

#if		(YAD1_DEVICE_IF	== YAD1_DEVICE_IF_SPI )
/*******************************************************************************
 *	YAD1MD_Write
 *
 *	Function:
 *			SPI Write
 *	Argument:
 *			u08Com				command byte
 *			pu08Data			pointer to data
 *			u32Size				size of data
 *	Return:
 *			YAD1DD_SUCCESS		success
 *			YAD1DD_ERROR_SPI	error(SPI communication)
 *			YAD1DD_ERROR_ARG	error(arguments)
 *
 ******************************************************************************/
S32	 YAD1MD_Write(U08 u08Com,const U08 *pu08Data,U32 u32Size)
{
	S32 s32Ret;
	U08 *write_buf=NULL;

	if (( pu08Data	== NULL )|| ( u32Size	== 0 ))
	{
		s32Ret	= YAD1DD_ERROR_ARG;
	}
	else
	{
		/* SPI Write */
		write_buf=kmalloc(u32Size+1,GFP_KERNEL);
		if(!write_buf)
		{
		  printk("alloc memery for ymu836 write err!");
		  return YAD1DD_ERROR_SPI;
		}
		write_buf[0]=u08Com;
		memcpy(write_buf+1,pu08Data,u32Size);
		s32Ret=spi_write(ymu836_spi,write_buf,u32Size+1);
		if(s32Ret<0)
		{
			s32Ret	= YAD1DD_ERROR_SPI;
		}
		else
		{
			s32Ret	= YAD1DD_SUCCESS;
		}
	}
	if(write_buf)
		kfree(write_buf);
	return s32Ret;
}

/*******************************************************************************
 *	YAD1MD_Read
 *
 *	Function:
 *			SPI Read
 *	Argument:
 *			u08Com				command byte
 *			pu08Data			pointer to data buffer
 *			u32Size				size of data buffer
 *	Return:
 *			YAD1DD_SUCCESS		success
 *			YAD1DD_ERROR_SPI	error(SPI communication)
 *			YAD1DD_ERROR_ARG	error(arguments)
 *
 ******************************************************************************/
S32	 YAD1MD_Read(U08 u08Com, U08 *pu08Data, U32 u32Size)
{
	S32 s32Ret;
	s32 i;
	U08 *buf=NULL;
	struct spi_message message;
	struct spi_transfer x[1];

	if (( pu08Data	== NULL )|| ( u32Size	== 0 ))
	{
		s32Ret	= YAD1DD_ERROR_ARG;
	}
	else
	{
		/* SPI Read */ 
		spi_message_init(&message);
		memset(x, 0, sizeof(x));
		buf=kmalloc(u32Size+1+u32Size+1,GFP_KERNEL);
		if(!buf)
		{
			printk("alloc memery for ymu583 read err!");
			return YAD1DD_ERROR_SPI;
		}
		buf[0]=u08Com;

		x[0].len=u32Size+1;
		x[0].tx_buf=buf;
		x[0].rx_buf=buf+u32Size+1;
		spi_message_add_tail(&x[0], &message);
		s32Ret=spi_sync(ymu836_spi, &message);
		if(s32Ret)
		{	
			s32Ret	= YAD1DD_ERROR_SPI;
		}
		else
		{
			memcpy(pu08Data,buf+u32Size+1+1,u32Size);
			s32Ret =  YAD1DD_SUCCESS;
			//printk("---------------value=0x%x\n",*pu08Data);
		}
	}
	if(buf)
		kfree(buf); 
	return s32Ret;
}
#elif	(YAD1_DEVICE_IF	== YAD1_DEVICE_IF_I2C )
/*******************************************************************************
 *	YAD1MD_WriteI2C
 *
 *	Function:
 *			I2C Write
 *	Argument:
 *			u08SA				I2C slave address
 *			u08IFA				Interface register address(with BW bit)
 *			pu08Data			pointer to data
 *			u32Size				size of data
 *	Return:
 *			YAD1DD_SUCCESS		success
 *			YAD1DD_ERROR_I2C	error(I2C communication)
 *			YAD1DD_ERROR_ARG	error(arguments)
 *
 ******************************************************************************/
S32		YAD1MD_WriteI2C
(
	U08					u08SA,
	U08					u08IFA,
	const U08 *			pu08Data,
	U32					u32Size
)
{
	S32					s32Ret;

	if (( pu08Data	== NULL )
	 || ( u32Size	== 0 ))
	{
		s32Ret	= YAD1DD_ERROR_ARG;
	}
	else
	{
		/* I2C Write */
		s32Ret	= YAD1DD_ERROR_I2C;
	}

	return s32Ret;
}

/*******************************************************************************
 *	YAD1MD_ReadI2C
 *
 *	Function:
 *			I2C Read
 *	Argument:
 *			u08SA				I2C slave address
 *			u08IFA				Interface register address(with BW bit)
 *			pu08Data			pointer to data
 *			u32Size				size of data
 *	Return:
 *			YAD1DD_SUCCESS		success
 *			YAD1DD_ERROR_I2C	error(I2C communication)
 *			YAD1DD_ERROR_ARG	error(arguments)
 *
 ******************************************************************************/
S32		YAD1MD_ReadI2C
(
	U08					u08SA,
	U08					u08IFA,
	U08 *				pu08Data,
	U32					u32Size
)
{
	S32					s32Ret;

	if (( pu08Data	== NULL )
	 || ( u32Size	!= 1 ))
	{
		s32Ret	= YAD1DD_ERROR_ARG;
	}
	else
	{
		/* I2C Read */
		s32Ret	= YAD1DD_ERROR_I2C;
	}

	return s32Ret;
}
#endif
/*******************************************************************************
 *	YAD1MD_Sleep
 *
 *	Function:
 *			Sleep u32MSec[msec]
 *	Argument:
 *			u32MSec				sleep time[msec]
 *	Return:
 *			None
 *
 ******************************************************************************/
void	YAD1MD_Sleep(U32	u32MSec)
{
	/* Sleep( u32MSec ) */
	msleep(u32MSec);
	return ;
}
