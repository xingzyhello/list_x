#ifndef _LIST_XZY_H_
#define _LIST_XZY_H_

typedef enum{
	enum_list_xzy_err_idle = 0,//��ȷִ�У�����û�õ���ȷ�Ľ��
	enum_list_xzy_err_ok,//��ȷִ�У����ҵõ���ȷ�Ľ��
	enum_list_xzy_err_param_NULL,
	enum_list_xzy_err_param_wrong,
	enum_list_xzy_err_param_small,//�ռ䲻����
	enum_list_xzy_err_read_busy,//
	enum_list_xzy_err_read_off,
	enum_list_xzy_err_write_busy,//
	enum_list_xzy_err_write_off,//
	enum_list_xzy_err_recover,//����������ʱ��ʾ��ȡ�����б����ǵ��µĶ�ȡʧ�ܣ�����������ʱ��ʾд������Ҫ���ǵ��µ�дʧ��
	
}ENUM_LIST_XZY_ERR;

typedef enum{
	enum_rw_step_idle=0,
	enum_rw_step_work,
}ENUM_LIST_XZY_RW_STEP;

typedef struct{
	unsigned int index;//���Բ�����λ���±�
	ENUM_LIST_XZY_RW_STEP flag_on_off;//�Ƿ��������
	ENUM_LIST_XZY_RW_STEP step;//���л������ڲ���
	unsigned int cir_times;//ÿ����cache_size_value���ֽڼ�һ
}STRUCT_LIST_XZY_INFO;

typedef struct{
	unsigned char *cache_address;//��ʼ����ȷ�������ڻ�������
	unsigned int cache_size;//��ʼ����ȷ�����������ݿռ�Ĵ�С
	unsigned int cache_size_value;//��ʼ����ȷ�����������ݿռ����Ч�ֽ���
	unsigned int unit_size;//��ʼ����ȷ����ÿ�ζ���д���ֽ���
	unsigned int count_w_cover;//д�����в����ĸ��Ǽ�������д��ʧ�ܼ���
	ENUM_LIST_XZY_RW_STEP flag_cover;//��ʼ����ȷ�����Ƿ�������
	ENUM_LIST_XZY_RW_STEP flag_on_off;//������
	STRUCT_LIST_XZY_INFO r;//flag_on_off��start��end�������ã�������read��������
	STRUCT_LIST_XZY_INFO w;//flag_on_off��start��end�������ã�������write��������
}STRUCT_LIST_XZY;

ENUM_LIST_XZY_ERR list_xzy_init(volatile STRUCT_LIST_XZY *p_info,unsigned char *buff,unsigned int size_buff,unsigned int unit_size,ENUM_LIST_XZY_RW_STEP flag_cover);
ENUM_LIST_XZY_ERR list_xzy_start(volatile STRUCT_LIST_XZY *p_info);
ENUM_LIST_XZY_ERR list_xzy_end(volatile STRUCT_LIST_XZY *p_info);
ENUM_LIST_XZY_ERR list_xzy_read(volatile STRUCT_LIST_XZY *p_info,unsigned char *p_data,unsigned int size_data,ENUM_LIST_XZY_RW_STEP flag_updata);
ENUM_LIST_XZY_ERR list_xzy_get_num(volatile STRUCT_LIST_XZY *p_info,unsigned int *item_num);
ENUM_LIST_XZY_ERR list_xzy_write(volatile STRUCT_LIST_XZY *p_info,unsigned char *p_data,unsigned int write_bytes);

#endif
