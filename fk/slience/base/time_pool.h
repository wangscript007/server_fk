/*
* ʱ�䶨ʱ��
* 
*
*
*/
#ifndef M_BASE_TIME_POOL_INCLUDE
#define M_BASE_TIME_POOL_INCLUDE

#include "slience/base/object_pool.h"
#include "slience/base/compatibility.hpp"
#include "slience/base/timer.hpp"

M_BASE_NAMESPACE_BEGIN


class TimerPool {
public:
	// ʱ��ڵ�
	struct TimeNode {
		base::s_int64_t expire;	// ��ʱʱ��(����)
		m_function_t<void(int param1, void* param2)> cb;	// �ص�����
	};

	// @max_interval_day ����ܳ���7�죬Ĭ����1�죨24Сʱ)
	TimerPool(int max_interval_day = 1);

	~TimerPool();

	// ���ô˺�������ʱ�Ľڵ�ᱻ���ûص�
	void Update(const base::timestamp& now);

	// @interval�Ǻ���
	bool AddTimer(int interval, m_function_t<void(int param1, void* param2)> func);

private:
	int _max_interval_day;	// ����ʱ����������)
	void** _big_bucket;		// ��Ͱ����������
	base::s_int64_t _beg_time;
	int _big_bucket_idx;
	int _small_bucket_idx;
};


M_BASE_NAMESPACE_END
#endif