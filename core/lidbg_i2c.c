
/*i2c new style driver (Standard driver) */

#include "lidbg.h"

//#define I2C_NEW_STYLE
//#define  USE_I2C_LOCK
#define I2C_API_FAKE_ADDR 0x7f
#define I2C_MINORS	      256

int  i2c_api_attach(struct i2c_adapter *adapter);
int  i2c_api_detach(struct i2c_adapter *adapter);

struct i2c_api
{
    struct list_head list;
    struct i2c_client *client;
};

static LIST_HEAD(i2c_api_list);
static DEFINE_SPINLOCK(i2c_api_list_lock);

static const unsigned short normal_addr[] = { I2C_API_FAKE_ADDR, I2C_CLIENT_END };
static const unsigned short ignore[]      = { I2C_CLIENT_END };

int lidbg_i2c_running = 0;//for use i2c in interrupt isr(touch etc.)


#if 0

static struct i2c_client_address_data addr_data =
{
    /* Addresses to scan */
    .normal_i2c = normal_addr,//���ָ��ָ������̽��ĵ�ַ���飬����ÿһ����������̽���������ĵ�ַ��
    .probe      = ignore,//List of adapter,address pairs to scan additionally���ָ��ָ����ڴ�ÿ����Ԫ��Ϊһ�飬ǰһ�������������ţ���һ�������ַ��ʹ�����ָ�����ʵ����ָ������������ָ����ַ����̽�⡣
    .ignore     = ignore,//List of adapter,address pairs not to scan���ָ��ָ�򽫱����Եĵ�ַ���ڶ�i2c_normal�еĵ�ַ̽��ǰ�����Ȳ鿴�Ƿ������������飬ֻ������������в����ڵĵ�ַ����̽�⡣���ָ��ָ�������Ҳ��ÿ���������ź����һ����ַ��
    .forces     = NULL,//List of adapter,address pairs to boldly assume to be presentǿ��ʹ�õĵ�ַ������������̽���ʹ�á�
    //����̽��ĺ��������������źŲ����ܷ��յ���Ӧ��ȷ��ĳ����ַ������豸�Ѿ��������������ˡ����ָ��ָ��һ��ָ�����飬ÿһ����Ա��ָ������Ҳ��ÿ���������ź����һ����ַ��
};
#endif


#ifdef USE_I2C_LOCK
static struct mutex i2c_lock;
#endif

#ifdef I2C_NEW_STYLE
static const struct i2c_device_id id[] =
{
    {"I2C-API", 0},
    {}
};
MODULE_DEVICE_TABLE(i2c, id);
#endif

static struct i2c_driver i2c_api_driver =
{
    //����device��drive��� ���client�����ֺ�driver->id_table[]�е�����ƥ�伴Ϊ�ɹ�
#ifdef I2C_NEW_STYLE
    .id_table       = id,
#endif
    .attach_adapter = i2c_api_attach,
    .detach_adapter	= i2c_api_detach,
    .command        = NULL,
    .driver         = {
        .name  = "I2C-API",
        .owner = THIS_MODULE,
    },

#ifdef I2C_NEW_STYLE
    .address_data   = &addr_data, /* Addresses to scan */
#endif

 };

#ifdef LIDBG_I2C_GPIO 

static struct i2c_gpio_platform_data i2c_gpio_pdata = {
	.sda_pin		= LIDBG_I2C_GPIO_SDA,
	.sda_is_open_drain	= 0,
	.scl_pin		= LIDBG_I2C_GPIO_SCL,
	.scl_is_open_drain	= 1,
	.udelay			= LIDBG_I2C_DEFAULT_DELAY,		/* 10 kHz */
};

static struct platform_device i2c_gpio_device = {
	.name			= "i2c-gpio",
	.id			= LIDBG_I2C_BUS_ID,		//will be used to set the i2c_bus number
	.dev			={
		.platform_data	= &i2c_gpio_pdata,
	},
};
#endif


