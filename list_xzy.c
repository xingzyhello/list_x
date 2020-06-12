#include "list_xzy.h"
#include "string.h"

#if 1
/*
	���ƣ������ʼ��
	������
		p_info�����ڴ洢����������Ϣ
		buff�����ڴ洢���������
		size_buff��buff�Ĵ�С���ֽڣ�
		unit_size��������ж�����д��������һ�ε��ֽ���
			��СֵΪ1��С�ڵ���size_buff
		flag_cover���Ƿ�������
			������enum_rw_step_idle
			����enum_rw_step_work
	����ֵ��
		enum_list_xzy_err_ok���ɹ�
		enum_list_xzy_err_param_NULL��
		enum_list_xzy_err_param_wrong��
		enum_list_xzy_err_param_small��
	��ע��
*/
ENUM_LIST_XZY_ERR list_xzy_init(volatile STRUCT_LIST_XZY *p_info,unsigned char *buff,unsigned int size_buff,unsigned int unit_size,ENUM_LIST_XZY_RW_STEP flag_cover)
{
	if(p_info==NULL || buff==NULL)return enum_list_xzy_err_param_NULL;
	if(unit_size < 1)return enum_list_xzy_err_param_wrong;
	if(size_buff < unit_size)return enum_list_xzy_err_param_small;

	//�ر�����
	p_info->flag_on_off = enum_rw_step_idle;
	//�رն���д
	p_info->r.flag_on_off = enum_rw_step_idle;
	p_info->w.flag_on_off = enum_rw_step_idle;	
	//�����ʼ��
	p_info->cache_address = buff;
	p_info->cache_size = size_buff;
	p_info->cache_size_value = size_buff / unit_size * unit_size;
	p_info->unit_size = unit_size;
	p_info->flag_cover = flag_cover;
	//������
	p_info->flag_on_off = enum_rw_step_work;
	
	return enum_list_xzy_err_ok;
}

/*
	���ƣ�����ʼ
	������
		p_info�����ڴ洢����������Ϣ
	����ֵ��
		enum_list_xzy_err_ok���ɹ�
		enum_list_xzy_err_param_NULL
		enum_list_xzy_err_param_wrong
	��ע��
		���ú���֮ǰ��Ҫ����list_xzy_init���������ʧ��
*/
ENUM_LIST_XZY_ERR list_xzy_start(volatile STRUCT_LIST_XZY *p_info)
{
	if(p_info==NULL)return enum_list_xzy_err_param_NULL;
	if(p_info->flag_on_off != enum_rw_step_work)return enum_list_xzy_err_param_wrong;
	//�ر�����
	p_info->flag_on_off = enum_rw_step_idle;
	//�رն���д
	p_info->r.flag_on_off = enum_rw_step_idle;
	p_info->w.flag_on_off = enum_rw_step_idle;
	//�����������
	p_info->count_w_cover = 0;
	//�����ֳ�ʼ��
	p_info->r.index = 0;
	p_info->r.step = enum_rw_step_idle;
	p_info->r.cir_times = 0;
	//д���ֳ�ʼ��
	p_info->w.index = 0;
	p_info->w.step = enum_rw_step_idle;
	p_info->w.cir_times = 0;
	//��������д
	p_info->r.flag_on_off = enum_rw_step_work;
	p_info->w.flag_on_off = enum_rw_step_work;
	//��������
	p_info->flag_on_off = enum_rw_step_work;
	
	return enum_list_xzy_err_ok;
}

/*
	���ƣ��������
	������
		p_info�����ڴ洢����������Ϣ
	����ֵ��
		enum_list_xzy_err_ok���ɹ�
		enum_list_xzy_err_param_NULL
		enum_list_xzy_err_param_wrong
	��ע��
*/
ENUM_LIST_XZY_ERR list_xzy_end(volatile STRUCT_LIST_XZY *p_info)
{
	if(p_info==NULL)return enum_list_xzy_err_param_NULL;
	if(p_info->flag_on_off != enum_rw_step_work)return enum_list_xzy_err_param_wrong;
	
	//�ر�����
	p_info->flag_on_off = enum_rw_step_idle;
	//�رն���д
	p_info->r.flag_on_off = enum_rw_step_idle;
	p_info->w.flag_on_off = enum_rw_step_idle;
	
	return enum_list_xzy_err_ok;
}

