/*----------------------------------------------------------------
// Copyright (C) 2017 public
//
// �޸��ˣ�xiaoquanjie
// ʱ�䣺2017/11/10
//
// �޸��ˣ�xiaoquanjie
// ʱ�䣺
// �޸�˵����
//
// �汾��V1.0.0
//----------------------------------------------------------------*/

#ifndef M_NETIO_NETIO_INCLUDE
#define M_NETIO_NETIO_INCLUDE

#include "slience/netio/config.hpp"
M_NETIO_NAMESPACE_BEGIN

class NetIo;
class TcpSocket;
class TcpConnector;
class HttpSocket;
class HttpConnector;
class SyncConnector;

typedef SocketLib::Buffer Buffer;
typedef shard_ptr_t<SocketLib::Buffer> BufferPtr;
typedef shard_ptr_t<TcpSocket>		   TcpSocketPtr;
typedef shard_ptr_t<TcpConnector>	   TcpConnectorPtr;
typedef shard_ptr_t<HttpSocket>		   HttpSocketPtr;
typedef shard_ptr_t<HttpConnector>	   HttpConnectorPtr;
typedef shard_ptr_t<SocketLib::TcpAcceptor<SocketLib::IoService> > TcpAcceptorPtr;
typedef shard_ptr_t<SyncConnector>  SyncConnectorPtr;

#ifndef lasterror
#define lasterror base::tlsdata<SocketLib::SocketError,0>::data()
#endif

class NetIo {
public:
	NetIo();
	NetIo(base::s_uint32_t backlog);

	virtual ~NetIo();

	// ����һ������
	bool ListenOne(const SocketLib::Tcp::EndPoint& ep);
	bool ListenOne(const std::string& addr, base::s_uint16_t port);

	// ����һ��http����
	bool ListenOneHttp(const SocketLib::Tcp::EndPoint& ep);
	bool ListenOneHttp(const std::string& addr, base::s_uint16_t port);

	// �첽����
	void ConnectOne(const SocketLib::Tcp::EndPoint& ep);
	void ConnectOne(const std::string& addr, base::s_uint16_t port);

	void ConnectOneHttp(const SocketLib::Tcp::EndPoint& ep);
	void ConnectOneHttp(const std::string& addr, base::s_uint16_t port);

	virtual void Start(unsigned int thread_cnt, bool isco = false);
	virtual void Stop();
	virtual void RunHandler();
	size_t  ServiceCount();

	// ��ȡ�����쳣
	SocketLib::SocketError GetLastError()const;
	SocketLib::IoService& GetIoService();
	base::s_uint32_t LocalEndian()const;

	/*
	*����������������Ϊ�麯�����Ա����ʵ��ҵ���ģʽ��������ģʽ����Ϣ���ַ�����
	*��֤ͬһ��socket���������������ĵ�����ѭOnConnected -> OnReceiveData -> OnDisconnected��˳��
	*��֤ͬһ��socket�����º����������ĵ��ö���ͬһ���߳���
	*/

	// ����֪ͨ,��������ﲻҪ����ҵ�񣬷�ֹ����
	virtual void OnConnected(TcpSocketPtr& clisock);
	virtual void OnConnected(TcpConnectorPtr& clisock, SocketLib::SocketError error);
	virtual void OnConnected(HttpSocketPtr& clisock);
	virtual void OnConnected(HttpConnectorPtr& clisock, SocketLib::SocketError error);

	// ����֪ͨ,��������ﲻҪ����ҵ�񣬷�ֹ����
	virtual void OnDisconnected(TcpSocketPtr& clisock);
	virtual void OnDisconnected(TcpConnectorPtr& clisock);
	virtual void OnDisconnected(HttpSocketPtr& clisock);
	virtual void OnDisconnected(HttpConnectorPtr& clisock);

	// ���ݰ�֪ͨ,��������ﲻҪ����ҵ�񣬷�ֹ����
	virtual void OnReceiveData(TcpSocketPtr& clisock, const base::s_byte_t* data, base::s_uint32_t len);
	virtual void OnReceiveData(TcpConnectorPtr& clisock, const base::s_byte_t* data, base::s_uint32_t len);
	virtual void OnReceiveData(HttpSocketPtr& clisock, HttpSvrRecvMsg& httpmsg);
	virtual void OnReceiveData(HttpConnectorPtr& clisock, HttpCliRecvMsg& httpmsg);

protected:
	void _Start(void*p);
	void _AcceptHandler(SocketLib::SocketError error, TcpSocketPtr& clisock, 
		TcpAcceptorPtr& acceptor);
	void _AcceptHttpHandler(SocketLib::SocketError error, HttpSocketPtr& clisock, 
		TcpAcceptorPtr& acceptor);

protected:
	NetIo(const NetIo&);
	NetIo& operator=(const NetIo&);

protected:
	SocketLib::IoService   _ioservice;
	base::s_uint32_t  _backlog;
	base::s_uint32_t  _endian;
	base::slist<base::thread*> _threadlist;
};

enum {
	E_STATE_START = 1,
	E_STATE_STOP,
	E_STATE_CLOSE,
};

M_NETIO_NAMESPACE_END
#endif

#include "slience/netio/packet.hpp"
#include "slience/netio/tcp_base_socket.hpp"
#include "slience/netio/tcp_stream_socket.hpp"
#include "slience/netio/tcp_socket.h"
#include "slience/netio/tcp_connector.h"
#include "slience/netio/httpmsg.hpp"
#include "slience/netio/http_base_socket.hpp"
#include "slience/netio/http_socket.h"
#include "slience/netio/http_connector.h"
#include "slience/netio/sync_connector.h"