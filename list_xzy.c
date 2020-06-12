#include "list_xzy.h"
#include "string.h"

#if 1
/*
	名称：链表初始化
	参数：
		p_info：用于存储链表的相关信息
		buff：用于存储链表的数据
		size_buff：buff的大小（字节）
		unit_size：链表进行读或者写操作进行一次的字节数
			最小值为1且小于等于size_buff
		flag_cover：是否允许覆盖
			不允许：enum_rw_step_idle
			允许：enum_rw_step_work
	返回值：
		enum_list_xzy_err_ok：成功
		enum_list_xzy_err_param_NULL：
		enum_list_xzy_err_param_wrong：
		enum_list_xzy_err_param_small：
	备注：
*/
ENUM_LIST_XZY_ERR list_xzy_init(volatile STRUCT_LIST_XZY *p_info,unsigned char *buff,unsigned int size_buff,unsigned int unit_size,ENUM_LIST_XZY_RW_STEP flag_cover)
{
	if(p_info==NULL || buff==NULL)return enum_list_xzy_err_param_NULL;
	if(unit_size < 1)return enum_list_xzy_err_param_wrong;
	if(size_buff < unit_size)return enum_list_xzy_err_param_small;

	//关闭链表
	p_info->flag_on_off = enum_rw_step_idle;
	//关闭读和写
	p_info->r.flag_on_off = enum_rw_step_idle;
	p_info->w.flag_on_off = enum_rw_step_idle;	
	//链表初始化
	p_info->cache_address = buff;
	p_info->cache_size = size_buff;
	p_info->cache_size_value = size_buff / unit_size * unit_size;
	p_info->unit_size = unit_size;
	p_info->flag_cover = flag_cover;
	//打开链表
	p_info->flag_on_off = enum_rw_step_work;
	
	return enum_list_xzy_err_ok;
}

/*
	名称：链表开始
	参数：
		p_info：用于存储链表的相关信息
	返回值：
		enum_list_xzy_err_ok：成功
		enum_list_xzy_err_param_NULL
		enum_list_xzy_err_param_wrong
	备注：
		调用函数之前需要调用list_xzy_init，否则调用失败
*/
ENUM_LIST_XZY_ERR list_xzy_start(volatile STRUCT_LIST_XZY *p_info)
{
	if(p_info==NULL)return enum_list_xzy_err_param_NULL;
	if(p_info->flag_on_off != enum_rw_step_work)return enum_list_xzy_err_param_wrong;
	//关闭链表
	p_info->flag_on_off = enum_rw_step_idle;
	//关闭读和写
	p_info->r.flag_on_off = enum_rw_step_idle;
	p_info->w.flag_on_off = enum_rw_step_idle;
	//链表相关清零
	p_info->count_w_cover = 0;
	//读部分初始化
	p_info->r.index = 0;
	p_info->r.step = enum_rw_step_idle;
	p_info->r.cir_times = 0;
	//写部分初始化
	p_info->w.index = 0;
	p_info->w.step = enum_rw_step_idle;
	p_info->w.cir_times = 0;
	//开启读和写
	p_info->r.flag_on_off = enum_rw_step_work;
	p_info->w.flag_on_off = enum_rw_step_work;
	//开启链表
	p_info->flag_on_off = enum_rw_step_work;
	
	return enum_list_xzy_err_ok;
}

/*
	名称：链表结束
	参数：
		p_info：用于存储链表的相关信息
	返回值：
		enum_list_xzy_err_ok：成功
		enum_list_xzy_err_param_NULL
		enum_list_xzy_err_param_wrong
	备注：
*/
ENUM_LIST_XZY_ERR list_xzy_end(volatile STRUCT_LIST_XZY *p_info)
{
	if(p_info==NULL)return enum_list_xzy_err_param_NULL;
	if(p_info->flag_on_off != enum_rw_step_work)return enum_list_xzy_err_param_wrong;
	
	//关闭链表
	p_info->flag_on_off = enum_rw_step_idle;
	//关闭读和写
	p_info->r.flag_on_off = enum_rw_step_idle;
	p_info->w.flag_on_off = enum_rw_step_idle;
	
	return enum_list_xzy_err_ok;
}

