
#include "lidbg.h"
#define AD_OFFSET  (100000)
#define AD_VAL_MAX  (3300000)
extern int is_key_scan_en;
int ad_en;
struct ad_key_remap
{
    u32 ch;
    u32 ad_value;
    u32 key;
};
static struct ad_key_remap ad_key[] =
{

    //feel  key  x_ain_3   mpp4
    //{35, 1000000, KEY_BACK},
    //{35, 1500000, KEY_BACK},
    //{35, 2000000, KEY_BACK},
    {35, 2200000, KEY_HOME},
    //feel  key  x_ain_4  mpp6
    //{37, 1000000, KEY_BACK},
    //{37, 1500000, KEY_BACK},
    {37, 2000000, KEY_MENU},
    {37, 2200000, KEY_BACK},
};
int find_ad_key(u32 ch)
{
    int val = 0;
    int i;

    SOC_ADC_Get(ch, &val);
    if(val > AD_VAL_MAX)
        return 0xff;

    for(i = 0; i < SIZE_OF_ARRAY(ad_key); i++)
    {
        if(ch == ad_key[i].ch)
            if((val > ad_key[i].ad_value - AD_OFFSET) && (val < ad_key[i].ad_value + AD_OFFSET))
            {
                lidbg("find_ad_key:ch%d=%d,key_id=%d,sendkey=%d\n", ch, val, i, ad_key[i].key);
                return i;
            }
    }

    return 0xff;

}

void key_scan(void)
{
    static int old_key = 0xff;
    int key = 0xff;
    key = find_ad_key(35);
    if(key != 0xff) goto find_key;
    key = find_ad_key(37);
    if(key != 0xff) goto find_key;

find_key:

    if((old_key != 0xff) && (key == 0xff))
    {
        SOC_Key_Report(ad_key[old_key].key, KEY_PRESSED_RELEASED);
    }
    old_key = key;
}
int thread_key(void *data)
{  
    FS_REGISTER_INT(ad_en, "ad_en", 0, NULL);
    if(ad_en)
    while(1)
    {
        if(is_key_scan_en)
        {
            key_scan();
            msleep(100);
        }
        else
            msleep(1000);
    }
    return 0;
}
