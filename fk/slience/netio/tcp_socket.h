#ifndef M_NETIO_TCP_SOCKET_INCLUDE
#define M_NETIO_TCP_SOCKET_INCLUDE

#include "slience/netio/config.hpp"
M_NETIO_NAMESPACE_BEGIN

// class tcpsocket
class TcpSocket :
	public TcpStreamSocket<TcpSocket, SocketLib::TcpSocket<SocketLib::IoService> >
{
	friend class NetIo;

public:
	TcpSocket(NetIo& netio);

	SocketLib::TcpSocket<SocketLib::IoService>& GetSocket();

protected:
	void Init();
};

M_NETIO_NAMESPACE_END
#endif