static struct i2c_api *get_i2c_api(int bus_id)
{
    //������ϵͳ�п��ܴ��ڶ��adapter,��Ϊ��ÿһ��I2C���߶�Ӧһ�����,�����г�ΪI2C���ߺ�.
    //������ߺŵ�PCI�е����ߺŲ�ͬ.����Ӳ���޹�,ֻ������ϱ������ֶ���
    //����i2c_add_adapter()����,��ʹ�õ��Ƕ�̬���ߺ�,����ϵͳ�������һ�����ߺ�

    struct i2c_api *i2c_api;

    spin_lock(&i2c_api_list_lock);
    list_for_each_entry(i2c_api, &i2c_api_list, list)
    {
        if (i2c_api->client->adapter->nr == bus_id)
            goto found;
    }
    i2c_api = NULL;

found:
    spin_unlock(&i2c_api_list_lock);
    return i2c_api;
}

static struct i2c_api *add_i2c_api(struct i2c_client *client)
{
    struct i2c_api *i2c_api;

    if (client->adapter->nr >= I2C_MINORS)
    {
        lidbg(KERN_ERR "i2c_api: Out of device minors (%d)\n",
              client->adapter->nr);
        return NULL;
    }

    i2c_api = kzalloc(sizeof(*i2c_api), GFP_KERNEL);
    if (!i2c_api)
        return NULL;
    i2c_api->client = client;
    // init_waitqueue_head();
    spin_lock(&i2c_api_list_lock);
    list_add_tail(&i2c_api->list, &i2c_api_list);
    spin_unlock(&i2c_api_list_lock);
    return i2c_api;
}

