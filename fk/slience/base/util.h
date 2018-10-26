#pragma once
#include "slience/base/config.hpp"
#include <string>

M_BASE_NAMESPACE_BEGIN

time_t HmsToSecond(const std::string& hms);

// һ��Ĺ̶�ʱ��
time_t CertainTimesOfDay(time_t now, const std::string& hms);

// ��ӽ���ʱ���
time_t ClosestTimeOfDay(time_t now, time_t peroid);

M_BASE_NAMESPACE_END