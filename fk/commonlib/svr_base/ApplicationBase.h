#ifndef M_SVR_BASE_APPLICATION_INCLUDE
#define M_SVR_BASE_APPLICATION_INCLUDE

#include <string>
#include <unordered_map>
#include "slience/base/timer.hpp"
#include "protolib/src/comm_conf.pb.h"
#include "protolib/src/svr_base.pb.h"
#include "commonlib/svr_base/svrbase.h"
#include "commonlib/svr_base/server_cfg.h"

class ApplicationBase {
public:
	ApplicationBase();

	virtual ~ApplicationBase();

	int Init(int argc, char** argv);

	int Run();

	const std::string& ConfigFilePath()const;

	const std::string& AppName()const;

	const std::string& PidFile()const;

	const base::timestamp& GetNow()const;

protected:
	virtual int OnInit();

	virtual int OnTick(const base::timestamp& now);

	virtual int OnReload();

	virtual int ServerType() = 0;

	virtual int InstanceId() = 0;

	virtual int OnInitNetWork() = 0;

	virtual void OnStopNetWork() = 0;

	virtual int UpdateNetWork() = 0;

	virtual int OnProc(base::s_int64_t fd, const AppHeadFrame& frame, const char* data, base::s_uint32_t data_len);

	virtual int OnExit();

protected:
	int ParseOpt(int argc, char** argv);

	void Usage()const;

	bool CheckReload();

protected:
	ServerCfg<config::CommConf> _comm_config;
	std::string _workdir;
	std::string _appname;
	std::string _pid_file;
	std::string _conf_file;
	std::string _comm_conf_file;
	std::string _log_file;
	int _log_level;
	int _log_withpid;
	int _daemon;
	int _svr_thread_cnt;

	int _msg_cache_size;
	base::timestamp _now;

	// application state
	static bool _app_exit;
};

#endif