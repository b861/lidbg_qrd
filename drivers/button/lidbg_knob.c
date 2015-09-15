
#include "lidbg.h"
LIDBG_DEFINE;

static  struct file_operations knob_fops;

#define ENABLE_ENCODE_IRQ_PROC	 1

//inc&dec ID
#define KB_VOL_DEC	0x00
#define KB_VOL_INC	0x01
#define KB_TUNE_DEC	0x02
#define KB_TUNE_INC	0x03	

#define EncoderID_L1	0x80		//Left Dec
#define EncoderID_L2	0x81		//Left Inc
#define EncoderID_R1	0x82		//Right Dec
#define EncoderID_R2	0x83		//Right Inc

//#define EXTI_USE_DELAYED		1

#if defined(EXTI_USE_DELAYED)
static struct workqueue_struct *p_work_encode_queue;
#endif

struct fly_KeyEncoderInfo{
	u8 iEncoderLeftIncCount;
	u8 iEncoderLeftDecCount;
	u8 iEncoderRightIncCount;
	u8 iEncoderRightDecCount;

	u8 curEncodeValueLeft;
	u8 curEncodeValueRight;

	u8 bTimeOutRun;
	u8 time_out;
	
	struct work_struct encoder_work;
	struct semaphore sem;
	wait_queue_head_t wait_queue;	
};


struct fly_KeyEncoderInfo *pfly_KeyEncoderInfo;//pointer to fly_KeyEncoderInfo

//for origin knob_data handle
u8 isProcess = 1;
struct completion origin_completion;

//u8 knob_data_for_hal;
#define HAL_BUF_SIZE (512)
u8 knob_data_for_hal[HAL_BUF_SIZE];

#define FIFO_SIZE (512)
u8 knob_fifo_buffer[FIFO_SIZE];
static struct kfifo knob_data_fifo;

spinlock_t irq_lock;
static u8 left_irq_is_disabled,right_irq_is_disabled;


/**
 * EncoderIDExchange - exchange data according to hw config
 * @index: the signal to exchange
 *
 * According to hw config,change iEncoderCounter independently.
 *
 */
void EncoderIDExchange(BYTE index)
{
	if (index >= EncoderID_L1 && index <= EncoderID_R2)
	{
		if (KB_VOL_INC == (index - 0x80))
		{
			if(g_hw.is_single_edge) pfly_KeyEncoderInfo->iEncoderLeftIncCount += 2;
			else pfly_KeyEncoderInfo->iEncoderLeftIncCount++;
			pfly_KeyEncoderInfo->iEncoderLeftDecCount = 0;
		}
		else if (KB_VOL_DEC == (index - 0x80))
		{
			if(g_hw.is_single_edge) pfly_KeyEncoderInfo->iEncoderLeftDecCount += 2;
			else pfly_KeyEncoderInfo->iEncoderLeftDecCount++;
			pfly_KeyEncoderInfo->iEncoderLeftIncCount = 0;
		}
		else if (KB_TUNE_INC == (index - 0x80))
		{
			if(g_hw.is_single_edge) pfly_KeyEncoderInfo->iEncoderRightIncCount += 2;
			else pfly_KeyEncoderInfo->iEncoderRightIncCount++;;
			pfly_KeyEncoderInfo->iEncoderRightDecCount = 0;
		}
		else if (KB_TUNE_DEC == (index - 0x80) )
		{
			if(g_hw.is_single_edge) pfly_KeyEncoderInfo->iEncoderRightDecCount += 2;
			else pfly_KeyEncoderInfo->iEncoderRightDecCount++;;
			pfly_KeyEncoderInfo->iEncoderRightIncCount = 0;
		}
	}
}

/**
 * thread_process - origin system knob_data process thread
 * @data: the pointer of data
 *
 * Origin system knob_data process thread.
 *
 */
