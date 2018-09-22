#include "commonlib/net_handler/router_mgr.h"
#include "slience/base/logger.hpp"
#include "commonlib/net_handler/net_handler.h"
#include <map>

RouterMgr::RouterMgr() {

}

int RouterMgr::Init() {
	int ret = 0;
	do {
		ret = Reload();
		if (0 != ret) {
			LogError("reload fail");
			break;
		}
	} while (false);
	return ret;
}

int RouterMgr::Reload() {
	ServerCfg<config::RouterConfig> tmp_router_config;
	if (0 != tmp_router_config.Parse(_router_file.c_str())) {
		LogError("_router_config.Parse fail: " << _router_file);
		return -1;
	}

	std::set<int> number_set;
	for (int idx = 0; idx < tmp_router_config.Data().router_list_size(); ++idx) {
		int number = tmp_router_config.Data().router_list(idx).number();
		if (number_set.count(number) > 0) {
			LogError("router number is duplicated: " << number);
			return -1;
		}
	}

	_router_config.Data().Swap(&tmp_router_config.Data());
	if (0 != ConnectRouters()) {
		LogError("ConnectRouters fail");
		return -1;
	}
	return 0;
}

void RouterMgr::SetRouterFile(const std::string& router_file) {
	_router_file = router_file;
}

int RouterMgr::ConnectRouters() {
	// ���ԭ���ǣ������ٹ�
	std::map<int, RouterInfo> tmp_router_info_map;
	for (int idx = 0; idx < _router_config.Data().router_list_size(); ++idx) {
		auto& item = _router_config.Data().router_list(idx);
		if (!ExistRouter(item.listen_ip(), item.listen_port(), item.number())) {
			NetIoHandlerSgl.ConnectOne(item.listen_ip(), item.listen_port(),
				Enum_ConnType_Router, item.number());

			RouterInfo router_info;
			router_info.ip = item.listen_ip();
			router_info.port = item.listen_port();
			router_info.number = item.number();
			router_info.fd = 0;
			tmp_router_info_map[item.number()] = router_info;
		}
	}

	// �ѱ��ر��˵�
	for (auto iter = _router_info_vec.begin(); iter != _router_info_vec.end();) {
		bool exist = false;
		for (int idx = 0; idx < _router_config.Data().router_list_size(); ++idx) {
			auto& item = _router_config.Data().router_list(idx);
			if (iter->ip == item.listen_ip()
				&& iter->port == item.listen_port()
				&& iter->number == item.number()) {
				exist = true;
				break;
			}
		}
		if (!exist) {
			NetIoHandlerSgl.CloseFd(iter->fd);
		}
		else {
			tmp_router_info_map[iter->number] = *iter;
			iter++;
		}
	}

	_router_info_vec.clear();
	for (auto iter = tmp_router_info_map.begin(); iter != tmp_router_info_map.end();
		++iter) {
		_router_info_vec.push_back(iter->second);
	}
	return 0;
}

bool RouterMgr::ExistRouter(const std::string& ip, unsigned int port, int number) {
	for (auto iter = _router_info_vec.begin(); iter != _router_info_vec.end(); ++iter) {
		if (ip == iter->ip
			&& port == iter->port
			&& number == iter->number) {
			return true;
		}
	}
	return false;
}

int RouterMgr::AddRouter(const std::string& ip, unsigned int port, int number,
	base::s_int64_t fd) {
	for (auto iter = _router_info_vec.begin(); iter != _router_info_vec.end();
		++iter) {
		if (ip == iter->ip
			&& port == iter->port
			&& number == iter->number) {
			iter->fd = fd;
			return 0;
		}
	}
	return -1;
}

int RouterMgr::DelRouter(const std::string& ip, unsigned int port, int number,
	base::s_int64_t fd) {
	for (auto iter = _router_info_vec.begin(); iter != _router_info_vec.end();
		++iter) {
		if (ip == iter->ip
			&& port == iter->port
			&& number == iter->number) {
			_router_info_vec.erase(iter);
			return 0;
		}
	}
	return -1;
}

int RouterMgr::SendMsg(int cmd, base::s_int64_t userid, bool is_broadcast,
	base::s_uint32_t src_svr_type, base::s_uint32_t dst_svr_type,
	base::s_uint32_t src_inst_id, base::s_uint32_t dst_inst_id,
	base::s_uint32_t src_trans_id, base::s_uint32_t dst_trans_id,
	google::protobuf::Message& msg) {
	// ��ѡһ��·��
	if (_router_info_vec.empty()) {
		LogError("no router to send");
		return -1;
	}
	int r = userid % _router_info_vec.size();
	return SendMsgByFd(_router_info_vec[r].fd,
		cmd, userid, is_broadcast,
		src_svr_type, dst_svr_type,
		src_inst_id, dst_inst_id,
		src_trans_id, dst_trans_id,
		msg);
}

int RouterMgr::SendMsgByFd(base::s_int64_t fd, int cmd, base::s_int64_t userid,
	bool is_broadcast, base::s_uint32_t src_svr_type, base::s_uint32_t dst_svr_type,
	base::s_uint32_t src_inst_id, base::s_uint32_t dst_inst_id,
	base::s_uint32_t src_trans_id, base::s_uint32_t dst_trans_id,
	google::protobuf::Message& msg) {
	AppHeadFrame frame;
	frame.set_is_broadcast(is_broadcast);
	frame.set_src_svr_type(src_svr_type);
	frame.set_dst_svr_type(dst_svr_type);
	frame.set_src_inst_id(src_inst_id);
	frame.set_dst_inst_id(dst_inst_id);
	frame.set_src_trans_id(src_trans_id);
	frame.set_dst_trans_id(dst_trans_id);
	frame.set_cmd(cmd);
	frame.set_userid(userid);

	std::string data = msg.SerializePartialAsString();
	frame.set_cmd_length(data.length());

	base::Buffer buffer;
	buffer.Write(frame);
	buffer.Write(data.c_str(), data.length());
	if (NetIoHandlerSgl.SendDataByFd(fd, buffer.Data(), buffer.Length())) {
		return 0;
	}
	else {
		return -1;
	}
}