/*
* ʱ�䶨ʱ��
* 
*
*
*/
#ifndef M_BASE_TIME_POOL_INCLUDE
#define M_BASE_TIME_POOL_INCLUDE

#include "slience/base/compatibility.hpp"
#include "slience/base/timer.hpp"

M_BASE_NAMESPACE_BEGIN


class TimerPool {
public:
	// ʱ��ڵ�
	struct TimeNode {
		TimeNode(){}
		base::s_int64_t expire;	// ��ʱʱ��(����)
		m_function_t<void()> cb;	// �ص�����
	};

	// @max_interval_day ����ܳ���7�죬Ĭ����1�죨24Сʱ)
	TimerPool(int max_interval_day = 1);

	~TimerPool();

	// ���ô˺�������ʱ�Ľڵ�ᱻ���ûص�
	void Update(const base::timestamp& now);

	// @interval�Ǻ���
	int AddTimer(int interval, m_function_t<void()> func);

protected:
	bool _CalcBucket(const base::timestamp& now, int interval, int& big_bucket, int& small_bucket);

private:
	int _max_interval_day;	// ����ʱ����������)
	void** _bucket;			// Ͱ�������Ǻ���
	base::s_int64_t _beg_time;
	int _big_bucket;
	int _small_bucket;
};


M_BASE_NAMESPACE_END
#endif