#ifndef M_BASE_TRANSACTION_INCLUDE
#define M_BASE_TRANSACTION_INCLUDE

//#include "google/protobuf/message.h"
#include "slience/base/config.hpp"
#include "slience/base/logger.hpp"
#include "commonlib/svr_base/svrbase.h"
#include "commonlib/net_handler/router_mgr.h"

class Transaction {
	friend class TransactionMgrImpl;
public:
	enum State{
		E_STATE_IDLE = 0,
		E_STATE_ACTIVE,
		E_STATE_TIMEOUT,
	};
	enum Wait_Return {
		E_RETURN_ERROR = 0,
		E_RETURN_ACTIVE = 1,
		E_RETURN_TIMEOUT = 2,
	};

	void Construct();

	Transaction(int cmd);

	virtual ~Transaction();

protected:
	virtual int InCoroutine() = 0;

	int Process(base::s_int64_t fd, base::s_uint32_t self_svr_type, 
		base::s_uint32_t self_inst_id,
		const AppHeadFrame& frame, const char* data);

	int ParseMsg(google::protobuf::Message& message);

	int SendMsgByServerType(int cmd, int svr_type,
		google::protobuf::Message& request, google::protobuf::Message& respond);
	
	int SendMsgByServerType(int cmd, int svr_type,
		google::protobuf::Message& request);

	int SendMsgByFd(int cmd, base::s_int64_t fd,
		google::protobuf::Message& request, google::protobuf::Message& respond);

	int SendMsgByFd(int cmd, base::s_int64_t fd,
		google::protobuf::Message& request);

protected:
	void OnState();

	void CancelTimer();

	void SetTimer(int interval);

	Wait_Return Wait(int interval);

	int OnIdle();

	int OnActive();

	int OnTimeOut();

protected:
	base::s_uint32_t trans_id();

	base::s_uint32_t cmd();

	base::s_uint64_t userid();

	base::s_int32_t co_id();

	base::s_int64_t fd();

	base::s_int64_t cur_fd();

	base::s_uint32_t self_svr_type();

	base::s_uint32_t self_inst_id();

	void set_co_id(base::s_int32_t);

	const AppHeadFrame& cur_frame_head();

	const AppHeadFrame& ori_frame_head();

private:
	base::s_uint16_t _state;
	base::s_uint64_t _timer_id;
	base::s_uint32_t _trans_id;
	base::s_uint32_t _cmd;
	base::s_uint64_t _userid;
	base::s_int32_t _co_id;
	base::s_int64_t _fd;
	base::s_int64_t _cur_fd;
	base::s_uint32_t _self_svr_type;
	base::s_uint32_t _self_inst_id;
	const AppHeadFrame* _cur_frame_head;
	AppHeadFrame _ori_frame_head;
	const char* _cur_frame_data;
};

////////////////////////////////////////////////////////////////////////////////

// 单向
template<typename TRANS_TYPE, typename REQUEST_TYPE>
class OneWayTransaction : public Transaction {
public:
	OneWayTransaction(int cmd) : Transaction(cmd) {}

protected:
	int InCoroutine() override {
		REQUEST_TYPE request;
		int ret = ParseMsg(request);
		if (0 != ret) {
			LogError("userid: " << userid() << " cmd: " << cmd() << " parse message fail");
			return -1;
		}
		LogDebug("userid: " << userid() << " cmd: " << cmd() << " REQUEST_TYPE=" << request.GetTypeName().c_str() << "|" << request.ShortDebugString().c_str());
		TRANS_TYPE* trans = dynamic_cast<TRANS_TYPE*>(this);
		if (!trans) {
			LogError("userid: " << userid() << " cmd: " << cmd() << "transaction dynamic_cast error");
			return -2;
		}
		ret = trans->OnRequest(request);
		return 0;
	}
};

// 双向
template<typename TRANS_TYPE, typename REQUEST_TYPE, typename RESPOND_TYPE>
class TwoWayTransaction : public Transaction {
public:
	TwoWayTransaction(int cmd) : Transaction(cmd) {}

protected:
	int InCoroutine() override {
		REQUEST_TYPE request;
		int ret = ParseMsg(request);
		if (0 != ret) {
			LogError("userid: " << userid() << " cmd: " << cmd() << " parse message fail");
			return -1;
		}
		LogDebug("userid: " << userid() << " cmd: " << cmd() << " REQUEST_TYPE=" << request.GetTypeName().c_str() << "|" << request.ShortDebugString().c_str());
		TRANS_TYPE* trans = dynamic_cast<TRANS_TYPE*>(this);
		if (!trans) {
			LogError("userid: " << userid() << " cmd: " << cmd() << "transaction dynamic_cast error");
			return -2;
		}
		RESPOND_TYPE respond;
		ret = trans->OnRequest(request, respond);
		if (0 == ret) {
			// 回包
			LogDebug("userid: " << userid() << " cmd: " << cmd() << " RESPOND_TYPE=" << respond.GetTypeName().c_str() << "|" << respond.ShortDebugString().c_str());
			AppHeadFrame frame;
			frame.set_is_broadcast(false);
			frame.set_src_svr_type(self_svr_type());
			frame.set_dst_svr_type(ori_frame_head().get_src_inst_id());
			frame.set_src_inst_id(self_inst_id());
			frame.set_dst_inst_id(ori_frame_head().get_src_inst_id());
			frame.set_src_trans_id(0);
			frame.set_dst_trans_id(ori_frame_head().get_src_trans_id());
			frame.set_cmd(cmd() + 1);
			std::string msg = respond.SerializePartialAsString();
			frame.set_cmd_length(msg.length());
			frame.set_userid(userid());
			RouterMgrSgl.SendMsg(frame, msg);
		}
		return 0;
	}
};

struct RespondNull {};

// 
template <typename TRANS_TYPE, typename REQUEST_TYPE, typename RESPOND_TYPE = RespondNull>
class BaseTransaction : public TwoWayTransaction<TRANS_TYPE, REQUEST_TYPE, RESPOND_TYPE> {
public:
	BaseTransaction(unsigned int cmd) : TwoWayTransaction<TRANS_TYPE, REQUEST_TYPE, RESPOND_TYPE>(cmd) {}
};

template <typename TRANS_TYPE, typename REQUEST_TYPE>
class BaseTransaction<TRANS_TYPE, REQUEST_TYPE, RespondNull> : public OneWayTransaction<TRANS_TYPE, REQUEST_TYPE> {
public:
	BaseTransaction(unsigned int cmd) : OneWayTransaction<TRANS_TYPE, REQUEST_TYPE>(cmd) {}
};

#endif