/*
	名称：链表读
	参数：
		p_info：用于存储链表的相关信息
		p_data：用于存储从链表读出来的数据
		size_data：p_data的大小（字节）
			最小值为list_xzy_init中的参数unit_size
		flag_updata：是否在读取数据后进行链表信息的更新
			不更新：enum_rw_step_idle，即只读，读取数据成功后该数据仍然存在链表中
			更新：enum_rw_step_work
	返回值：
		enum_list_xzy_err_ok：成功
		enum_list_xzy_err_idle：链表没有可读的数据
		enum_list_xzy_err_param_NULL
		enum_list_xzy_err_param_wrong
		enum_list_xzy_err_param_small
		enum_list_xzy_err_read_off
		enum_list_xzy_err_read_busy
		enum_list_xzy_err_recover		
	备注：
*/
ENUM_LIST_XZY_ERR list_xzy_read(volatile STRUCT_LIST_XZY *p_info,unsigned char *p_data,unsigned int size_data,ENUM_LIST_XZY_RW_STEP flag_updata)
{
	STRUCT_LIST_XZY tmp_info = {0};
	STRUCT_LIST_XZY tmp_info_1 = {0};
	
	if(p_info==NULL)return enum_list_xzy_err_param_NULL;
	if(p_info->flag_on_off != enum_rw_step_work)return enum_list_xzy_err_param_wrong;
	if(p_data == NULL)return enum_list_xzy_err_param_NULL;
	if(size_data < p_info->unit_size)return enum_list_xzy_err_param_small;
		
	if(p_info->r.flag_on_off != enum_rw_step_work)return enum_list_xzy_err_read_off;
	if(p_info->r.step == enum_rw_step_work)return enum_list_xzy_err_read_busy;
	
	tmp_info = *p_info;//主要用于判断读出的数据是否被覆盖
	//将读操作置于工作状态
	p_info->r.step = enum_rw_step_work;
	//链表允许覆盖时，检查读的指针是否需要调整
	if(p_info->flag_cover == enum_rw_step_work)
	{
		if(p_info->r.cir_times == p_info->w.cir_times)
			//读和写处于同一个cir_times，尚未进行过覆盖操作
			;
		else if((p_info->r.cir_times+1) == p_info->w.cir_times && \
				p_info->r.index >= p_info->w.index
			)
			//写处于读的下一个cir_times，但写index小于等于读index，尚未进行过覆盖
			;
		else
		{
			p_info->r.cir_times = p_info->w.cir_times - 1;
			p_info->r.index = p_info->w.index;
		}
	}
	//查看链表中是否存在允许读取的数据
	if(p_info->r.cir_times == p_info->w.cir_times && \
		p_info->r.index == p_info->w.index
		)
		return enum_list_xzy_err_idle;
	//读取数据
	memcpy(p_data,p_info->cache_address + p_info->r.index,p_info->unit_size);	
	tmp_info_1 = *p_info;//主要用于判断读出的数据是否被覆盖
	//链表允许覆盖时，查看数据读取过程中是否被写操作修改过
	if(p_info->flag_cover == enum_rw_step_work)
	{
		if(tmp_info.w.cir_times == tmp_info_1.w.cir_times && \
			tmp_info.w.index == tmp_info_1.w.index \
			)
			//读的过程中未进行写操作
			;
		if(p_info->r.index == tmp_info_1.w.index && \
			tmp_info_1.w.step == enum_rw_step_work)
		{//读完后写正处于写操作做且写index与读index相等
			return enum_list_xzy_err_recover;
		}
		else if(p_info->r.cir_times == tmp_info.w.cir_times && \
				p_info->r.index < tmp_info.w.index && \
				p_info->r.cir_times == tmp_info_1.w.cir_times && \
				p_info->r.index < tmp_info_1.w.index
				)
			//读和写处于同一个cir_times
			;//
		else if(p_info->r.cir_times == tmp_info.w.cir_times && \
				p_info->r.index < tmp_info.w.index && \
				(p_info->r.cir_times+1) == tmp_info_1.w.cir_times && \
				p_info->r.index >= tmp_info_1.w.index
				)
			//读之前和写处于同一个cir_times，读之后，写处于一个cir_times，但是读之后的读index大于写index
			;//
		else if((p_info->r.cir_times+1) == tmp_info.w.cir_times && \
				p_info->r.index >= tmp_info.w.index && \
				(p_info->r.cir_times+1) == tmp_info_1.w.cir_times && \
				p_info->r.index >= tmp_info_1.w.index
				)
			//读之前，写处于下一个cir_times，但是读之前的读index大于写index
			//读之后，写处于下一个cir_times，但是读之后的读index大于写index
			;//
		else
		{//读的过程中，读的数据可能被写修改过
			return enum_list_xzy_err_recover;
		}
	}
	//更新计数
	if(flag_updata == enum_rw_step_work)
	{
		p_info->r.index += p_info->unit_size;
		p_info->r.cir_times += p_info->r.index / p_info->cache_size_value;
		p_info->r.index %= p_info->cache_size_value;
	}
	
	return enum_list_xzy_err_ok;
}

