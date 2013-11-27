
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

struct i2c_client *i2c_client = NULL;//每一个具体的设备用一个i2c_client表示

static unsigned short normal_i2c[] = { 0, I2C_CLIENT_END };

/*
7位地址的高2位由具体的布线方法决定（可以分别接到VCC或者GND）。
如果ltc3445驱动程序的开发者知道具体的布线方法，那么在驱动程序中就可以直接指定。
否则可以指定地址检测范围为这两个地址，而在加载驱动程序模块时由软件进行地址检测。
*/

#if 0

static unsigned short forces[] = { I2C_FAKE_ADDR, I2C_CLIENT_END };
static const unsigned short ignore[]  = { I2C_CLIENT_END };


static struct i2c_client_address_data addr_data =
{
    /* Addresses to scan */
    .normal_i2c = normal_i2c,//这个指针指向正常探测的地址数组，将在每一个适配器上探测这个数组的地址。
    .probe		= ignore,//List of adapter,address pairs to scan additionally这个指针指向的内存每两个元素为一组，前一个代表适配器号，后一个代表地址。使用这个指针可以实现在指定的适配器的指定地址进行探测。
    .ignore 	= ignore,//List of adapter,address pairs not to scan这个指针指向将被忽略的地址，在对i2c_normal中的地址探测前将首先查看是否存在于这个数组，只有在这个数组中不存在的地址才能探测。这个指针指向的内容也是每个适配器号后紧跟一个地址。
    .forces 	= forces,//List of adapter,address pairs to boldly assume to be present强制使用的地址，不进行物理探测就使用。
    //物理探测的含义是主机发送信号测试能否收到回应，确定某个地址代表的设备已经连接在总线上了。这个指针指向一个指针数组，每一个成员所指的内容也是每个适配器号后紧跟一个地址。
};
#else
/* Magic definition of all other variables and things */
I2C_CLIENT_INSMOD;
/*
addr_data其实是I2C_CLIENT_INSMOD这个宏扩展得到的

要想得到一个可用的addr_data，在I2C_CLIENT_INSMOD之前必须定义探测地址的数组。
应该定义normal_addr这个数组，这个数组中的地址会在每一个适配器上探测。
模块参数数组有三个：probe、ignore和forces。注意名字不能改变。
这四个数组分别对应i2c_client_address_data的四个成员。它们都必须以I2C_CLIENT_END结尾。

I2C_CLIENT_MODULE_PARM直接或者间接的被调用声明并初始化了force、probe、ignore三个数组，
还将它们声明为模块参数。所以如果自己定义这三个数组会引起编译错误。这三个数组只能在插入模块的时候初始化。

*/
#endif


/* corgi i2c codec control layer */
//辅助作用的数据结构 并不对应物理实体
static struct i2c_driver legacy_i2c_driver =
{
    .driver = {
        .name = DRV_NAME,
        .owner = THIS_MODULE,
    },
    //.id =             I2C_DRIVERID_WM8988, //?

    /*
    attach_adapter和detach_adapter是Legacy model的驱动需要完成的接口
    attach_adapter回调函数在安装i2c设备驱动程序模块时(i2c_add_driver)、或者在安装i2c适配器驱动程序模块时被(循环)调用，
    用于检测、认领设备并为设备分配i2c_client数据结构。

    detach_client方法在卸载适配器或设备驱动程序模块时被调用，
    用于从总线上注销设备、并释放i2c_client及相应的私有数据结构

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

    if (!i2c_check_functionality(adap, I2C_FUNC_I2C))  //判定适配器能力
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

    /****构建i2c-client****/
    //每一个具体的设备用一个i2c_client表示
    client = i2c_client;
    client->adapter = adap;//设备依附的适配器
    client->driver = &legacy_i2c_driver;//设备依附的驱动
    client->addr = addr;//存放代表的设备地址，存储在最低7比特

#ifdef SOC_RK2818

    client->mode = NORMALMODE;
    client->Channel = I2C_CH1;
    client->addressBit = I2C_7BIT_ADDRESS_8BIT_REG;
    client->speed = 200;

#endif
    /*
    如果检测成功则调用设备驱动程序提供的回调函数创建描述设备的i2c_client数据结构，
    并将其中的driver指针指向设备驱动程序的i2c_driver数据结构。
    */
    strlcpy(client->name, DRV_NAME, I2C_NAME_SIZE);

    //i2c_set_clientdata(client, codec);

    if (kind < 0)
    {
        //已经IIC地址检测存在的设备 kind>=0 为强制使用的设备
    }

    ret = i2c_attach_client(client);//注册i2c_client
    if (ret < 0)
    {
        err("failed to attach codec at addr %x\n", addr);
        goto exit_detach;
    }

    //注册字符设备
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


    所以应该遍历当前所有的i2c总线"认领"其上可能存在的设备，即进行设备地址检测。
    这个任务通过一个循环，对adapters数组所指所有的i2c_adapter数据结构调用驱动程序的attach_adapter方法实现，顺序调用这些适配器来连接我们的i2c设备
    若检测到设备，则执行rk_i2c_codec_probe(第三个参数为一个在地址成功检测时被调用的回调函数),并把当时的探测地址address作为参数传入
    检测成功则调用设备驱动程序提供的回调函数创建描述设备的i2c_client数据结构
    */
    return i2c_probe_legacy(adap/*适配器数组首地址*/, &addr_data/*地址信息*/, i2c_detect_legacy/*探测到设备后调用的函数*/);

}



int i2c_probe_legacy(unsigned short i2c_addr)//不能是已注册的iic地址，地址发送的时候还要左移一位
//int rk_i2c_probe_legacy(void)
{
    int ret = 0;
    DBG("Enter::%s----%d\n", __FUNCTION__, __LINE__);

    if (i2c_addr)
    {

        normal_i2c[0] = i2c_addr;

        ret = i2c_add_driver(&legacy_i2c_driver);//遍历adapter 调用i2c_driver的attach_adapter
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
        if (i2c_master_send(i2c_client, data, 2) == 2)//返回发送的长度
            return 0;
        else
            return -EIO;
    }
    else
    {
        lidbg("i2c == NULL\n");

    }

    //或使用i2c_transfer
    //每个消息可以是读，也可以是写，也可以混合。发送过程是连贯的，在发送中没有停止条件。
    //如果连续多条消息的话，除了第一条之外，余下的都不需要发送起始条件。
    //ret = i2c_transfer(adap, &msg, 1);


    return 0;
}

#endif
