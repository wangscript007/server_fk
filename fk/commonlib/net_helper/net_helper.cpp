#include "commonlib/net_helper/net_helper.h"
#include "slience/base/logger.hpp"
#include "protolib/src/svr_base.pb.h"
#include "protolib/src/cmd.pb.h"

NetHandler::NetHandler(base::timestamp& now, callback_type callback) : _now(now){
	_msg_cache_size = 5000;
	_callback = callback;
}

int NetHandler::Update() {
	static base::slist<TcpSocketMsg*> tmp_tcp_socket_msg_list;
	static base::slist<TcpSocketMsg*> tmp_tcp_socket_msg_list2;
	static base::slist<TcpConnectorMsg*> tmp_tcp_connector_msg_list;
	static base::slist<TcpConnectorMsg*> tmp_tcp_connector_msg_list2;

	auto &tcp_socket_fd_index = _tcp_socket_container.get<tag_socket_context_fd>();
	auto &tcp_conn_fd_index = _tcp_connector_container.get<tag_socket_context_fd>();

	// swap message
	if (_tcp_socket_msg_list.size() || _tcp_connector_msg_list.size()) {
		_msg_lock.lock();
		tmp_tcp_socket_msg_list.swap(_tcp_socket_msg_list);
		tmp_tcp_connector_msg_list.swap(_tcp_connector_msg_list);
		_msg_lock.unlock();
	}

	// process connector msglist
	while (tmp_tcp_connector_msg_list.size()) {
		TcpConnectorMsg* pmsg = tmp_tcp_connector_msg_list.front();
		if (pmsg->type == M_SOCKET_DATA) {
			base::s_int64_t fd = M_TCP_CONNECTOR_FD_FLAG | pmsg->ptr->GetFd();
			auto tmp_iter = tcp_conn_fd_index.find(fd);
			if (tmp_iter != tcp_conn_fd_index.end()) {
				tcp_conn_fd_index.modify(tmp_iter, FuncModifySocketContext(tmp_iter->msgcount + 1, GetNow().second()));
				AppHeadFrame& pFrame = *(AppHeadFrame*)pmsg->buf.Data();
				const char* data = (const char*)pmsg->buf.Data() + sizeof(AppHeadFrame);
				_callback(fd, pFrame, data, pFrame.get_cmd_length());
			}
			else {
				LogError(fd << " fd not found in _tcp_connector_container");
			}
		}
		else if (pmsg->type == M_SOCKET_IN) {
			OnConnection(pmsg->ptr, pmsg->error);
		}
		else if (pmsg->type == M_SOCKET_OUT) {
			OnDisConnection(pmsg->ptr);
		}
		else {
			assert(0);
		}
		tmp_tcp_connector_msg_list.pop_front();
		tmp_tcp_connector_msg_list2.push_back(pmsg);
	}

	// process tcp socket msg list
	while (tmp_tcp_socket_msg_list.size()) {
		TcpSocketMsg* pmsg = tmp_tcp_socket_msg_list.front();
		if (pmsg->type == M_SOCKET_DATA) {
			base::s_int64_t fd = M_TCP_FD_FLAG | pmsg->ptr->GetFd();
			auto tmp_iter = tcp_socket_fd_index.find(fd);
			if (tmp_iter != tcp_socket_fd_index.end()) {
				tcp_socket_fd_index.modify(tmp_iter, FuncModifySocketContext(tmp_iter->msgcount + 1, GetNow().second()));
				AppHeadFrame& pFrame = *(AppHeadFrame*)pmsg->buf.Data();
				const char* data = (const char*)pmsg->buf.Data() + sizeof(AppHeadFrame);
				_callback(fd, pFrame, data, pFrame.get_cmd_length());
			}
			else {
				LogError(fd << " fd not found in _tcp_socket_container");
			}
		}
		else if (pmsg->type == M_SOCKET_IN) {
			OnConnection(pmsg->ptr);
		}
		else if (pmsg->type == M_SOCKET_OUT) {
			OnDisConnection(pmsg->ptr);
		}
		else {
			assert(0);
		}
		tmp_tcp_socket_msg_list.pop_front();
		tmp_tcp_socket_msg_list2.push_back(pmsg);
	}

	// recycle msg buffer
	if (tmp_tcp_connector_msg_list2.size() || tmp_tcp_socket_msg_list2.size()) {
		_msg_lock.lock();
		// tmp_tcp_connector_msg_list2
		if ((tmp_tcp_connector_msg_list2.size() + _tcp_connector_msg_list2.size()) <= _msg_cache_size) {
			_tcp_connector_msg_list2.join(tmp_tcp_connector_msg_list2);
		}
		else {
			while (tmp_tcp_connector_msg_list2.size()) {
				if (_tcp_connector_msg_list2.size() < _msg_cache_size) {
					_tcp_connector_msg_list2.push_back(tmp_tcp_connector_msg_list2.front());
				}
				else {
					delete tmp_tcp_connector_msg_list2.front();
				}
				tmp_tcp_connector_msg_list2.pop_front();
			}
		}
		// tmp_tcp_socket_msg_list2
		if ((tmp_tcp_socket_msg_list2.size() + _tcp_socket_msg_list2.size()) <= _msg_cache_size) {
			_tcp_socket_msg_list2.join(tmp_tcp_socket_msg_list2);
		}
		else {
			while (tmp_tcp_socket_msg_list2.size()) {
				if (_tcp_socket_msg_list2.size() < _msg_cache_size) {
					_tcp_socket_msg_list2.push_back(tmp_tcp_socket_msg_list2.front());
				}
				else {
					delete tmp_tcp_socket_msg_list2.front();
				}
				tmp_tcp_socket_msg_list2.pop_front();
			}
		}
		_msg_lock.unlock();
	}
	else {
		// Sleep(1);
		return -1;
	}
	return 0;
}

