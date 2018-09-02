#ifndef M_COMMONLIB_TRANSACTION_MGR_H
#define M_COMMONLIB_TRANSACTION_MGR_H

#include <vector>
#include "slience/coroutine/coroutine.hpp"
#include "slience/base/time_pool.h"
#include "commonlib/transaction/base_transaction.h"

class Transaction;
struct AppHeadFrame;

class TransactionBucket {
public:
	TransactionBucket(base::s_uint32_t cmd);

	virtual ~TransactionBucket();

	virtual Transaction* CreateTransaction() = 0;

	virtual void RecycleTransaction(Transaction*) = 0;

	int Size() {
		return _trans_vec.size();
	}

protected:
	base::s_uint32_t _cmd;
	std::vector<Transaction*> _trans_vec;
};

template<typename TransactionType>
class TransactionBucketImpl : public TransactionBucket {
public:
	TransactionBucketImpl(base::s_uint32_t cmd) : TransactionBucket(cmd) {
	}

	Transaction* CreateTransaction() override {
		if (_trans_vec.empty()) {
			Transaction* p = new TransactionType(_cmd);
			return p;
		}
		else {
			Transaction* p = _trans_vec.back();
			p->Construct();
			_trans_vec.pop_back();
			return p;
		}
	}

	void RecycleTransaction(Transaction* p) override {
		_trans_vec.push_back(p);
	}
};

///////////////////////////////////////////////////////////////////////////

class TransactionMgrImpl {
	friend class Transaction;

public:
	TransactionMgrImpl();

	void Init();

	void Init(base::s_int32_t max_concurrent_trans);

	void Update(const base::timestamp& now);

	int ProcessFrame(base::s_int64_t fd, const AppHeadFrame& frame, const char* data);

	void CoroutineEnter(void* p);

	void TimerCallback(base::s_uint32_t trans_id);

	int CancelTimer(base::s_uint64_t id);

	base::s_uint64_t AddTimer(int interval, base::s_uint32_t trans_id);

	base::s_uint32_t GeneratorTransId();

	Transaction* CreateTransaction(base::s_uint64_t uid, base::s_uint32_t cmd);

	Transaction* GetTransaction(base::s_uint32_t trans_id);

	base::s_int32_t GetActiveTransCnt();

	template<int CMD, typename TransactionType>
	int RegisterTransaction();

	void RecyleTransaction(Transaction*);

	void PrintStatus();

private:
	base::s_int32_t _max_concurrent_trans;
	base::TimerPool _timer_pool;
	base::s_uint32_t _trans_id_generator;
	base::timestamp _last_check_time;
	m_unorder_map_t<base::s_uint32_t, Transaction*> _active_trans_map;
	m_unorder_map_t<int, TransactionBucket*> _trans_bucket_map;
};

template<int CMD, typename TransactionType>
int TransactionMgrImpl::RegisterTransaction() {
	LogInfo("register transaction info, cmd:" << CMD << " type:" << typeid(TransactionType).name());	
	auto iter = _trans_bucket_map.find(CMD);
	if (iter == _trans_bucket_map.end()) {
		TransactionBucket* bucket = new TransactionBucketImpl<TransactionType>(CMD);
		_trans_bucket_map[CMD] = bucket;
		return 0;
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////

class TransactionMgr {
public:
	static void Init() {
		return GetImpl()->Init();
	}

	static void Init(base::s_int32_t max_concurrent_trans) {
		return GetImpl()->Init(max_concurrent_trans);
	}

	static void Update(const base::timestamp& now) {
		return GetImpl()->Update(now);
	}

	static int ProcessFrame(base::s_int64_t fd, const AppHeadFrame& frame, const char* data) {
		return GetImpl()->ProcessFrame(fd, frame, data);
	}

	static void CoroutineEnter(void* p) {
		return GetImpl()->CoroutineEnter(p);
	}

	static void TimerCallback(base::s_uint32_t trans_id) {
		return GetImpl()->TimerCallback(trans_id);
	}

	static int CancelTimer(base::s_uint64_t id) {
		return GetImpl()->CancelTimer(id);
	}

	static base::s_uint64_t AddTimer(int interval, base::s_uint32_t trans_id) {
		return GetImpl()->AddTimer(interval, trans_id);
	}

	static base::s_uint32_t GeneratorTransId() {
		return GetImpl()->GeneratorTransId();
	}

	static Transaction* CreateTransaction(base::s_uint64_t uid, base::s_uint32_t cmd) {
		return GetImpl()->CreateTransaction(uid, cmd);
	}

	static Transaction* GetTransaction(base::s_uint32_t trans_id) {
		return GetImpl()->GetTransaction(trans_id);
	}

	static base::s_int32_t GetActiveTransCnt() {
		return GetImpl()->GetActiveTransCnt();
	}

	template<int CMD, typename TransactionType>
	static int RegisterTransaction() {
		return GetImpl()->RegisterTransaction<CMD, TransactionType>();
	}

protected:
	static TransactionMgrImpl* GetImpl() {
		static TransactionMgrImpl impl;
		return &impl;
	}

	void RecyleTransaction(Transaction* p) {
		return GetImpl()->RecyleTransaction(p);
	}

	void PrintStatus() {
		return GetImpl()->PrintStatus();
	}
};

///////////////////////////////////////////////////////////////////////////

// ����ע��
#define REGISTER_TRANSACTION(cmd, TRANS_TYPE) \
enum cmd##_ {};\
enum TRANS_TYPE##_{};\
	int ret_##cmd = TransactionMgr::RegisterTransaction<proto::CMD::cmd, TRANS_TYPE>();

#endif