int thread_process(void *data)
{
	int read_len,fifo_len,bytes,i;
	while(1)
	{
		wait_for_completion(&origin_completion);
		if(isProcess)
		{
			//pr_debug("[knob]thread_process begin!");
			down(&pfly_KeyEncoderInfo->sem);
			if(kfifo_is_empty(&knob_data_fifo))
			{
				up(&pfly_KeyEncoderInfo->sem);
				continue;
			}

			fifo_len = kfifo_len(&knob_data_fifo);
			
			if(fifo_len > HAL_BUF_SIZE )
			    read_len = HAL_BUF_SIZE;
			else
			    read_len = fifo_len;

			bytes = kfifo_out(&knob_data_fifo, knob_data_for_hal, read_len);
			up(&pfly_KeyEncoderInfo->sem);

			for(i = 0;i < read_len;i++)
			{
				if (knob_data_for_hal[i] & (1 << 0))
				{
					if(g_var.recovery_mode == 1)
						SOC_Key_Report(KEY_UP, KEY_PRESSED_RELEASED);
					else
						SOC_Key_Report(KEY_VOLUMEUP, KEY_PRESSED_RELEASED);
					
				}
				if (knob_data_for_hal[i] & (1 << 1))
				{
					if(g_var.recovery_mode == 1)
						SOC_Key_Report(KEY_DOWN, KEY_PRESSED_RELEASED);
					else
						SOC_Key_Report(KEY_VOLUMEDOWN, KEY_PRESSED_RELEASED);
				}
				if (knob_data_for_hal[i] & (1 << 2))
				{
					if(g_var.recovery_mode == 1)
						SOC_Key_Report(KEY_UP, KEY_PRESSED_RELEASED);
					else
						SOC_Key_Report(KEY_VOLUMEUP, KEY_PRESSED_RELEASED);
				}
				if (knob_data_for_hal[i] & (1 << 3))
				{
					if(g_var.recovery_mode == 1)
						SOC_Key_Report(KEY_DOWN, KEY_PRESSED_RELEASED);
					else
						SOC_Key_Report(KEY_VOLUMEDOWN, KEY_PRESSED_RELEASED);
				}
			}
		}
	}	
}

/**
 * work_knob_fn - put knob data into the fifo,and wakeup the wait_queue
 * @work_struct: the pointer of work_struct
 *
 * work function to process knob_data and put into fifo.
 *
 */
static void work_knob_fn(struct work_struct *work)
{
	u8 knob_data;
	
	if(!((pfly_KeyEncoderInfo->iEncoderLeftIncCount >= 2) | (pfly_KeyEncoderInfo->iEncoderLeftDecCount >= 2) | 
		(pfly_KeyEncoderInfo->iEncoderRightIncCount >= 2) | (pfly_KeyEncoderInfo->iEncoderRightDecCount >= 2)) )
	{
		return;
	}
	
	pr_debug("\nFlyKeyEncoderThread start\n");

	knob_data = 0;
	while (pfly_KeyEncoderInfo->iEncoderLeftIncCount >= 2)
	{
		knob_data |= (1 << 0);
		pfly_KeyEncoderInfo->iEncoderLeftIncCount -= 2;
	}
	while (pfly_KeyEncoderInfo->iEncoderLeftDecCount >= 2)
	{
		knob_data |= (1 <<1);
		pfly_KeyEncoderInfo->iEncoderLeftDecCount -= 2;
	}
	while (pfly_KeyEncoderInfo->iEncoderRightIncCount >= 2)
	{
		knob_data |= (1 <<2);
		pfly_KeyEncoderInfo->iEncoderRightIncCount-=2;
	}
	while (pfly_KeyEncoderInfo->iEncoderRightDecCount >= 2)
	{
		knob_data |= (1 <<3);
		pfly_KeyEncoderInfo->iEncoderRightDecCount-=2;
	}


	pfly_KeyEncoderInfo->bTimeOutRun = 1;
	//pfly_KeyEncoderInfo->time_out = GetTickCount();

	down(&pfly_KeyEncoderInfo->sem);
        if(kfifo_is_full(&knob_data_fifo))
        {
	        u8 temp_reset_data;
		int tempbyte;
		//kfifo_reset(&knob_data_fifo);
		tempbyte = kfifo_out(&knob_data_fifo, &temp_reset_data, 1);
		pr_debug("[knob]kfifo_reset!!!!!\n");
        }
        kfifo_in(&knob_data_fifo, &knob_data, 1);
        up(&pfly_KeyEncoderInfo->sem);

	wake_up_interruptible(&pfly_KeyEncoderInfo->wait_queue);
	complete(&origin_completion);
	pr_debug("knob_data = %x\n",knob_data);
}

