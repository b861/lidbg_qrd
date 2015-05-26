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
	"ndroid.launcher",
	"d.process.media",
	"roid.flyaudioui",
	"flyaudioservice",
	".flybootservice",
	"dio.flyaudioram",
	"mediatorservice",
	"droid.launcher3",
	".flyaudio.media",
	"mediaserver",
	"ndroid.systemui",
	"system_server",
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
		}
		
		if(skip == 1)
		{
			task_unlock(p);
			continue;	
		}
}