static void del_i2c_api(struct i2c_api *i2c_api)
{
    spin_lock(&i2c_api_list_lock);
    list_del(&i2c_api->list);
    spin_unlock(&i2c_api_list_lock);
    kfree(i2c_api);
}
int i2c_api_set_rate(int  bus_id, int rate)
{
    struct i2c_api *i2c_api = NULL ;
    struct i2c_algo_bit_data *adap1=i2c_api->client->adapter->algo_data;
    lidbg_i2c_running = 1; // for touch intr
	#ifdef USE_I2C_LOCK
    mutex_lock(&i2c_lock);
	#endif
   i2c_api = get_i2c_api(bus_id);
   if (!i2c_api)
        return -ENODEV;
	 
	if(adap1!=NULL){
		
			(*adap1).udelay=rate;	
			//printk("\n*****Set the i2c_rate sucessuful !\n\n");
			return 0;
	 }
	else{
		//printk("\n*****Set the i2c_rate not sucessuful !\n\n");
	}
	
	#ifdef USE_I2C_LOCK
    mutex_unlock(&i2c_lock);
	#endif
    lidbg_i2c_running = 0;
	return 0;

}
static int i2c_api_do_xfer(int bus_id, char chip_addr, unsigned int sub_addr, int mode,
                           char *buf, unsigned int size)
{
    /**
     * you can define more transfer mode here, implement it below.
     */
#define I2C_API_XFER_MODE_SEND            0x0 /* standard send */
#define I2C_API_XFER_MODE_RECV            0x1 /* standard receive */
#define I2C_API_XFER_MODE_SEND_NO_SUBADDR 0x2 /* send with no sub address */
#define I2C_API_XFER_MODE_RECV_NO_SUBADDR 0x3 /* receive with no sub address */
#define I2C_API_XFER_MODE_RECV_SUBADDR_2BYTES 0x7
#define I2C_API_XFER_MODE_RECV_SUBADDR_3BYTES 0x8

#define I2C_API_XFER_MODE_RECV_SAF7741    0x4 /* receive for SAF7741 */
#define I2C_API_XFER_MODE_SEND_TEF7000    0x5 /* send for TEF7000 */
#define I2C_API_XFER_MODE_RECV_TEF7000    0x6 /* receive for TEF7000 */

    int ret = 0;
    struct i2c_api *i2c_api ;

    lidbg_i2c_running = 1; // for touch intr

#ifdef USE_I2C_LOCK
    mutex_lock(&i2c_lock);
#endif

    i2c_api = get_i2c_api(bus_id);

    if (!i2c_api)
        return -ENODEV;

    i2c_api->client->addr = chip_addr;

#ifdef SOC_RK2818
    lidbg("SOC_RK2818\n");

    i2c_api->client->mode = NORMALMODE;
    i2c_api->client->Channel = I2C_CH1;
    i2c_api->client->addressBit = I2C_7BIT_ADDRESS_8BIT_REG;
    i2c_api->client->speed = 200;
    i2c_api->client->driver = &i2c_api_driver;
#endif

    switch (mode)
    {
    case I2C_API_XFER_MODE_SEND:
    {
        struct i2c_adapter *adap = i2c_api->client->adapter;
        struct i2c_msg msg;

        msg.addr = i2c_api->client->addr;
        msg.flags = 0;
        msg.len = size;
        msg.buf = buf;

        ret = i2c_transfer(adap, &msg, 1);
        break;
    }

    case I2C_API_XFER_MODE_RECV:
    {
        struct i2c_adapter *adap = i2c_api->client->adapter;
        struct i2c_msg msg[2];
        char subaddr;
        subaddr = sub_addr & 0xff;

        msg[0].addr = i2c_api->client->addr;
        msg[0].flags = 0;
        msg[0].len = 1;
        msg[0].buf = &subaddr;

        msg[1].addr = i2c_api->client->addr;
        msg[1].flags = I2C_M_RD;
        msg[1].len = size;
        msg[1].buf = buf;


        ret = i2c_transfer(adap, msg, 2);
        break;
    }
    case I2C_API_XFER_MODE_SEND_NO_SUBADDR:
        ret = i2c_master_send(i2c_api->client, buf, size);
        break;

    case I2C_API_XFER_MODE_RECV_SUBADDR_2BYTES:
    {
        struct i2c_adapter *adap = i2c_api->client->adapter;
        struct i2c_msg msg[2];
        char SubAddr[2];

        SubAddr[0] = (sub_addr >> 8) & 0xff;
        SubAddr[1] = sub_addr & 0xff;

        msg[0].addr = i2c_api->client->addr;
        msg[0].flags = 0;
        msg[0].len = 2;
        msg[0].buf = SubAddr;

        msg[1].addr = i2c_api->client->addr;
        msg[1].flags = I2C_M_RD;
        msg[1].len = size;
        msg[1].buf = buf;

        ret = i2c_transfer(adap, msg, 2);
        break;

    }

    case I2C_API_XFER_MODE_RECV_NO_SUBADDR:
    {
        struct i2c_adapter *adap = i2c_api->client->adapter;
        struct i2c_msg msg[2];

        msg[0].addr = i2c_api->client->addr;
        msg[0].flags = I2C_M_RD;
        msg[0].len = size;
        msg[0].buf = buf;
        ret = i2c_transfer(adap, msg, 1);
        break;
    }

    case I2C_API_XFER_MODE_RECV_SAF7741:
    case I2C_API_XFER_MODE_RECV_SUBADDR_3BYTES:

    {
        struct i2c_adapter *adap = i2c_api->client->adapter;
        struct i2c_msg msg[2];
        char SubAddr[3];
        SubAddr[0] = (sub_addr >> 16) & 0xff;
        SubAddr[1] = (sub_addr >> 8) & 0xff;
        SubAddr[2] = sub_addr & 0xff;

        msg[0].addr = i2c_api->client->addr;
        msg[0].flags = 0;
        msg[0].len = 3;
        msg[0].buf = SubAddr;

        msg[1].addr = i2c_api->client->addr;
        msg[1].flags = I2C_M_RD;
        msg[1].len = size;
        msg[1].buf = buf;

        ret = i2c_transfer(adap, msg, 2);
        break;
    }

#define SAF7741_I2C_ADDR_FOR_TEF7000  0x38

    case I2C_API_XFER_MODE_SEND_TEF7000:
    {
        struct i2c_adapter *adap = i2c_api->client->adapter;
        struct i2c_msg msg[2];
        char SubAddr[3];
        SubAddr[0] = 0x00;
        SubAddr[1] = 0xff;
        SubAddr[2] = 0xff;

        msg[0].addr = (SAF7741_I2C_ADDR_FOR_TEF7000 >> 1);
        msg[0].flags = 0;
        msg[0].len = 3;
        msg[0].buf = SubAddr;

        msg[1].addr = i2c_api->client->addr;
        msg[1].flags = 0;
        msg[1].len = size;
        msg[1].buf = buf;

        ret = i2c_transfer(adap, msg, 2);
        break;
    }

    case I2C_API_XFER_MODE_RECV_TEF7000:
    {
        struct i2c_adapter *adap = i2c_api->client->adapter;
        struct i2c_msg msg[2];
        char SubAddr[3];
        SubAddr[0] = 0x00;
        SubAddr[1] = 0xff;
        SubAddr[2] = 0xff;


        msg[0].addr = (SAF7741_I2C_ADDR_FOR_TEF7000 >> 1);
        msg[0].flags = 0;
        msg[0].len = 3;
        msg[0].buf = SubAddr;

        msg[1].addr = i2c_api->client->addr;
        msg[1].flags = I2C_M_RD;
        msg[1].len = size;
        msg[1].buf = buf;

        ret = i2c_transfer(adap, msg, 2);
        break;

    }

    default:
        return -EINVAL;
    }
#ifdef USE_I2C_LOCK
    mutex_unlock(&i2c_lock);
#endif
    lidbg_i2c_running = 0;

    return ret;
}