/**
 * enable_left_irq - left knob irq enable
 *
 * left knob irq enable.
 *
 */
void enable_left_irq(void)
{
	unsigned long irqflags = 0;
	spin_lock_irqsave(&irq_lock, irqflags);
	if (left_irq_is_disabled) {
		enable_irq(GPIO_TO_INT(BUTTON_LEFT_1));
		enable_irq(GPIO_TO_INT(BUTTON_LEFT_2));
		left_irq_is_disabled = 0;
	}
	spin_unlock_irqrestore(&irq_lock, irqflags);
}

/**
 * enable_right_irq - right knob irq enable
 *
 * right knob irq enable.
 *
 */
void enable_right_irq(void)
{
	unsigned long irqflags = 0;
	spin_lock_irqsave(&irq_lock, irqflags);
	if (right_irq_is_disabled) {
		enable_irq(GPIO_TO_INT(BUTTON_RIGHT_1));
		enable_irq(GPIO_TO_INT(BUTTON_RIGHT_2));
		right_irq_is_disabled = 0;
	}
	spin_unlock_irqrestore(&irq_lock, irqflags);
}

/**
 * irq_left_proc - left knob irq process
 * @num: 1-irq line 1(left rising)
 *		2-irq line 2(left falling)
 *
 * left knob irq process,according to BUTTON_LEFT_1 and BUTTON_LEFT_2.
 *
 */
void irq_left_proc(u8 num)
{
	pfly_KeyEncoderInfo->curEncodeValueLeft = pfly_KeyEncoderInfo->curEncodeValueLeft << 4;
#if defined(BUTTON_LEFT_1) && defined(ENABLE_ENCODE_IRQ_PROC)
	if (SOC_IO_Input(BUTTON_LEFT_1, BUTTON_LEFT_1, GPIO_CFG_PULL_UP))
#else
	if (0)
#endif
	{
		pfly_KeyEncoderInfo->curEncodeValueLeft |= (1 << 2);
	}

#if defined(BUTTON_LEFT_2) && defined(ENABLE_ENCODE_IRQ_PROC)
	if (SOC_IO_Input(BUTTON_LEFT_2, BUTTON_LEFT_2, GPIO_CFG_PULL_UP))
#else
	if (0)
#endif
	{
		pfly_KeyEncoderInfo->curEncodeValueLeft |= (1 << 0);
	}
	pr_debug("L:%x,num:%d",pfly_KeyEncoderInfo->curEncodeValueLeft,num);
	if(g_hw.is_single_edge)
	{
		if(num == 1)
		{
			//debounce(rising irq when signal fall down)
			if((pfly_KeyEncoderInfo->curEncodeValueLeft & (1 << 2)) == 0) 
			{
				enable_left_irq();
				return;//debounce
			}
			
			//if(old_left_num == num) //debounce(same irq)
			if(((pfly_KeyEncoderInfo->curEncodeValueLeft >> 4) & 0x0F) ==
				(pfly_KeyEncoderInfo->curEncodeValueLeft & 0x0F)) //debounce(same irq)
			{
				pr_debug("---------ehol1 debounce------");
				enable_left_irq();
				return;//debounce
			}
			//old_left_num = num;
			
			if(!(pfly_KeyEncoderInfo->curEncodeValueLeft & (1 << 0)))
			{
				pr_debug("num1---L1---done");
				EncoderIDExchange(EncoderID_L1);
			}
			else
			{
				pr_debug("num1---L2--done");
				EncoderIDExchange(EncoderID_L2);
			}
		}
		
		if(num == 2)
		{
			//debounce(falling irq when signal rise up)
			if(pfly_KeyEncoderInfo->curEncodeValueLeft & (1 << 0))
			{
				enable_left_irq();
				return;
			}
			
			//if(old_left_num == num) //debounce(same irq)
			if(((pfly_KeyEncoderInfo->curEncodeValueLeft >> 4) & 0x0F) ==
				(pfly_KeyEncoderInfo->curEncodeValueLeft & 0x0F)) //debounce(same irq)
			{
				pr_debug("---------ehol2 debounce------");
				enable_left_irq();
				return;//debounce
			}
			//old_left_num = num;
			
			if(!(pfly_KeyEncoderInfo->curEncodeValueLeft & (1 << 2)))
			{
				pr_debug("num2---L1---done");
				EncoderIDExchange(EncoderID_L1);
			}
			else
			{
				pr_debug("num2---L2--done");
				EncoderIDExchange(EncoderID_L2);
			}
		}
	}
	else
	{
		if (pfly_KeyEncoderInfo->curEncodeValueLeft == 0x04 || pfly_KeyEncoderInfo->curEncodeValueLeft == 0x45
			|| pfly_KeyEncoderInfo->curEncodeValueLeft == 0x51 || pfly_KeyEncoderInfo->curEncodeValueLeft == 0x10)
		{
			EncoderIDExchange(EncoderID_L1);
		}
		else if (pfly_KeyEncoderInfo->curEncodeValueLeft == 0x01 || pfly_KeyEncoderInfo->curEncodeValueLeft == 0x15
			|| pfly_KeyEncoderInfo->curEncodeValueLeft == 0x54 || pfly_KeyEncoderInfo->curEncodeValueLeft == 0x40)
		{
			EncoderIDExchange(EncoderID_L2);
		}
	}
	
	//irq proc
#if defined(ENABLE_ENCODE_IRQ_PROC)
	if(!work_pending(&pfly_KeyEncoderInfo->encoder_work))
	{
		#if defined(EXTI_USE_DELAYED)
		queue_delayed_work(p_work_encode_queue, &pfly_KeyEncoderInfo->encoder_work, 1);
		#else
		schedule_work(&pfly_KeyEncoderInfo->encoder_work);
		#endif
	}	
#endif
	if(g_hw.is_single_edge) enable_left_irq();
}

