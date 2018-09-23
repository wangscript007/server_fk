#ifndef M_NETIO_TCP_STREAM_SOCKET_INCLUDE
#define M_NETIO_TCP_STREAM_SOCKET_INCLUDE

#include "slience/netio/config.hpp"
M_NETIO_NAMESPACE_BEGIN

template<typename T, typename SocketType>
class TcpStreamSocket : public TcpBaseSocket<T, SocketType> {
protected:
	struct _readerinfo_ {
		base::s_byte_t* readbuf;
		SocketLib::Buffer msgbuffer;
		
		_readerinfo_();
		~_readerinfo_();
	};

	_readerinfo_ _reader;

	void _ReadHandler(base::s_uint32_t tran_byte, SocketLib::SocketError error);

	// �ü������ݰ�������false��ζ�����ݰ��д�
	bool _CutMsgPack(base::s_byte_t* buf, base::s_uint32_t tran_byte);

	void _TryRecvData();

public:
	TcpStreamSocket(NetIo& netio);

	bool SendPacket(const base::s_byte_t* data, base::s_uint32_t len);

	template<typename MsgHeadType>
	bool SendPacket(const MsgHeadType& head, const base::s_byte_t* data,
		base::s_uint32_t len);
};

template<typename T, typename SocketType>
TcpStreamSocket<T, SocketType>::_readerinfo_::_readerinfo_() {
	readbuf = new base::s_byte_t[M_SOCKET_READ_SIZE];
	g_memset(readbuf, 0, M_SOCKET_READ_SIZE);
}

template<typename T, typename SocketType>
TcpStreamSocket<T, SocketType>::_readerinfo_::~_readerinfo_() {
	delete[]readbuf;
}

template<typename T, typename SocketType>
void TcpStreamSocket<T, SocketType>::_ReadHandler(base::s_uint32_t tran_byte, SocketLib::SocketError error) {
	do {
		// ����ر�����
		if (error) {
			M_NETIO_LOGGER("read handler happend error:" << M_ERROR_DESC_STR(error));
			break;
		}
		// �Է��ر�д
		if (tran_byte <= 0)
			break;

		// �ҷ�post�˹ر�
		if (this->_flag != E_STATE_START)
			break;

		if (_CutMsgPack(_reader.readbuf, tran_byte)) {
			_TryRecvData();
			return;
		}
		else {
			// ���ݼ����������Ͽ�����
			this->_socket->Shutdown(SocketLib::E_Shutdown_RD, error);
		}
	} while (false);

	this->_PostClose();
}

template<typename T, typename SocketType>
bool TcpStreamSocket<T, SocketType>::_CutMsgPack(base::s_byte_t* buf, base::s_uint32_t tran_byte) {
	// �����ڴ濽���Ǵ˺�������ƹؼ�
	base::s_uint32_t hdrlen = (base::s_uint32_t)sizeof(PacketHeader);
	shard_ptr_t<T> ref;
	base::s_byte_t* data = 0;
	base::s_uint32_t datalen = 0;

	do {
		if (_reader.msgbuffer.Length() == 0) {
			if (tran_byte < hdrlen) {
				_reader.msgbuffer.Write(buf, tran_byte);
				break;
			}
			PacketHeader* header = (PacketHeader*)buf;
			header->n2h();
			if (tran_byte - hdrlen < header->size) {
				header->n2h();
				_reader.msgbuffer.Write(buf, tran_byte);
				break;
			}
			if (header->timestamp != 0xFCFCFCFC) {
				return false;
			}
			
			data = buf + hdrlen;
			datalen = header->size;
			tran_byte -= (hdrlen + header->size);
			buf += (hdrlen + header->size);
		}
		else {
			// base

			if (_reader.msgbuffer.Length() + tran_byte < hdrlen) {
				_reader.msgbuffer.Write(buf, tran_byte);
				break;
			}
			if (_reader.msgbuffer.Length() < hdrlen) {
				//_reader.msgbuffer.Write(buf, )
			}
		}

		if (data) {
			if (datalen > M_SOCKET_PACK_SIZE) {
				return false;
			}
			if (!ref) {
				ref = this->shared_from_this();
			}
			this->_netio.OnReceiveData(ref, data, datalen);
			data = 0;
			datalen = 0;
		}
	} while (true);
	return true;
}

template<typename T, typename SocketType>
void TcpStreamSocket<T, SocketType>::_TryRecvData() {
	SocketLib::SocketError error;
	this->_socket->AsyncRecvSome(m_bind_t(&TcpStreamSocket::_ReadHandler, this->shared_from_this(), placeholder_1, placeholder_2)
		, _reader.readbuf, M_SOCKET_READ_SIZE, error);
	if (error)
		this->_PostClose();
}

template<typename T, typename SocketType>
TcpStreamSocket<T, SocketType>::TcpStreamSocket(NetIo& netio)
	:TcpBaseSocket<T, SocketType>(netio) {
}

template<typename T, typename SocketType>
bool TcpStreamSocket<T, SocketType>::SendPacket(const base::s_byte_t* data,
	base::s_uint32_t len) {
	SocketLib::ScopedLock scoped_w(_writer.lock);
	if (!_CheckCanSend(len + sizeof(PacketHeader))) {
		return false;
	}

	PacketHeader hdr;
	hdr.size = len;
	hdr.timestamp = 0xFCFCFCFC;
	hdr.h2n();
	_writer.msgbuffer2.Write(hdr);
	_writer.msgbuffer2.Write((void*)data, len);
	_TrySendData();
	return true;
}

template<typename T, typename SocketType>
template<typename MsgHeadType>
bool TcpStreamSocket<T, SocketType>::SendPacket(const MsgHeadType& head, 
	const base::s_byte_t* data, base::s_uint32_t len) {
	SocketLib::ScopedLock scoped_w(_writer.lock);
	if (!_CheckCanSend(len + sizeof(PacketHeader) + sizeof(MsgHeadType))) {
		return false;
	}

	PacketHeader hdr;
	hdr.size = len + sizeof(MsgHeadType);
	hdr.timestamp = 0xFCFCFCFC;
	hdr.h2n();
	_writer.msgbuffer2.Write(hdr);
	_writer.msgbuffer2.Write(head);
	_writer.msgbuffer2.Write((void*)data, len);
	_TrySendData();
	return true;
}

M_NETIO_NAMESPACE_END
#endif