void mod_i2c_main(int argc, char **argv)
{
    if(argc < 1)
    {
        lidbg("Usage:\n");
        lidbg("r bus_id dev_addr start_reg num\n");
        lidbg("w bus_id dev_addr num data1 data2 ... \n");
        lidbg("r_all bus_id dev_addr num\n");

        lidbg("r_mult_sub_addr  bus_id  dev_addr sub_addr_bytes  start_reg num\n");

        return;

    }

    if(!strcmp(argv[0], "w"))
    {
        u32 bus_id, dev_addr, num, i;
        char *psend_data;

        bus_id = simple_strtoul(argv[1], 0, 0);
        dev_addr = simple_strtoul(argv[2], 0, 0);
        num = simple_strtoul(argv[3], 0, 0);

        if(argc - 4 < num)
        {
            lidbg("input num err:\n");
            return;
        }
        psend_data = (char *)kzalloc(num, GFP_KERNEL);
        for(i = 0; i < num; i++)
        {

            psend_data[i] = (char)simple_strtoul(argv[i+4], 0, 0);
        }


#if 1
        lidbg("\nmod_i2c_main:w ");
        printk("bus_id %d;", bus_id);
        printk("dev_addr(8bit)=0x%x<<1%x;", dev_addr, dev_addr << 1);
        printk("reg 0x%x;", psend_data[0]);
        printk("data_num %d;\n", num); //����reg��data

        lidbg("data: ");
        for(i = 0; i < num; i++)
        {
            printk("%x  ", psend_data[i]);
        }

        printk("\n");

#endif

        //��ַ���͵�ʱ��Ҫ����һλ
        // i2c_api_do_send(bus_id, dev_addr, psend_data[0], &psend_data[0], num ); /*How many bytes to write ��ȥdev_addr��reg��ַ��*/
        i2c_api_do_send(bus_id, dev_addr, 0, &psend_data[0], num ); /*How many bytes to write ��ȥdev_addr��reg��ַ��*/
        kfree(psend_data);

    }
    else if(!strcmp(argv[0], "r"))
    {

        u32 bus_id, dev_addr, num, i, start_reg;
        char *psend_data;

        // lidbg("r bus_id dev_addr start_reg num\n");
        bus_id = simple_strtoul(argv[1], 0, 0);
        dev_addr = simple_strtoul(argv[2], 0, 0);
        start_reg = simple_strtoul(argv[3], 0, 0);
        num = simple_strtoul(argv[4], 0, 0);
#if 1
        lidbg("\nmod_i2c_main:r ");
        printk("bus_id %d;", bus_id);
        printk("dev_addr(8bit)=0x%x(7bit)<<1=0x%x;", dev_addr, dev_addr << 1);
        printk("start_reg 0x%x;", start_reg);
        printk("data_num %d;\n\n", num); //����reg��data
#endif

        psend_data = (char *)kzalloc(num, GFP_KERNEL);

        i2c_api_do_recv(bus_id, dev_addr, start_reg, psend_data, num );

        for(i = 0; i < num; i++)
        {
            printk("reg 0x%x = 0x%x\n", start_reg + i, psend_data[i]);

        }

        kfree(psend_data);

    }
    else if(!strcmp(argv[0], "r_all"))
    {
        u32 bus_id, dev_addr, num, i, start_reg;
        char *psend_data;

        bus_id = simple_strtoul(argv[1], 0, 0);
        dev_addr = simple_strtoul(argv[2], 0, 0);
        start_reg = simple_strtoul(argv[3], 0, 0);
        num = simple_strtoul(argv[4], 0, 0);
        psend_data = (char *)kzalloc(num, GFP_KERNEL);

        for(i = 0; i < num; i++)
        {
            i2c_api_do_recv(bus_id, dev_addr, i, psend_data + i, 1 );

            printk("reg %x = %x\n", start_reg + i, psend_data[i]);

        }

        kfree(psend_data);

    }

    else if(!strcmp(argv[0], "r_mult_sub_addr"))
    {

        u32 bus_id, dev_addr, num, i, start_reg, sub_addr_bytes;
        char *psend_data;

        // lidbg("r_mult_sub_addr  bus_id  dev_addr sub_addr_bytes  start_reg num\n");
        bus_id = simple_strtoul(argv[1], 0, 0);
        dev_addr = simple_strtoul(argv[2], 0, 0);
        sub_addr_bytes  = simple_strtoul(argv[3], 0, 0);
        start_reg = simple_strtoul(argv[4], 0, 0);
        num = simple_strtoul(argv[5], 0, 0);
#if 1
        lidbg("\nmod_i2c_main:r_mult_sub_addr ");
        printk("bus_id %d;", bus_id);
        printk("dev_addr 0x%x<<1=0x%x;", dev_addr, dev_addr << 1);
        printk("sub_addr_bytes %d;", sub_addr_bytes);
        printk("start_reg %d;", start_reg);
        printk("data_num %d;\n\n", num); //����reg��data
#endif

        psend_data = (char *)kzalloc(num, GFP_KERNEL);

        if(sub_addr_bytes == 2)
            i2c_api_do_recv_sub_addr_2bytes(bus_id, dev_addr, start_reg, psend_data, num );
        else if(sub_addr_bytes == 3)
            i2c_api_do_recv_sub_addr_3bytes(bus_id, dev_addr, start_reg, psend_data, num );

        for(i = 0; i < num; i++)
        {
            printk("reg %x = %x\n", start_reg + i, psend_data[i]);

        }

        kfree(psend_data);
    }
    else if(!strcmp(argv[0], "probe"))
    {
#define PROBE_I2C_BUS  (1)
        int rc, i, j = 0;
        u8 *i2c_devices;
        u8 tmp[8];

#define PROBE_INTERVAL_TIME (20)
#define I2C_DEVICES_MAX (32)

        i2c_devices = kmalloc(I2C_DEVICES_MAX, GFP_KERNEL);
		if (i2c_devices == NULL)
		{
			LIDBG_ERR("kmalloc.\n");		
		}

        memset(i2c_devices, 0, I2C_DEVICES_MAX);


        for(i = 1; i<(0xff >> 1); i++)
        {
            rc = i2c_api_do_recv(PROBE_I2C_BUS, i, 0, tmp, 1 );

            if (rc >= 0)
            {
                i2c_devices[j] = i;
                j++;
                lidbg("\n\n\n\ni2c_addr 0x%x probe!\n\n\n\n", i);
            }
            else
            {
                lidbg("0x%x ......\n", i);
                msleep(PROBE_INTERVAL_TIME);
            }
        }
        printk("\n");
        lidbg("find <%d> i2c_devices:", j);
        for(i = 0; i < j; i++)
        {
            printk("0x%x, ", i2c_devices[i]);

        }
        printk("\n");
        printk("\n");
        kfree( i2c_devices);
    }

}