/**
 * irq_right_proc - right knob irq process
 * @num: 1-irq line 1(right rising)
 *		2-irq line 2(right falling)
 *
 * right knob irq process,according to BUTTON_RIGHT_1 and BUTTON_RIGHT_2.
 *
 */
void irq_right_proc(u8 num)
{
	pfly_KeyEncoderInfo->curEncodeValueRight= pfly_KeyEncoderInfo->curEncodeValueRight << 4;
#if defined(BUTTON_RIGHT_1) && defined(ENABLE_ENCODE_IRQ_PROC)
	if (SOC_IO_Input(BUTTON_RIGHT_1, BUTTON_RIGHT_1, GPIO_CFG_PULL_UP))
#else
	if (0)
#endif
	{
		pfly_KeyEncoderInfo->curEncodeValueRight|= (1 << 2);
	}

#if defined(BUTTON_RIGHT_2) && defined(ENABLE_ENCODE_IRQ_PROC)
	if (SOC_IO_Input(BUTTON_RIGHT_2, BUTTON_RIGHT_2, GPIO_CFG_PULL_UP))
#else
	if (0)
#endif
	{
		pfly_KeyEncoderInfo->curEncodeValueRight |= (1 << 0);
	}
	pr_debug("R:%x,num:%d",pfly_KeyEncoderInfo->curEncodeValueRight,num);
	if(g_hw.is_single_edge)
	{
		if(num == 1)
		{
			//debounce(rising irq when signal fall down)
			if((pfly_KeyEncoderInfo->curEncodeValueRight & (1 << 2)) == 0)
			{
				enable_right_irq();
				return;
			}
			
			//if(old_right_num == num) //debounce(same irq)
			if(((pfly_KeyEncoderInfo->curEncodeValueRight >> 4) & 0x0F) ==
				(pfly_KeyEncoderInfo->curEncodeValueRight & 0x0F)) //debounce(same irq)
			{
				pr_debug("---------ehor1 debounce------");
				enable_right_irq();
				return;//debounce
			}
			//old_right_num = num;
			
			if(pfly_KeyEncoderInfo->curEncodeValueRight & (1 << 0))
			{
				pr_debug("num1---R1---done");
				EncoderIDExchange(EncoderID_R1);
			}
			else
			{
				pr_debug("num1---R2--done");
				EncoderIDExchange(EncoderID_R2);
			}
		}
		
		if(num == 2)
		{
			//debounce(falling irq when signal rise up)
			if(pfly_KeyEncoderInfo->curEncodeValueRight & (1 << 0))
			{
				enable_right_irq();
				return;
			}
			
			//if(old_right_num == num)//debounce(same irq)
			if(((pfly_KeyEncoderInfo->curEncodeValueRight >> 4) & 0x0F) ==
				(pfly_KeyEncoderInfo->curEncodeValueRight & 0x0F)) //debounce(same irq)
			{
				pr_debug("---------eho debounce------");
				enable_right_irq();
				return;//debounce
			}
			//old_right_num = num;
			
			if(pfly_KeyEncoderInfo->curEncodeValueRight & (1 << 2))
			{
				pr_debug("num2---R1---done");
				EncoderIDExchange(EncoderID_R1);
			}
			else
			{
				pr_debug("num2---R2--done");
				EncoderIDExchange(EncoderID_R2);
			}
		}
	}
	else
	{
		if (pfly_KeyEncoderInfo->curEncodeValueRight == 0x04 || pfly_KeyEncoderInfo->curEncodeValueRight == 0x45
			|| pfly_KeyEncoderInfo->curEncodeValueRight == 0x51 || pfly_KeyEncoderInfo->curEncodeValueRight == 0x10)
		{
			EncoderIDExchange(EncoderID_R2);
		}
		else if (pfly_KeyEncoderInfo->curEncodeValueRight == 0x01 || pfly_KeyEncoderInfo->curEncodeValueRight == 0x15
			|| pfly_KeyEncoderInfo->curEncodeValueRight == 0x54 || pfly_KeyEncoderInfo->curEncodeValueRight == 0x40)
		{
			EncoderIDExchange(EncoderID_R1);
		}
	}
	
#if defined(ENABLE_ENCODE_IRQ_PROC)
	if(!work_pending(&pfly_KeyEncoderInfo->encoder_work))
	{
		#if defined(EXTI_USE_DELAYED)
		queue_delayed_work(p_work_encode_queue, &pfly_KeyEncoderInfo->encoder_work, 1);
		#else
		schedule_work(&pfly_KeyEncoderInfo->encoder_work);
		#endif
	}
#endif
	if(g_hw.is_single_edge) enable_right_irq();
}

