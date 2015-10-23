
//	Manage.c (kernel\irq)	


//	flyaudio+               
//	#include <../drivers/flyaudio/wakeup_irq_manager.c>
	{
	    static char *white_list[] =
	    {
		"qpnp_kpdpwr_status",
		NULL,
	    };
	    static int irqall = 0, irqskipd = 0, j = 0;
	    if (desc->action && desc->action->name)
	    {
		irqall++;
		for(j = 0; white_list[j] != NULL; j++)
		{
		    if(strncmp(desc->action->name, white_list[j], sizeof(white_list[j]) - 1) == 0)
		    {
			printk( KERN_CRIT  "fuirq.name.protect============%d/%d[%s]\n", irqskipd, irqall, desc->action->name);
			break;
		    }
		    else
		    {
		        irqskipd++;
		        printk( KERN_CRIT  "fuirq.name.skip============%d/%d[%s]\n", irqskipd, irqall, desc->action->name);
		        irq_put_desc_busunlock(desc, flags);
		        return 0;
		    }
		}
	    }
	    else
		printk( KERN_CRIT "fuirq.name.null============[null,%d]\n",irq);
	}