void NetHandler::OnTick() {
	CheckTcpSocketExpire();
}

const base::timestamp& NetHandler::GetNow()const {
	return _now;
}

void NetHandler::CheckTcpSocketExpire() {
	static base::timestamp last_check_time;
	if (GetNow().second() - last_check_time.second() >= M_EXPIRE_CHECK_INTERVAL) {
		// ten second
		auto &tt_index = _tcp_socket_container.get<tag_socket_context_active>();
		for (auto iter = tt_index.begin(); iter != tt_index.end(); ++iter) {
			if ((GetNow().second() - iter->tt) >= M_EXPIRE_INTERVAL
				&& iter->ptr->IsConnected()) {
				base::s_int64_t fd = M_TCP_FD_FLAG | iter->ptr->GetFd();
				LogInfo("connection expire been closed, remote_ip: " << iter->ptr->RemoteEndpoint().Address()
					<< " fd: " << fd);
				iter->ptr->Close();
			}
			else {
				break;
			}
		}
		last_check_time = GetNow();
	}
}

void NetHandler::OnConnection(netiolib::TcpConnectorPtr& clisock, SocketLib::SocketError error) {
	if (!error) {
		// connect success
		base::s_int64_t fd = M_TCP_CONNECTOR_FD_FLAG | clisock->GetFd();
		TcpConnectorContext context;
		context.instid = 0;
		context.fd = fd;
		context.ptr = clisock;
		context.msgcount = 0;
		context.tt = GetNow().second();

		auto &fd_index = _tcp_connector_container.get<tag_socket_context_fd>();
		if (!fd_index.insert(context).second) {
			LogError("new connection insert fail, remote_ip: " << clisock->RemoteEndpoint().Address() << " fd: " << fd << " time: " << GetNow().format_ymd_hms());
			// close socket
			clisock->Close();
		}
		else {
			LogInfo("new connection insert success, remote_ip: " << clisock->RemoteEndpoint().Address() << " fd: " << fd << " time: " << GetNow().format_ymd_hms());
			proto::SocketClientIn client_in;
			std::string str = client_in.SerializeAsString();
			AppHeadFrame frame;
			frame.set_cmd(proto::CMD::CMD_SOCKET_CLIENT_IN);
			_callback(fd, frame, str.c_str(), str.size());
		}
	}
}