irqreturn_t irq_left_knob1(int irq, void *dev_id)
{
	if(g_hw.is_single_edge)
	{
		unsigned long irqflags = 0;
		spin_lock_irqsave(&irq_lock, irqflags);//prevent irq from nesting
		if (!left_irq_is_disabled) 
		{
			left_irq_is_disabled = 1;
			disable_irq_nosync(GPIO_TO_INT(BUTTON_LEFT_1));
			disable_irq_nosync(GPIO_TO_INT(BUTTON_LEFT_2));
		}
		spin_unlock_irqrestore(&irq_lock, irqflags);
	}
    	pr_debug("irq_left_knob1: %d\n", irq);
	irq_left_proc(1);
	return IRQ_HANDLED;

}
irqreturn_t irq_left_knob2(int irq, void *dev_id)
{
	if(g_hw.is_single_edge)
	{
		unsigned long irqflags = 0;
		spin_lock_irqsave(&irq_lock, irqflags);//prevent irq from nesting
		if (!left_irq_is_disabled) 
		{
			left_irq_is_disabled = 1;
			disable_irq_nosync(GPIO_TO_INT(BUTTON_LEFT_1));
			disable_irq_nosync(GPIO_TO_INT(BUTTON_LEFT_2));
		}
		spin_unlock_irqrestore(&irq_lock, irqflags);
	}
	pr_debug("irq_left_knob2: %d\n", irq);
	irq_left_proc(2);
	return IRQ_HANDLED;
}
irqreturn_t irq_right_knob1(int irq, void *dev_id)
{
	if(g_hw.is_single_edge)
	{
		unsigned long irqflags = 0;
		spin_lock_irqsave(&irq_lock, irqflags);//prevent irq from nesting
		if (!right_irq_is_disabled) 
		{
			right_irq_is_disabled = 1;
			disable_irq_nosync(GPIO_TO_INT(BUTTON_RIGHT_1));
			disable_irq_nosync(GPIO_TO_INT(BUTTON_RIGHT_2));
		}
		spin_unlock_irqrestore(&irq_lock, irqflags);
	}
	pr_debug("irq_right_knob1: %d\n", irq);
	irq_right_proc(1);
	return IRQ_HANDLED;
}
irqreturn_t irq_right_knob2(int irq, void *dev_id)
{
	if(g_hw.is_single_edge)
	{
		unsigned long irqflags = 0;
		spin_lock_irqsave(&irq_lock, irqflags);//prevent irq from nesting
		if (!right_irq_is_disabled) 
		{
			right_irq_is_disabled = 1;
			disable_irq_nosync(GPIO_TO_INT(BUTTON_RIGHT_1));
			disable_irq_nosync(GPIO_TO_INT(BUTTON_RIGHT_2));
		}
		spin_unlock_irqrestore(&irq_lock, irqflags);
	}
	pr_debug("irq_right_knob2: %d\n", irq);
	irq_right_proc(2);
	return IRQ_HANDLED;
}


