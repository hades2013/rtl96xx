
#ifndef APP_HEAD_H
#define APP_HEAD_H


#define ADDI_HEADLEN 64

struct addi_hdr_t
{
	unsigned char str_mark[ADDI_HEADLEN-4];
	unsigned int imgsize;
} __attribute__ ((aligned(1), packed));



#define PRODUCT_VER_STR_LEN_MAX 36
#define FMW_HEADLEN 0x60

struct fmw_hdr_t
{
	unsigned int headlen;   // �ļ�ͷ�ĳ���
	unsigned int imgsize;   // �����ļ���С������ͷ
	unsigned int checksum;  // У��ֵ������ͷ���㷨��4�ֽ���ӣ����Ϊ0��
	unsigned int timestamp; // �����ļ�������ʱ��
	unsigned int kernellen; // �ں˵ĳ���
	unsigned int fslen;     // �ļ�ϵͳ�ĳ���
			        // ����ļ�����4�ֽڶ��룬��������룬����0��䡣
	
	unsigned char _model; // ��Ʒ���� ER5200
	unsigned char _major; // ��ƷV�汾�� V100
	unsigned char _minor; // ��ƷR�汾�� R001
	unsigned char _patch; // ��ƷD�汾�� D001
	unsigned char _limit; // ��Ʒ���޷����İ汾�� L00
	
	
	unsigned char kernel_type;   // ������ͣ�1���ںˣ� 0xff:��
	unsigned char _kernel_major; // �ں�V�汾�� V100��  �������ϢʱΪ0xff
	unsigned char _kernel_minor; // �ں�R�汾�� R001��  �������ϢʱΪ0xff
	unsigned char _kernel_patch; // �ں�D�汾�� D001��  �������ϢʱΪ0xff
	unsigned char _kernel_limit; // �ں����޷����İ汾�� L00��  �������ϢʱΪ0xff
	
	unsigned char fs_type;   // ������ͣ�1���ļ�ϵͳ�� 0xff:��
	unsigned char product_version_str[PRODUCT_VER_STR_LEN_MAX]; 
	
	unsigned char addition[FMW_HEADLEN-0x47]; // �����������0���̶�����
	
} __attribute__ ((aligned(1), packed));


#define KERNEL_TYPE (0x01)
#define FS_TYPE     (0x02)
#define NULL_TYPE   (0xff)

#endif