//��ַ���͵�ʱ��Ҫ����һλ
int i2c_api_do_send(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return i2c_api_do_xfer(bus_id, chip_addr, sub_addr, I2C_API_XFER_MODE_SEND, buf, size);
}

int i2c_api_do_recv(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return i2c_api_do_xfer(bus_id, chip_addr, sub_addr, I2C_API_XFER_MODE_RECV, buf, size);
}

int i2c_api_do_recv_no_sub_addr(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return i2c_api_do_xfer(bus_id, chip_addr, sub_addr, I2C_API_XFER_MODE_RECV_NO_SUBADDR, buf, size);
}


int i2c_api_do_recv_sub_addr_2bytes(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return i2c_api_do_xfer(bus_id, chip_addr, sub_addr, I2C_API_XFER_MODE_RECV_SUBADDR_2BYTES, buf, size);
}


int i2c_api_do_recv_sub_addr_3bytes(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return i2c_api_do_xfer(bus_id, chip_addr, sub_addr, I2C_API_XFER_MODE_RECV_SUBADDR_3BYTES, buf, size);
}

int i2c_api_do_recv_SAF7741(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return i2c_api_do_xfer(bus_id, chip_addr, sub_addr, I2C_API_XFER_MODE_RECV_SAF7741, buf, size);
}