/**
 * knob_suspend - disable irq
 *
 * disable irq of all pins.
 *
 */
static int knob_suspend(struct platform_device *pdev, pm_message_t state)
{
    SOC_IO_ISR_Disable(BUTTON_LEFT_1);
    SOC_IO_ISR_Disable(BUTTON_LEFT_2);
    SOC_IO_ISR_Disable(BUTTON_RIGHT_1);
    SOC_IO_ISR_Disable(BUTTON_RIGHT_2);
    return 0;
}

/**
 * knob_resume - resume irq
 *
 * resume irq of all pins.
 *
 */
static int knob_resume(struct platform_device *pdev)
{

    IO_CONFIG_INPUT(0,BUTTON_LEFT_1);
    IO_CONFIG_INPUT(0,BUTTON_LEFT_2);
    IO_CONFIG_INPUT(0,BUTTON_RIGHT_1);
    IO_CONFIG_INPUT(0,BUTTON_RIGHT_2);

    SOC_IO_ISR_Enable(BUTTON_LEFT_1);
    SOC_IO_ISR_Enable(BUTTON_LEFT_2);
    SOC_IO_ISR_Enable(BUTTON_RIGHT_1);
    SOC_IO_ISR_Enable(BUTTON_RIGHT_2);

    return 0;
}

/*
static struct dev_pm_ops knob_pm_ops =
{
    .suspend	= knob_suspend,
    .resume	= knob_resume,
};
*/


/**
 * knob_init - init knob processing work
 *
 * init knob processing work.
 *
 */