void NetHandler::OnConnection(netiolib::TcpSocketPtr& clisock) {
	base::s_int64_t fd = M_TCP_FD_FLAG | clisock->GetFd();
	TcpSocketContext context;
	context.instid = 0;
	context.fd = fd;
	context.ptr = clisock;
	context.msgcount = 0;
	context.tt = GetNow().second();

	auto &fd_index = _tcp_socket_container.get<tag_socket_context_fd>();
	if (!fd_index.insert(context).second) {
		LogError("new connection insert fail, remote_ip: " << clisock->RemoteEndpoint().Address() << " fd: " << fd << " time: " << GetNow().format_ymd_hms());
		// close socket
		clisock->Close();
	}
	else {
		LogInfo("new connection insert success, remote_ip: " << clisock->RemoteEndpoint().Address() << " fd: " << fd << " time: " << GetNow().format_ymd_hms());

		proto::SocketClientIn client_in;
		std::string str = client_in.SerializeAsString();
		AppHeadFrame frame;
		frame.set_cmd(proto::CMD::CMD_SOCKET_CLIENT_IN);
		_callback(fd, frame, str.c_str(), str.size());
	}
}

void NetHandler::OnDisConnection(netiolib::TcpConnectorPtr& clisock) {
	base::s_int64_t fd = M_TCP_CONNECTOR_FD_FLAG | clisock->GetFd();
	auto &fd_index = _tcp_connector_container.get<tag_socket_context_fd>();
	if (0 == fd_index.erase(fd)) {
		LogError("fd: " << fd << " not exist, this is a big bug!!!!!!!!!!!!!!!!!");
	}
	else {
		proto::SocketClientOut client_out;
		std::string str = client_out.SerializeAsString();
		AppHeadFrame frame;
		frame.set_cmd(proto::CMD::CMD_SOCKET_CLIENT_OUT);

		LogInfo("connection broken, remote_ip: " << clisock->RemoteEndpoint().Address() << " fd: " << fd << " time: " << GetNow().format_ymd_hms());
		_callback(fd, frame, str.c_str(), str.size());
	}
}

void NetHandler::OnDisConnection(netiolib::TcpSocketPtr& clisock) {
	base::s_int64_t fd = M_TCP_FD_FLAG | clisock->GetFd();
	auto &fd_index = _tcp_socket_container.get<tag_socket_context_fd>();
	auto iter = fd_index.find(fd);
	if (iter == fd_index.end()) {
		LogError("fd: " << fd << " not exist, this is a big bug!!!!!!!!!!!!!!!!!");
	}
	else {
		int instid = iter->instid;
		_instid_fd_map.erase(instid);
		fd_index.erase(iter);
		proto::SocketClientOut client_out;
		std::string str = client_out.SerializeAsString();
		AppHeadFrame frame;
		frame.set_dst_inst_id(instid);
		frame.set_cmd(proto::CMD::CMD_SOCKET_CLIENT_OUT);

		LogInfo("connection broken, remote_ip: " << clisock->RemoteEndpoint().Address() << " fd: " << fd << " time: " << GetNow().format_ymd_hms());
		_callback(fd, frame, str.c_str(), str.size());
	}
}

void NetHandler::OnConnected(netiolib::TcpSocketPtr& clisock) {
	base::ScopedLock scoped(_msg_lock);
	TcpSocketMsg* pMessage = 0;
	if (_tcp_socket_msg_list2.size() > 0) {
		pMessage = _tcp_socket_msg_list2.front();
		_tcp_socket_msg_list2.pop_front();
	}
	else {
		pMessage = new TcpSocketMsg;
	}
	pMessage->ptr = clisock;
	pMessage->type = M_SOCKET_IN;
	_tcp_socket_msg_list.push_back(pMessage);
}

