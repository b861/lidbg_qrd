 /*
"com.android.flyaudioui",
"cn.flyaudio.android.flyaudioservice",
"cn.flyaudio.navigation", 
"com.android.launcher",
"cn.flyaudio.osd.service", 
"android.process.acore",
"android.process.media", 
"com.android.systemui",
"com.android.deskclock", 
"sys.DeviceHealth", 
"system",
"com.fly.flybootservice"
*/

{
static int j,skip;
static char *lmk_white_list[] =
{
	"d.process.acore",
	"ndroid.launcher",
	"d.process.media",
	"roid.flyaudioui",
	"flyaudioservice",
	".flybootservice",
	"dio.flyaudioram",
	"mediatorservice",
	"droid.launcher3",
	".flyaudio.media",
	"ndroid.systemui",
	"lyaudio.Weather",
	"oadcastreceiver",
	"c2739.mainframe",
	"alcomm.fastboot",
	"udio.navigation",
	"dio.osd.service",
	"droid.deskclock",
	"ys.DeviceHealth",
	"io.proxyservice",
	"mediaserver",
	"system_server",
	"com.sygic.aura",
	"oodocom.gocsdk",
	"system",
	"logcat",
	NULL,
};


		skip = 0;
		for(j = 0; lmk_white_list[j] != NULL; j++)
		{
	
			if (strncmp(p->comm, lmk_white_list[j], sizeof(lmk_white_list[j]) - 1) == 0)
			{
				pr_debug("lmkp:protect %s\n",lmk_white_list[j]);
				skip = 1;
				break;
			}
			
			if (strstr(p->comm, "fly"))
			{
				pr_debug("lmkp:protect %s\n",p->comm);
				skip = 1;
				break;
			}
			
			if (strstr(p->comm, "qualcomm"))
			{
				pr_debug("lmkp:protect %s\n",p->comm);
				skip = 1;
				break;
			}
			
			if (strstr(p->comm, "lidbg"))
			{
				pr_debug("lmkp:protect %s\n",p->comm);
				skip = 1;
				break;
			}

			
		}
		
		if(skip == 1)
		{
			task_unlock(p);
			continue;	
		}
}
