

static DEFINE_MUTEX(early_suspend_lock);
static LIST_HEAD(early_suspend_handlers);

/*
struct early_suspend {
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct list_head link;
	int level;
	void (*suspend)(struct early_suspend *h);
	void (*resume)(struct early_suspend *h);
#endif
};
*/


void fake_register_early_suspend(struct early_suspend *handler)
{
	struct list_head *pos;
#if 1
	mutex_lock(&early_suspend_lock);
	list_for_each(pos, &early_suspend_handlers) {
		struct early_suspend *e;
		e = list_entry(pos, struct early_suspend, link);
		if (e->level > handler->level)
			break;
	}
	list_add_tail(&handler->link, pos);
	mutex_unlock(&early_suspend_lock);
#endif
}


static void fake_early_suspend()
{
	struct early_suspend *pos;
	list_for_each_entry(pos, &early_suspend_handlers, link) 
	{
		if (pos->suspend != NULL) 
		{
			
			lidbg("early_suspend: calling %pf\n", pos->suspend);
			pos->suspend(pos);
		}
	}
}

static void fake_late_resume()
{
	struct early_suspend *pos;

	list_for_each_entry_reverse(pos, &early_suspend_handlers, link) 
	{
		if (pos->resume != NULL) 
		{
			lidbg("late_resume: calling %pf\n", pos->resume);
			pos->resume(pos);
		}
	}
}




int fake_suspend(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{
	
	DUMP_FUN_ENTER;
	if(is_fake_suspend == 0)
	{
		is_fake_suspend = 1;
		
		fastboot_set_status(PM_STATUS_READY_TO_FAKE_PWROFF);

		fake_early_suspend();
		fastboot_task_kill_exclude(kill_exclude_process_fake_suspend);

		//SOC_Write_Servicer(SUSPEND_KERNEL);
	}
	DUMP_FUN_LEAVE;

	return sprintf(buf, "is_fake_suspend:%d\n", is_fake_suspend);
}

void create_proc_entry_fake_suspend()
{
    create_proc_read_entry("fake_suspend", 0, NULL, fake_suspend, NULL);

}

int fake_wakeup(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{

	DUMP_FUN_ENTER;
	if(is_fake_suspend == 1)
	{
		is_fake_suspend = 0;
		fake_late_resume();
		//SOC_Write_Servicer(WAKEUP_KERNEL);
	}
	DUMP_FUN_LEAVE;
   
   return sprintf(buf, "is_fake_suspend:%d\n", is_fake_suspend);
   
}

void create_proc_entry_fake_wakeup()
{
    create_proc_read_entry("fake_wakeup", 0, NULL, fake_wakeup, NULL);

}