/*
	���ƣ������
	������
		p_info�����ڴ洢����������Ϣ
		p_data�����ڴ洢�����������������
		size_data��p_data�Ĵ�С���ֽڣ�
			��СֵΪlist_xzy_init�еĲ���unit_size
		flag_updata���Ƿ��ڶ�ȡ���ݺ����������Ϣ�ĸ���
			�����£�enum_rw_step_idle����ֻ������ȡ���ݳɹ����������Ȼ����������
			���£�enum_rw_step_work
	����ֵ��
		enum_list_xzy_err_ok���ɹ�
		enum_list_xzy_err_idle������û�пɶ�������
		enum_list_xzy_err_param_NULL
		enum_list_xzy_err_param_wrong
		enum_list_xzy_err_param_small
		enum_list_xzy_err_read_off
		enum_list_xzy_err_read_busy
		enum_list_xzy_err_recover		
	��ע��
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
	
	tmp_info = *p_info;//��Ҫ�����ж϶����������Ƿ񱻸���
	//�����������ڹ���״̬
	p_info->r.step = enum_rw_step_work;
	//����������ʱ��������ָ���Ƿ���Ҫ����
	if(p_info->flag_cover == enum_rw_step_work)
	{
		if(p_info->r.cir_times == p_info->w.cir_times)
			//����д����ͬһ��cir_times����δ���й����ǲ���
			;
		else if((p_info->r.cir_times+1) == p_info->w.cir_times && \
				p_info->r.index >= p_info->w.index
			)
			//д���ڶ�����һ��cir_times����дindexС�ڵ��ڶ�index����δ���й�����
			;
		else
		{
			p_info->r.cir_times = p_info->w.cir_times - 1;
			p_info->r.index = p_info->w.index;
		}
	}
	//�鿴�������Ƿ���������ȡ������
	if(p_info->r.cir_times == p_info->w.cir_times && \
		p_info->r.index == p_info->w.index
		)
		return enum_list_xzy_err_idle;
	//��ȡ����
	memcpy(p_data,p_info->cache_address + p_info->r.index,p_info->unit_size);	
	tmp_info_1 = *p_info;//��Ҫ�����ж϶����������Ƿ񱻸���
	//����������ʱ���鿴���ݶ�ȡ�������Ƿ�д�����޸Ĺ�
	if(p_info->flag_cover == enum_rw_step_work)
	{
		if(tmp_info.w.cir_times == tmp_info_1.w.cir_times && \
			tmp_info.w.index == tmp_info_1.w.index \
			)
			//���Ĺ�����δ����д����
			;
		if(p_info->r.index == tmp_info_1.w.index && \
			tmp_info_1.w.step == enum_rw_step_work)
		{//�����д������д��������дindex���index���
			return enum_list_xzy_err_recover;
		}
		else if(p_info->r.cir_times == tmp_info.w.cir_times && \
				p_info->r.index < tmp_info.w.index && \
				p_info->r.cir_times == tmp_info_1.w.cir_times && \
				p_info->r.index < tmp_info_1.w.index
				)
			//����д����ͬһ��cir_times
			;//
		else if(p_info->r.cir_times == tmp_info.w.cir_times && \
				p_info->r.index < tmp_info.w.index && \
				(p_info->r.cir_times+1) == tmp_info_1.w.cir_times && \
				p_info->r.index >= tmp_info_1.w.index
				)
			//��֮ǰ��д����ͬһ��cir_times����֮��д����һ��cir_times�����Ƕ�֮��Ķ�index����дindex
			;//
		else if((p_info->r.cir_times+1) == tmp_info.w.cir_times && \
				p_info->r.index >= tmp_info.w.index && \
				(p_info->r.cir_times+1) == tmp_info_1.w.cir_times && \
				p_info->r.index >= tmp_info_1.w.index
				)
			//��֮ǰ��д������һ��cir_times�����Ƕ�֮ǰ�Ķ�index����дindex
			//��֮��д������һ��cir_times�����Ƕ�֮��Ķ�index����дindex
			;//
		else
		{//���Ĺ����У��������ݿ��ܱ�д�޸Ĺ�
			return enum_list_xzy_err_recover;
		}
	}
	//���¼���
	if(flag_updata == enum_rw_step_work)
	{
		p_info->r.index += p_info->unit_size;
		p_info->r.cir_times += p_info->r.index / p_info->cache_size_value;
		p_info->r.index %= p_info->cache_size_value;
	}
	
	return enum_list_xzy_err_ok;
}

/*
	���ƣ����������ݵĿɶ�����
	������
		p_info�����ڴ洢����������Ϣ
		item_num���ɶ������Ĵ洢��ַ
	����ֵ��
		enum_list_xzy_err_ok���ɹ�
		enum_list_xzy_err_param_NULL
		enum_list_xzy_err_param_wrong
	��ע��
		�ú�����ȡ�Ĵ������ڲ�׼�����
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
	���ƣ�����д
	������
		p_info�����ڴ洢����������Ϣ
		p_data��д���������ݵ��׵�ַ
		write_bytes��д��������ֽ���
			���ֵΪlist_xzy_init�еĲ���unit_size
			��СֵΪ��
	����ֵ��
		enum_list_xzy_err_ok���ɹ�
		enum_list_xzy_err_param_NULL
		enum_list_xzy_err_param_wrong
		enum_list_xzy_err_write_busy
		enum_list_xzy_err_recover
	��ע��
*/
ENUM_LIST_XZY_ERR list_xzy_write(volatile STRUCT_LIST_XZY *p_info,unsigned char *p_data,unsigned int write_bytes)
{
	if(p_info==NULL)return enum_list_xzy_err_param_NULL;
	if(write_bytes > p_info->unit_size)return enum_list_xzy_err_param_wrong;
	if(p_info->flag_on_off != enum_rw_step_work)return enum_list_xzy_err_param_wrong;
	if(p_info->w.flag_on_off != enum_rw_step_work)return enum_list_xzy_err_param_wrong;
	if(p_info->w.step != enum_rw_step_work)return enum_list_xzy_err_write_busy;
	
	//��д�������ڹ���״̬
	p_info->w.step = enum_rw_step_work;
	//����������ʱ��д���Ƿ�Ḳ��
	if(p_info->flag_cover == enum_rw_step_idle)
	{
		if((p_info->r.cir_times + 1) == p_info->w.cir_times && \
			p_info->w.index == p_info->r.index \
			)
			return enum_list_xzy_err_recover;
	}
	//�����㣬��д�룬������Ϊ�˷�ֹд���ֽ�������ʱʣ��ռ����֮ǰ������
	memset(p_info->cache_address + p_info->w.index,0,p_info->unit_size);
	memcpy(p_info->cache_address + p_info->w.index,p_data,write_bytes);
	//���¼���
	p_info->w.index += p_info->unit_size;
	p_info->w.cir_times += p_info->w.index / p_info->cache_size_value;
	p_info->w.index %= p_info->cache_size_value;
	//��д�������ڿ���״̬
	p_info->w.step = enum_rw_step_idle;
	
	return enum_list_xzy_err_ok;
}
#endif