void NetHandler::OnConnected(netiolib::TcpConnectorPtr& clisock, SocketLib::SocketError error) {
	base::ScopedLock scoped(_msg_lock);
	TcpConnectorMsg* pMessage = 0;
	if (_tcp_connector_msg_list2.size() > 0) {
		pMessage = _tcp_connector_msg_list2.front();
		_tcp_connector_msg_list2.pop_front();
	}
	else {
		pMessage = new TcpConnectorMsg;
	}

	pMessage->error = error;
	pMessage->ptr = clisock;
	pMessage->type = M_SOCKET_IN;
	_tcp_connector_msg_list.push_back(pMessage);
}

void NetHandler::OnDisconnected(netiolib::TcpSocketPtr& clisock) {
	base::ScopedLock scoped(_msg_lock);
	TcpSocketMsg* pMessage = 0;
	if (_tcp_socket_msg_list2.size() > 0) {
		pMessage = _tcp_socket_msg_list2.front();
		_tcp_socket_msg_list2.pop_front();
	}
	else {
		pMessage = new TcpSocketMsg;
	}
	;
	pMessage->ptr = clisock;
	pMessage->type = M_SOCKET_OUT;
	_tcp_socket_msg_list.push_back(pMessage);
}

void NetHandler::OnDisconnected(netiolib::TcpConnectorPtr& clisock) {
	base::ScopedLock scoped(_msg_lock);
	TcpConnectorMsg* pMessage = 0;
	if (_tcp_connector_msg_list2.size() > 0) {
		pMessage = _tcp_connector_msg_list2.front();
		_tcp_connector_msg_list2.pop_front();
	}
	else {
		pMessage = new TcpConnectorMsg;
	}

	pMessage->ptr = clisock;
	pMessage->type = M_SOCKET_OUT;
	_tcp_connector_msg_list.push_back(pMessage);
}

void NetHandler::OnReceiveData(netiolib::TcpSocketPtr& clisock, SocketLib::Buffer& buffer) {
	base::ScopedLock scoped(_msg_lock);
	if (_tcp_socket_msg_list.size() >= M_MAX_MESSAGE_LIST) {
		// message list is too many
		LogError("tcp_socket_msg_list is too many, new message will be dropped");
		return;
	}

	TcpSocketMsg* pMessage = 0;
	if (_tcp_socket_msg_list2.size() > 0) {
		pMessage = _tcp_socket_msg_list2.front();
		_tcp_socket_msg_list2.pop_front();
	}
	else {
		pMessage = new TcpSocketMsg;
	}

	pMessage->ptr = clisock;
	pMessage->type = M_SOCKET_DATA;
	pMessage->buf.Write(buffer.Data(), buffer.Length());
	_tcp_socket_msg_list.push_back(pMessage);
}

void NetHandler::OnReceiveData(netiolib::TcpConnectorPtr& clisock, SocketLib::Buffer& buffer) {
	base::ScopedLock scoped(_msg_lock);
	if (_tcp_connector_msg_list.size() >= M_MAX_MESSAGE_LIST) {
		// message list is too many
		LogError("tcp_connector_msg_list is too many, new message will be dropped");
		return;
	}

	TcpConnectorMsg* pMessage = 0;
	if (_tcp_connector_msg_list2.size() > 0) {
		pMessage = _tcp_connector_msg_list2.front();
		_tcp_connector_msg_list2.pop_front();
	}
	else {
		pMessage = new TcpConnectorMsg;
	}

	pMessage->ptr = clisock;
	pMessage->type = M_SOCKET_DATA;
	pMessage->buf.Write(buffer.Data(), buffer.Length());
	_tcp_connector_msg_list.push_back(pMessage);
}