/*
	名称：链表缓存数据的可读次数
	参数：
		p_info：用于存储链表的相关信息
		item_num：可读次数的存储地址
	返回值：
		enum_list_xzy_err_ok：成功
		enum_list_xzy_err_param_NULL
		enum_list_xzy_err_param_wrong
	备注：
		该函数读取的次数存在不准的情况
*/
ENUM_LIST_XZY_ERR list_xzy_get_num(volatile STRUCT_LIST_XZY *p_info,unsigned int *item_num)
{
	STRUCT_LIST_XZY tmp_info = {0};
	
	if(p_info==NULL)return enum_list_xzy_err_param_NULL;
	if(p_info->flag_on_off != enum_rw_step_work)return enum_list_xzy_err_param_wrong;
	if(p_info->r.flag_on_off != enum_rw_step_work)return enum_list_xzy_err_param_wrong;
	if(p_info->w.flag_on_off != enum_rw_step_work)return enum_list_xzy_err_param_wrong;
	if(item_num==NULL)return enum_list_xzy_err_param_NULL;
	
	tmp_info = *p_info;
	if(tmp_info.r.cir_times == tmp_info.w.cir_times)
	{
		if(tmp_info.r.index == tmp_info.w.index)
			*item_num = 0;
		else
			*item_num = (tmp_info.w.index - tmp_info.r.index) / tmp_info.unit_size;
	}
	else if((tmp_info.r.cir_times+1) == tmp_info.w.cir_times)
	{
		if(tmp_info.r.index >= p_info->w.index)
			*item_num = (tmp_info.cache_size_value - (tmp_info.r.index - p_info->w.index)) / tmp_info.unit_size;
		else
			*item_num = tmp_info.cache_size_value / tmp_info.unit_size;
	}
	else
		*item_num = tmp_info.cache_size_value / tmp_info.unit_size;
		
	return enum_list_xzy_err_ok;
}

/*
	名称：链表写
	参数：
		p_info：用于存储链表的相关信息
		p_data：写入链表数据的首地址
		write_bytes：写入链表的字节数
			最大值为list_xzy_init中的参数unit_size
			最小值为零
	返回值：
		enum_list_xzy_err_ok：成功
		enum_list_xzy_err_param_NULL
		enum_list_xzy_err_param_wrong
		enum_list_xzy_err_write_busy
		enum_list_xzy_err_recover
	备注：
*/
ENUM_LIST_XZY_ERR list_xzy_write(volatile STRUCT_LIST_XZY *p_info,unsigned char *p_data,unsigned int write_bytes)
{
	if(p_info==NULL)return enum_list_xzy_err_param_NULL;
	if(write_bytes > p_info->unit_size)return enum_list_xzy_err_param_wrong;
	if(p_info->flag_on_off != enum_rw_step_work)return enum_list_xzy_err_param_wrong;
	if(p_info->w.flag_on_off != enum_rw_step_work)return enum_list_xzy_err_param_wrong;
	if(p_info->w.step != enum_rw_step_work)return enum_list_xzy_err_write_busy;
	
	//将写操作置于工作状态
	p_info->w.step = enum_rw_step_work;
	//链表不允许覆盖时，写入是否会覆盖
	if(p_info->flag_cover == enum_rw_step_idle)
	{
		if((p_info->r.cir_times + 1) == p_info->w.cir_times && \
			p_info->w.index == p_info->r.index \
			)
			return enum_list_xzy_err_recover;
	}
	//先清零，后写入，清零是为了防止写入字节数不足时剩余空间残留之前的数据
	memset(p_info->cache_address + p_info->w.index,0,p_info->unit_size);
	memcpy(p_info->cache_address + p_info->w.index,p_data,write_bytes);
	//更新计数
	p_info->w.index += p_info->unit_size;
	p_info->w.cir_times += p_info->w.index / p_info->cache_size_value;
	p_info->w.index %= p_info->cache_size_value;
	//将写操作置于空闲状态
	p_info->w.step = enum_rw_step_idle;
	
	return enum_list_xzy_err_ok;
}
#endif
