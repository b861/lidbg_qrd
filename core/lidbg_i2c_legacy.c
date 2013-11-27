
/*i2c legacy driver*/


#include "lidbg.h"


#if 0
#define   DRV_NAME "RK I2C"
#define I2C_FAKE_ADDR 0x7f

/*
 * Debug
 */
#if 1
#define	DBG(x...)	lidbg(KERN_INFO x)
#else
#define	DBG(x...)
#endif

#define err(format, arg...) \
	lidbg(KERN_ERR  ": " format "\n" , ## arg)
#define info(format, arg...) \
	lidbg(KERN_INFO ": " format "\n" , ## arg)


static int legacy_i2c_attach(struct i2c_adapter *adap);
static int legacy_i2c_detach(struct i2c_client *client);




//unsigned short i2c_address;

struct i2c_client *i2c_client = NULL;//ÿһ��������豸��һ��i2c_client��ʾ

static unsigned short normal_i2c[] = { 0, I2C_CLIENT_END };

/*
7λ��ַ�ĸ�2λ�ɾ���Ĳ��߷������������Էֱ�ӵ�VCC����GND����
���ltc3445��������Ŀ�����֪������Ĳ��߷�������ô�����������оͿ���ֱ��ָ����
�������ָ����ַ��ⷶΧΪ��������ַ�����ڼ�����������ģ��ʱ��������е�ַ��⡣
*/

#if 0

static unsigned short forces[] = { I2C_FAKE_ADDR, I2C_CLIENT_END };
static const unsigned short ignore[]  = { I2C_CLIENT_END };


static struct i2c_client_address_data addr_data =
{
    /* Addresses to scan */
    .normal_i2c = normal_i2c,//���ָ��ָ������̽��ĵ�ַ���飬����ÿһ����������̽���������ĵ�ַ��
    .probe		= ignore,//List of adapter,address pairs to scan additionally���ָ��ָ����ڴ�ÿ����Ԫ��Ϊһ�飬ǰһ�������������ţ���һ�������ַ��ʹ�����ָ�����ʵ����ָ������������ָ����ַ����̽�⡣
    .ignore 	= ignore,//List of adapter,address pairs not to scan���ָ��ָ�򽫱����Եĵ�ַ���ڶ�i2c_normal�еĵ�ַ̽��ǰ�����Ȳ鿴�Ƿ������������飬ֻ������������в����ڵĵ�ַ����̽�⡣���ָ��ָ�������Ҳ��ÿ���������ź����һ����ַ��
    .forces 	= forces,//List of adapter,address pairs to boldly assume to be presentǿ��ʹ�õĵ�ַ������������̽���ʹ�á�
    //����̽��ĺ��������������źŲ����ܷ��յ���Ӧ��ȷ��ĳ����ַ������豸�Ѿ��������������ˡ����ָ��ָ��һ��ָ�����飬ÿһ����Ա��ָ������Ҳ��ÿ���������ź����һ����ַ��
};
#else
/* Magic definition of all other variables and things */
I2C_CLIENT_INSMOD;
/*
addr_data��ʵ��I2C_CLIENT_INSMOD�������չ�õ���

Ҫ��õ�һ�����õ�addr_data����I2C_CLIENT_INSMOD֮ǰ���붨��̽���ַ�����顣
Ӧ�ö���normal_addr������飬��������еĵ�ַ����ÿһ����������̽�⡣
ģ�����������������probe��ignore��forces��ע�����ֲ��ܸı䡣
���ĸ�����ֱ��Ӧi2c_client_address_data���ĸ���Ա�����Ƕ�������I2C_CLIENT_END��β��

I2C_CLIENT_MODULE_PARMֱ�ӻ��߼�ӵı�������������ʼ����force��probe��ignore�������飬
������������Ϊģ���������������Լ������������������������������������ֻ���ڲ���ģ���ʱ���ʼ����

*/
#endif


/* corgi i2c codec control layer */
//�������õ����ݽṹ ������Ӧ����ʵ��
static struct i2c_driver legacy_i2c_driver =
{
    .driver = {
        .name = DRV_NAME,
        .owner = THIS_MODULE,
    },
    //.id =             I2C_DRIVERID_WM8988, //?

    /*
    attach_adapter��detach_adapter��Legacy model��������Ҫ��ɵĽӿ�
    attach_adapter�ص������ڰ�װi2c�豸��������ģ��ʱ(i2c_add_driver)�������ڰ�װi2c��������������ģ��ʱ��(ѭ��)���ã�
    ���ڼ�⡢�����豸��Ϊ�豸����i2c_client���ݽṹ��

    detach_client������ж�����������豸��������ģ��ʱ�����ã�
    ���ڴ�������ע���豸�����ͷ�i2c_client����Ӧ��˽�����ݽṹ

    */
    .attach_adapter = legacy_i2c_attach,
    .detach_client  = legacy_i2c_detach,
    .command =        NULL,
};




static int i2c_detect_legacy(struct i2c_adapter *adap, int addr, int kind)
{
    int ret;
    struct i2c_client *client = NULL;

    DBG("Enter::%s----%d\n", __FUNCTION__, __LINE__);

    lidbg("addr 0x%x\n", addr);


    //if (addr != i2c_address)
    //	return -ENODEV;

    if (!i2c_check_functionality(adap, I2C_FUNC_I2C))  //�ж�����������
    {
        lidbg("i2c_check_functionality err \n");
        ret = -ENODEV;
        goto err;
    }

    i2c_client = kzalloc(sizeof(struct i2c_client), GFP_KERNEL);
    if (i2c_client == NULL)
    {
        ret = -ENOMEM;
        goto err;
    }

    /*


    */

    /****����i2c-client****/
    //ÿһ��������豸��һ��i2c_client��ʾ
    client = i2c_client;
    client->adapter = adap;//�豸������������
    client->driver = &legacy_i2c_driver;//�豸����������
    client->addr = addr;//��Ŵ�����豸��ַ���洢�����7����

#ifdef SOC_RK2818

    client->mode = NORMALMODE;
    client->Channel = I2C_CH1;
    client->addressBit = I2C_7BIT_ADDRESS_8BIT_REG;
    client->speed = 200;

#endif
    /*
    ������ɹ�������豸���������ṩ�Ļص��������������豸��i2c_client���ݽṹ��
    �������е�driverָ��ָ���豸���������i2c_driver���ݽṹ��
    */
    strlcpy(client->name, DRV_NAME, I2C_NAME_SIZE);

    //i2c_set_clientdata(client, codec);

    if (kind < 0)
    {
        //�Ѿ�IIC��ַ�����ڵ��豸 kind>=0 Ϊǿ��ʹ�õ��豸
    }

    ret = i2c_attach_client(client);//ע��i2c_client
    if (ret < 0)
    {
        err("failed to attach codec at addr %x\n", addr);
        goto exit_detach;
    }

    //ע���ַ��豸
    //,,,

    return 0;

exit_detach:

    lidbg("exit_detach \n");
    i2c_detach_client(client);

err:

    lidbg("err \n");
    kfree(client);
    return ret;
}

static int legacy_i2c_detach (struct i2c_client *client)
{
    i2c_detach_client(client);
    kfree(client);
    return 0;
}

static int legacy_i2c_attach(struct i2c_adapter *adap)
{

    DBG("Enter::%s----%d\n", __FUNCTION__, __LINE__);

    lidbg("addr_data.normal_i2c %x\n", (u32)addr_data.normal_i2c[0]);
    /*


    ����Ӧ�ñ�����ǰ���е�i2c����"����"���Ͽ��ܴ��ڵ��豸���������豸��ַ��⡣
    �������ͨ��һ��ѭ������adapters������ָ���е�i2c_adapter���ݽṹ�������������attach_adapter����ʵ�֣�˳�������Щ���������������ǵ�i2c�豸
    ����⵽�豸����ִ��rk_i2c_codec_probe(����������Ϊһ���ڵ�ַ�ɹ����ʱ�����õĻص�����),���ѵ�ʱ��̽���ַaddress��Ϊ��������
    ���ɹ�������豸���������ṩ�Ļص��������������豸��i2c_client���ݽṹ
    */
    return i2c_probe_legacy(adap/*�����������׵�ַ*/, &addr_data/*��ַ��Ϣ*/, i2c_detect_legacy/*̽�⵽�豸����õĺ���*/);

}



int i2c_probe_legacy(unsigned short i2c_addr)//��������ע���iic��ַ����ַ���͵�ʱ��Ҫ����һλ
//int rk_i2c_probe_legacy(void)
{
    int ret = 0;
    DBG("Enter::%s----%d\n", __FUNCTION__, __LINE__);

    if (i2c_addr)
    {

        normal_i2c[0] = i2c_addr;

        ret = i2c_add_driver(&legacy_i2c_driver);//����adapter ����i2c_driver��attach_adapter
        if (ret != 0)
            lidbg(KERN_ERR "can't add i2c driver");
    }
    return ret;
}

int i2c_remove_legacy(void)
{
    i2c_del_driver(&legacy_i2c_driver);
    return 0;
}



int i2c_write_legacy(unsigned int reg, unsigned int value)
//int rk_i2c_write_legacy(unsigned short i2c_addr, unsigned int reg, unsigned int value)
{
    u8 data[2];
    data[0] = reg ;
    data[1] = value & 0x00ff;
    lidbg("rk_i2c_write_legacy\n");
    if(i2c_client)
    {
        //i2c_client->addr = i2c_addr;
        if (i2c_master_send(i2c_client, data, 2) == 2)//���ط��͵ĳ���
            return 0;
        else
            return -EIO;
    }
    else
    {
        lidbg("i2c == NULL\n");

    }

    //��ʹ��i2c_transfer
    //ÿ����Ϣ�����Ƕ���Ҳ������д��Ҳ���Ի�ϡ����͹���������ģ��ڷ�����û��ֹͣ������
    //�������������Ϣ�Ļ������˵�һ��֮�⣬���µĶ�����Ҫ������ʼ������
    //ret = i2c_transfer(adap, &msg, 1);


    return 0;
}

#endif