void knob_init(void)
{
	int button_en;

	lidbg("knob_init\n");
	FS_REGISTER_INT(button_en, "button_en", 1, NULL);
	pfly_KeyEncoderInfo->iEncoderLeftIncCount = 0;
	pfly_KeyEncoderInfo->iEncoderLeftDecCount = 0;
	pfly_KeyEncoderInfo->iEncoderRightIncCount = 0;
	pfly_KeyEncoderInfo->iEncoderRightDecCount = 0;
	if(button_en)
	{
#if defined(EXTI_USE_DELAYED)
	p_work_encode_queue = create_singlethread_workqueue("encode_knob_queue");
	INIT_DELAYED_WORK(&pfly_KeyEncoderInfo->encoder_work, work_knob_fn);
#else
	INIT_WORK(&pfly_KeyEncoderInfo->encoder_work, work_knob_fn);
#endif

#ifdef SOC_mt3360
#else
	SOC_IO_Input(BUTTON_LEFT_1, BUTTON_LEFT_1, GPIO_CFG_PULL_UP);
	SOC_IO_Input(BUTTON_LEFT_2, BUTTON_LEFT_2, GPIO_CFG_PULL_UP);
	SOC_IO_Input(BUTTON_RIGHT_1, BUTTON_RIGHT_1, GPIO_CFG_PULL_UP);
	SOC_IO_Input(BUTTON_RIGHT_2, BUTTON_RIGHT_2, GPIO_CFG_PULL_UP);
#endif
	if(g_hw.is_single_edge)
	{
		lidbg("---------single_edge add IO isr----------");
		SOC_IO_ISR_Add(BUTTON_LEFT_1, IRQF_TRIGGER_RISING, irq_left_knob1, NULL);
		SOC_IO_ISR_Add(BUTTON_LEFT_2,  IRQF_TRIGGER_FALLING, irq_left_knob2, NULL);
		SOC_IO_ISR_Add(BUTTON_RIGHT_1, IRQF_TRIGGER_RISING , irq_right_knob1, NULL);
		SOC_IO_ISR_Add(BUTTON_RIGHT_2,  IRQF_TRIGGER_FALLING, irq_right_knob2, NULL);
	}
	else
	{
		lidbg("---------both_edge add IO isr----------");
		SOC_IO_ISR_Add(BUTTON_LEFT_1, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING , irq_left_knob1, NULL);
		SOC_IO_ISR_Add(BUTTON_LEFT_2, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, irq_left_knob2, NULL);
		SOC_IO_ISR_Add(BUTTON_RIGHT_1, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, irq_right_knob1, NULL);
		SOC_IO_ISR_Add(BUTTON_RIGHT_2, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, irq_right_knob2, NULL);
	}
    }
}

/**
 * thread_knob_init - init thread
 * @data:data put into thread.
 *
 * the thread invoke knob_init().
 *
 */
int thread_knob_init(void *data)
{
	//knob_init();
	if(g_var.is_fly == 0)
	{
		knob_init();//temp for product commit
		//INIT_WORK(&process_work, work_process);
		//schedule_work(&process_work);
		init_completion(&origin_completion);
		CREATE_KTHREAD(thread_process, NULL);
	}	
	return 0;
}

/**
 * knob_poll - poll function
 * @filp:module file struct
 * @wait:poll table
 *
 * poll function.
 *
 */
static unsigned int knob_poll(struct file *filp, struct poll_table_struct *wait)
{
    //struct gps_device *dev = filp->private_data;
    struct fly_KeyEncoderInfo *pfly_KeyEncoderInfo = filp->private_data;
    unsigned int mask = 0;
    pr_debug("[knob_poll]wait begin\n");
    poll_wait(filp, &pfly_KeyEncoderInfo->wait_queue, wait);
    pr_debug("[knob_poll]wait done\n");
    down(&pfly_KeyEncoderInfo->sem);
    if(!kfifo_is_empty(&knob_data_fifo))
    {
        mask |= POLLIN | POLLRDNORM;
    }
    up(&pfly_KeyEncoderInfo->sem);
    return mask;
}

/**
 * knob_probe - probe function
 * @pdev:platform_device
 *
 * probe function.
 *
 */
static int  knob_probe(struct platform_device *pdev)
{
	int ret;
	DUMP_FUN;
	// 1creat cdev
	pfly_KeyEncoderInfo = (struct fly_KeyEncoderInfo *)kmalloc( sizeof(struct fly_KeyEncoderInfo), GFP_KERNEL );
	if (pfly_KeyEncoderInfo == NULL)
	{
	    ret = -ENOMEM;
	    lidbg("[knob]:kmalloc err\n");
	    return ret;
	}
	lidbg_new_cdev(&knob_fops, "knob");//add cdev

	// 2init all the tools
	init_waitqueue_head(&pfly_KeyEncoderInfo->wait_queue);
	sema_init(&pfly_KeyEncoderInfo->sem, 1);
	kfifo_init(&knob_data_fifo, knob_fifo_buffer, FIFO_SIZE);
	lidbg_chmod("/dev/knob0");

	// 3creat thread
	CREATE_KTHREAD(thread_knob_init, NULL);
	return 0;
}

/**
 * knob_probe - probe function
 * @filp:file struct
 * @buf:user buffer
 * @count:bytes count
 * @f_pos:file pos
 *
 * read function.
 *
 */
