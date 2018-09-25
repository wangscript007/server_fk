#ifndef M_NETIO_SYNC_CONNECTOR_INCLUDE
#define M_NETIO_SYNC_CONNECTOR_INCLUDE

#include "slience/netio/config.hpp"
M_NETIO_NAMESPACE_BEGIN

// ͬ��connector
class SyncConnector {
public:
	SyncConnector();

	~SyncConnector();

	bool Connect(const SocketLib::Tcp::EndPoint& ep, base::s_uint32_t timeo_sec = -1);

	bool Connect(const std::string& addr, base::s_uint16_t port, base::s_uint32_t timeo_sec = -1);

	const SocketLib::Tcp::EndPoint& LocalEndpoint()const;

	const SocketLib::Tcp::EndPoint& RemoteEndpoint()const;

	// ���������������ζ�����������Ͽ���������е�δ��������ݴ�����ͻ�Ͽ�
	void Close();

	bool Send(const base::s_byte_t* data, base::s_uint32_t len);

	bool IsConnected()const;

	SocketLib::Buffer* Recv();

	void SetTimeOut(base::s_uint32_t timeo);

protected:
	base::s_uint32_t _LocalEndian()const;

	SocketLib::Buffer* _CutMsgPack(base::s_byte_t* buf, base::s_uint32_t& tran_byte);

protected:
	SocketLib::IoService _ioservice;
	SocketLib::TcpConnector<SocketLib::IoService>* _socket;

	// endpoint
	SocketLib::Tcp::EndPoint _localep;
	SocketLib::Tcp::EndPoint _remoteep;

	PacketHeader _curheader;
	base::s_byte_t*  _readbuf;
	base::s_uint32_t _readsize;

	SocketLib::Buffer _sndbuffer;
	SocketLib::Buffer _rcvbuffer;
	// ״̬��־
	unsigned short _flag;
};


M_NETIO_NAMESPACE_END
#endif