#ifndef M_NET_HANDLER_INCLUDE
#define M_NET_HANDLER_INCLUDE

#include "slience/base/singletion.hpp"
#include "commonlib/svr_base/svrbase.h"

// �����Ϣ����
#ifndef M_MAX_MESSAGE_LIST
#define M_MAX_MESSAGE_LIST (5000)
#endif

// expire�����
#ifndef M_EXPIRE_CHECK_INTERVAL
#define M_EXPIRE_CHECK_INTERVAL (15)
#endif

// expireʱ��
#ifndef M_EXPIRE_INTERVAL
#define M_EXPIRE_INTERVAL (30)
#endif

enum ConnType {
	Enum_ConnType_Router = 1,
};

struct ConnInfo {
	int conn_type;
	int serial_num;
	int port;
	char ip[65];

	std::string ToString();
};

class NetIoHandler : public netiolib::NetIo {
public:
	typedef m_function_t<int(base::s_int64_t fd, const AppHeadFrame& frame, 
		const char* data, base::s_uint32_t data_len)> callback_type;

	int Init(base::timestamp& now, callback_type callback);

	int Update();

	void OnTick();

	const base::timestamp& GetNow()const;

	void CheckTcpSocketExpire();

	bool SendDataByFd(base::s_int64_t fd, const char* data, base::s_int32_t len);

	void CloseFd(base::s_int64_t fd);

	netiolib::TcpConnectorPtr GetConnectorPtr(base::s_int64_t fd);

	netiolib::TcpSocketPtr GetSocketPtr(base::s_int64_t fd);

	bool ConnectOne(const std::string& addr, SocketLib::s_uint16_t port, 
		int conn_type, int serial_num);

	void ConnectOneHttp(const std::string& addr, SocketLib::s_uint16_t port, 
		int conn_type, int serial_num);

protected:
	void _ConnectOne(ConnInfo* info);

	virtual void OnConnection(netiolib::TcpConnectorPtr& clisock, SocketLib::SocketError error);

	virtual void OnConnection(netiolib::TcpSocketPtr& clisock);

	virtual void OnDisConnection(netiolib::TcpConnectorPtr& clisock);

	virtual void OnDisConnection(netiolib::TcpSocketPtr& clisock);

protected:
	void OnConnected(netiolib::TcpSocketPtr& clisock) override;

	void OnConnected(netiolib::TcpConnectorPtr& clisock, SocketLib::SocketError error) override;

	void OnDisconnected(netiolib::TcpSocketPtr& clisock) override;

	void OnDisconnected(netiolib::TcpConnectorPtr& clisock) override;

	void OnReceiveData(netiolib::TcpSocketPtr& clisock, SocketLib::Buffer& buffer) override;

	void OnReceiveData(netiolib::TcpConnectorPtr& clisock, SocketLib::Buffer& buffer) override;

protected:
	// ��ʵ��
	void OnConnected(netiolib::HttpSocketPtr& clisock) override {}
	void OnConnected(netiolib::HttpConnectorPtr& clisock, SocketLib::SocketError error) override {}
	void OnDisconnected(netiolib::HttpSocketPtr& clisock) override {}
	void OnDisconnected(netiolib::HttpConnectorPtr& clisock) override {}
	void OnReceiveData(netiolib::HttpSocketPtr& clisock, netiolib::HttpSvrRecvMsg& httpmsg) override {}
	void OnReceiveData(netiolib::HttpConnectorPtr& clisock, netiolib::HttpCliRecvMsg& httpmsg) override {}

	TcpSocketMsg* CreateTcpSocketMsg();
	TcpConnectorMsg* CreateTcpConnectorMsg();

protected:
	size_t _msg_cache_size;
	base::timestamp* _now;

	// callback
	callback_type _callback;

	// send buff
	base::Buffer _snd_buff;

	// message list
	base::MutexLock _msg_lock;
	base::svector<TcpSocketMsg*> _tcp_socket_msg_list;
	base::svector<TcpSocketMsg*> _tcp_socket_msg_list2;
	base::svector<TcpConnectorMsg*> _tcp_connector_msg_list;
	base::svector<TcpConnectorMsg*> _tcp_connector_msg_list2;

	// socket container
	TcpSocketContextContainer _tcp_socket_container;
	TcpConnectorContextContainer _tcp_connector_container;

	// last check expire time
	base::timestamp _last_check_time;
};

#ifndef NetIoHandlerSgl
#define NetIoHandlerSgl base::singleton<NetIoHandler>::mutable_instance()
#endif

#endif