ssize_t knob_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct fly_KeyEncoderInfo *pfly_KeyEncoderInfo = filp->private_data;
	int read_len,fifo_len,bytes;
	pr_debug("knob read start.\n");
	if(kfifo_is_empty(&knob_data_fifo))
	{
	    if(wait_event_interruptible(pfly_KeyEncoderInfo->wait_queue, !kfifo_is_empty(&knob_data_fifo)))
	        return -ERESTARTSYS;
	}
	down(&pfly_KeyEncoderInfo->sem);
	

	fifo_len = kfifo_len(&knob_data_fifo);
	
	
	if(count > HAL_BUF_SIZE )
	    read_len = HAL_BUF_SIZE;
	else
	    read_len = count;
	 
	 /*
	if((count > HAL_BUF_SIZE) &  (fifo_len > HAL_BUF_SIZE))
	    read_len = HAL_BUF_SIZE;
	else if (count > fifo_len )
	    read_len = fifo_len;
	else
	    read_len = count;
	*/

	bytes = kfifo_out(&knob_data_fifo, knob_data_for_hal, read_len);
	up(&pfly_KeyEncoderInfo->sem);

	if(copy_to_user(buf, &knob_data_for_hal, read_len))
	{
	    return -1;
	}

	if(fifo_len > bytes)
	    wake_up_interruptible(&pfly_KeyEncoderInfo->wait_queue);

	return read_len;
}

/**
 * knob_write - write function
 * @filp:file struct
 * @buf:user buffer
 * @count:bytes count
 * @f_pos:file pos
 *
 * write function.
 *
 */
ssize_t knob_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	char *cmd[8] = {NULL};
	int cmd_num  = 0;
	char cmd_buf[512];
	memset(cmd_buf, '\0', 512);

	if(copy_from_user(cmd_buf, buf, count))
	{
	    lidbg("copy_from_user ERR\n");
	}
	if(cmd_buf[count - 1] == '\n')
	    cmd_buf[count - 1] = '\0';
	lidbg("-----FLYSTEP------------------[%s]---\n", cmd_buf);

	cmd_num = lidbg_token_string(cmd_buf, " ", cmd) ;

	if(!strcmp(cmd[0], "process"))
	{
		lidbg("case:[%s]\n", cmd[0]);
		isProcess = isProcess ? 0:1;
	}
	
	return count;
}

/**
 * knob_open - open function
 * @inode:inode
 * @filp:file struct
 *
 * open function.
 *
 */
int knob_open (struct inode *inode, struct file *filp)
{
    filp->private_data = pfly_KeyEncoderInfo;
    lidbg("[knob]knob_open\n");
    return 0;
}

/**
 * knob_remove - remove function
 * @pdev:platform_device
 *
 * remove function.
 *
 */
static int  knob_remove(struct platform_device *pdev)
{
    return 0;
}

static  struct file_operations knob_fops =
{
    .owner = THIS_MODULE,
    .read = knob_read,
    .write = knob_write,
    .poll = knob_poll,
    .open = knob_open,
};

static struct platform_driver knob_driver =
{
    .probe		= knob_probe,
    .remove     = knob_remove,
    .suspend	= knob_suspend,
    .resume	= knob_resume,
    .driver         = {
    .name = "lidbg_knob",
    .owner = THIS_MODULE,
    },
};

static struct platform_device lidbg_knob_device =
{
    .name               = "lidbg_knob",
    .id                 = -1,
};

static int  knob_dev_init(void)
{
	printk(KERN_WARNING "knob chdrv_init\n");
	lidbg("hello_knob\n");
          
        LIDBG_GET;
	platform_device_register(&lidbg_knob_device);
  	return platform_driver_register(&knob_driver);
}

static void  knob_dev_exit(void)
{
	printk("knob chdrv_exit\n");
}

//EXPORT_SYMBOL(knob_suspend);
//EXPORT_SYMBOL(knob_resume);
module_init(knob_dev_init);
module_exit(knob_dev_exit);



MODULE_AUTHOR("fly, <fly@gmail.com>");
MODULE_DESCRIPTION("Devices Driver");
MODULE_LICENSE("GPL");