int i2c_api_do_send_TEF7000(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return i2c_api_do_xfer(bus_id, chip_addr, sub_addr, I2C_API_XFER_MODE_SEND_TEF7000, buf, size);
}

int i2c_api_do_recv_TEF7000(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return i2c_api_do_xfer(bus_id, chip_addr, sub_addr, I2C_API_XFER_MODE_RECV_TEF7000, buf, size);
}

int i2c_api_attach(struct i2c_adapter *adap)//��ѭ�����ã���������adapter
{
    struct i2c_board_info info;//����I2C������Ϣ
    struct i2c_client *client;
    /*

    ͨ��i2c_register_board_info������i2c�豸��̬�õǼǵ�ϵͳ�С�
    ��ʵ��I2C��֧��Ӳ��̽�⣬���ܿ��������豸�ܹ�ʹ��I2C_SMBUS_QUICK��֪�ڸ����ĵ�ַ���Ƿ��и��豸��
    ����ͨ����Ҫ�������Ϣ����оƬ���͡����á�������IRQ�ȵȡ�
    i2c_board_info���ڹ�����Ϣ�����г����ڵ�I2C�豸����һ��Ϣ������������I2C����������ģ������
    �������壬��ʹ��i2c_register_board_info()����̬�����������Ӱ壬������֪��������ʹ��i2c_new_device()��̬������

    i2c_register_board_info�Ĵ�ͳ�÷������ں˳�ʼ��ʱ����i2c_adapterע��֮ǰ�����ע�����Ѿ�˵���ˡ�
    �鿴i2c_adapter��ע�������Է��֣�i2c_adapterע���ʱ���ɨ���Ѿ�ע���board_info������
    Ϊÿһ��ע�����Ϣ����i2c_new_device��������i2c_client,������i2c_driverע���ʱ���豸����������ƥ�䲢����probe.

    i2c new style driver�����в�����Ҫ����i2c_client�ṹ�壬������i2c�ں˴����ġ�


    i2c_new_device�����������ʹ��info�ṩ����Ϣ����һ��i2c_client�����һ������ָ���i2c_adapter�󶨡�
    ���صĲ�����һ��i2c_clientָ�롣
    �����п���ֱ��ʹ��i2c_clientָ����豸ͨ���ˡ����������һ���Ƚϼ򵥵ķ�����

    */
    memset(&info, 0, sizeof(struct i2c_board_info));
    strlcpy(info.type, "i2c_api", I2C_NAME_SIZE);
    info.addr = I2C_API_FAKE_ADDR;
    client = i2c_new_device(adap, &info);//�ֶ�Ϊÿ��adapter������һ��client����
    if (client)
        add_i2c_api(client);//������adapter����i2c_api_list����
    lidbg(KERN_INFO "i2c_api_attach adap[%d]\n", adap->nr);
    return 0;
}

