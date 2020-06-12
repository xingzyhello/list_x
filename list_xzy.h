#ifndef _LIST_XZY_H_
#define _LIST_XZY_H_

typedef enum{
	enum_list_xzy_err_idle = 0,//正确执行，但是没得到正确的结果
	enum_list_xzy_err_ok,//正确执行，并且得到正确的结果
	enum_list_xzy_err_param_NULL,
	enum_list_xzy_err_param_wrong,
	enum_list_xzy_err_param_small,//空间不够大
	enum_list_xzy_err_read_busy,//
	enum_list_xzy_err_read_off,
	enum_list_xzy_err_write_busy,//
	enum_list_xzy_err_write_off,//
	enum_list_xzy_err_recover,//链表允许覆盖时表示读取过程中被覆盖导致的读取失败；链表不允许覆盖时表示写过程中要覆盖导致的写失败
	
}ENUM_LIST_XZY_ERR;

typedef enum{
	enum_rw_step_idle=0,
	enum_rw_step_work,
}ENUM_LIST_XZY_RW_STEP;

typedef struct{
	unsigned int index;//可以操作的位置下标
	ENUM_LIST_XZY_RW_STEP flag_on_off;//是否允许操作
	ENUM_LIST_XZY_RW_STEP step;//空闲还是正在操作
	unsigned int cir_times;//每操作cache_size_value个字节加一
}STRUCT_LIST_XZY_INFO;

typedef struct{
	unsigned char *cache_address;//初始化就确定，用于缓存数据
	unsigned int cache_size;//初始化就确定，缓存数据空间的大小
	unsigned int cache_size_value;//初始化就确定，缓存数据空间的有效字节数
	unsigned int unit_size;//初始化就确定，每次读和写的字节数
	unsigned int count_w_cover;//写过程中产生的覆盖计数或者写入失败计数
	ENUM_LIST_XZY_RW_STEP flag_cover;//初始化就确定，是否允许覆盖
	ENUM_LIST_XZY_RW_STEP flag_on_off;//链表开关
	STRUCT_LIST_XZY_INFO r;//flag_on_off由start与end函数设置，其余由read函数设置
	STRUCT_LIST_XZY_INFO w;//flag_on_off由start与end函数设置，其余由write函数设置
}STRUCT_LIST_XZY;

ENUM_LIST_XZY_ERR list_xzy_init(volatile STRUCT_LIST_XZY *p_info,unsigned char *buff,unsigned int size_buff,unsigned int unit_size,ENUM_LIST_XZY_RW_STEP flag_cover);
ENUM_LIST_XZY_ERR list_xzy_start(volatile STRUCT_LIST_XZY *p_info);
ENUM_LIST_XZY_ERR list_xzy_end(volatile STRUCT_LIST_XZY *p_info);
ENUM_LIST_XZY_ERR list_xzy_read(volatile STRUCT_LIST_XZY *p_info,unsigned char *p_data,unsigned int size_data,ENUM_LIST_XZY_RW_STEP flag_updata);
ENUM_LIST_XZY_ERR list_xzy_get_num(volatile STRUCT_LIST_XZY *p_info,unsigned int *item_num);
ENUM_LIST_XZY_ERR list_xzy_write(volatile STRUCT_LIST_XZY *p_info,unsigned char *p_data,unsigned int write_bytes);

#endif
