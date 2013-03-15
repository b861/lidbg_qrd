//-------------------------------------------------------------------------
// �ļ�����
// ���ܣ�
// ��ע��
//
//
//         By  ����ΰ
//             lisuwei@flyaudio.cn
//-------------------------------------------------------------------------

#ifndef __LIDBG_DEF__
#define __LIDBG_DEF__


typedef unsigned char			U8;
typedef unsigned short int		U16;
typedef unsigned int			U32;
typedef unsigned int			uchar;

//#define uchar unsigned char

//typedef unsigned int			*PU32;
//typedef unsigned char			BOOL;


//typedef char           INT8;
//typedef char 		   int8;
//typedef unsigned char  UINT8;
//typedef signed short   INT16;
//typedef unsigned short UINT16;
//typedef signed long    INT32;
//typedef unsigned long  UINT32;
//typedef unsigned long long      UINT64;

//typedef signed long int32;



#define READ_REGISTER_ULONG(reg)        (*(volatile ULONG * const)(reg))
#define WRITE_REGISTER_ULONG(reg,val)   (*(volatile ULONG * const)(reg) = (ULONG)(val))

#define READ_REGISTER_USHORT(reg)       (*(volatile USHORT * const)(reg))
#define WRITE_REGISTER_USHORT(reg,val)  (*(volatile USHORT * const)(reg) = (USHORT)(val))

#define READ_REGISTER_UCHAR(reg)       (*(volatile UCHAR * const)(reg))
#define WRITE_REGISTER_UCHAR(reg,val)  (*(volatile UCHAR * const)(reg) = (UCHAR)(val))




// For Debug
#define MSG_ERROR        (1)
#define MSG_DEBUG        (1)

#define lidbg(msg...)  do { printk( KERN_CRIT "lidbg: " msg); }while(0)
#define lidbgerr(msg...)  do { printk( KERN_CRIT "\nlidbgerr: " msg); }while(0)


#define FUNCTION_IN       do{lidbg("%d: %s() In", __LINE__, __FUNCTION__);}while(0)
#define FUNCTION_OUT    do{lidbg("%d: %s() Out", __LINE__, __FUNCTION__);}while(0)
#define DUMP_BUILD_TIME    do{ lidbg( "Build Time: %s %s  %s \n", __FUNCTION__, __DATE__, __TIME__);}while(0)
#define DUMP_FUN     do{lidbg( "%s+\n", __FUNCTION__);}while(0)
#define DUMP_FUN_ENTER     DUMP_FUN
#define DUMP_FUN_LEAVE     do{lidbg( "%s-\n", __FUNCTION__);}while(0)
#define WHILE_ENTER     do{lidbg( "%s:while blocking...\n", __FUNCTION__);}while(0)
#define DUMP_POS     do{lidbg( "come to %s,line %d\n", __FILE__,__LINE__);}while(0)


#define HIGH			(1)
#define LOW				(0)

#define MEM_SIZE_1_KB          	     (0x00000400)
#define MEM_SIZE_2_KB          	     (0x00000800)
#define MEM_SIZE_4_KB          	     (0x00001000)
#define MEM_SIZE_8_KB         	     (0x00002000)
#define MEM_SIZE_16_KB         	     (0x00004000)
#define MEM_SIZE_32_KB         	     (0x00008000)

#define MEM_SIZE_64_KB               (0x00010000)
#define MEM_SIZE_128_KB              (0x00020000)
#define MEM_SIZE_256_KB              (0x00040000)
#define MEM_SIZE_512_KB              (0x00080000)

#define MEM_SIZE_1_MB                (0x00100000)
#define MEM_SIZE_2_MB                (0x00200000)
#define MEM_SIZE_4_MB                (0x00400000)
#define MEM_SIZE_8_MB                (0x00800000)

#define MEM_SIZE_16_MB               (0x01000000)
#define MEM_SIZE_32_MB               (0x02000000)
#define MEM_SIZE_64_MB               (0x04000000)
#define MEM_SIZE_128_MB              (0x08000000)

#define MEM_SIZE_256_MB              (0x10000000)
#define MEM_SIZE_512_MB              (0x20000000)
#define MEM_SIZE_1024_MB             (0x40000000)
#define MEM_SIZE_2048_MB             (0x80000000)





/**************************************/
//��������Ԫ�صĸ���,ֻ��ֱ����������Ч������ָ����Ч
#define SIZE_OF_ARRAY(array_name) (sizeof(array_name)/sizeof(array_name[0]))

//��������int��ֵ
//#define SWAP(a,b) do {int tmp;tmp = (a);(a) = (b);(b) = tmp;}while(0)

//����������ֵ
#define SWAP(x, y) { \
						x = x ^ y;		 \
						y = x ^ y;		 \
						x = x ^ y;		 \
					}

//�����ֵ����Сֵ
#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )

//�����ֵ
#define ABS(a)      ((a)>=0?(a):(-(a)))

//��һ����ĸת��Ϊ��д
#define  UPCASE( c ) ( (c >= 'a' && c <= 'z') ? (c - 0x20) : c)


//�ж��ַ��ǲ���10��ֵ������
#define  DECCHK( c ) (c >= '0' && c <= '9')

//�ж��ַ��ǲ���16��ֵ������
#define  HEXCHK( c ) ( (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') ||(c >= 'a' && c <= 'f') )


//����LSB��ʽ�������ֽ�ת��Ϊһ������
#define  FLIPW( ray ) ( (((U16) (ray)[0]) * 256) + (ray)[1] )

//����LSB��ʽ��һ������ת��Ϊ�����ֽ�
#define  FLOPW( ray, val ) do {(ray)[0] = ((val) / 256); (ray)[1] = ((val) & 0xFF) }while(0)


// �����ʹ�С�˻���
#define BIG_LITTLE_SWAP32(A)        ((((U32)(A) & 0xff000000) >> 24) | \
                                   (((U32)(A) & 0x00ff0000) >> 8) | \
                                   (((U32)(A) & 0x0000ff00) << 8) | \
                                   (((U32)(A) & 0x000000ff) << 24))

/*
// ������˷���1��С�˷���0
int checkCPUendian()
{
        union{
                unsigned long int i;
                unsigned char s[4];
        }c = {0x12345678};

        return (0x12 == c.s[0]);
}


*/
//�õ�ָ����ַ�ϵ�һ���ֽڻ���
#define  MEM_B( x )  ( *( (UINT8 *) (x) ) )
#define  MEM_W( x )  ( *( (UINT32 *) (x) ) )


//ͨ������(ָ��)����	arr�׵�ַ;num����Ԫ�ظ���;����sizeÿ��Ԫ�ش�С
//���ǵ���ָ�벻��ʹ��sizeof�����,�ʴ���Ƚ϶����,����ֱ�Ӵ�������

#define RESERVE_ARR(arr,num,size) {                   			     \
						int j, i = 0;							 \
							j = num - 1;						 \
						while(i < j) {							 \
							SWAP(arr[i], arr[j]);				 \
							i++;								 \
							j--;								 \
						}										 \
					}


#endif