int i2c_api_detach(struct i2c_adapter *adap)
{
    struct i2c_api *i2c_api;

    i2c_api = get_i2c_api(adap->nr);
    if (i2c_api)
        del_i2c_api(i2c_api);
    return 0;
}


static int __init i2c_api_init(void)
{
    //����adapter
    int ret;
	LIDBG_MODULE_LOG;

#ifdef LIDBG_I2C_GPIO 
#ifndef CONFIG_I2C_GPIO
	lidbg("load lidbg_i2c_gpio.ko\n");
	lidbg_insmod( "/system/lib/modules/out/lidbg_i2c_gpio.ko");
	lidbg_insmod( "/flysystem/lib/out/lidbg_i2c_gpio.ko");
#endif	

	I2C_GPIO_CONFIG;
	ret = platform_device_register(&i2c_gpio_device);
	if (ret)
	{
		lidbg(KERN_ERR "[%s] Device registration failed!\n", __func__);
		return ret;
	}
#endif

    ret = i2c_add_driver(&i2c_api_driver); //��driverע�ᵽ��i2c_bus_type�������� ����i2c_client�����ƺ�id_table�е�������ƥ���
    if (ret)
    {
        lidbg(KERN_ERR "[%s] Driver registration failed, module not inserted.\n", __func__);
        return ret;
    }
    DUMP_BUILD_TIME;
#ifdef USE_I2C_LOCK
    mutex_init(&i2c_lock);
#endif

    return 0 ;
}

static void __exit i2c_api_exit(void)
{
    i2c_del_driver(&i2c_api_driver);
}

MODULE_AUTHOR("Loon, <sepnic@gmail.com>");
MODULE_DESCRIPTION("I2C i2c_api Driver");
MODULE_LICENSE("GPL");

module_init(i2c_api_init);
module_exit(i2c_api_exit);


EXPORT_SYMBOL(i2c_api_do_send);
EXPORT_SYMBOL(i2c_api_do_recv);
EXPORT_SYMBOL(i2c_api_do_recv_no_sub_addr);
EXPORT_SYMBOL(i2c_api_do_recv_sub_addr_2bytes);
EXPORT_SYMBOL(i2c_api_do_recv_sub_addr_3bytes);
EXPORT_SYMBOL(mod_i2c_main);
EXPORT_SYMBOL(lidbg_i2c_running);

EXPORT_SYMBOL(i2c_api_do_recv_SAF7741);
EXPORT_SYMBOL(i2c_api_do_send_TEF7000);
EXPORT_SYMBOL(i2c_api_do_recv_TEF7000);
EXPORT_SYMBOL(i2c_api_set_rate);//add by huangzongqiang

