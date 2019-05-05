#pragma once

/**
α�����������ͷ�ļ�����������ͬ���㷨
*/

#include "slience/base/config.hpp"
#include <ctime>

M_BASE_NAMESPACE_BEGIN

class random {
	typedef time_t SEED_TYPE;
public:

	random():m_is_rand_seed(true){}

	random(SEED_TYPE seed):m_seed(seed),m_is_rand_seed(false){}

	SEED_TYPE getSeed(){ return m_seed;}

	void setSeed(SEED_TYPE seed);

	/// ���һ�����������0��1֮��
	float rand();

	/// ���һ�����������0, max֮�䣬[0, max)
	int rand(int max); // [0, max)

	/// ���һ�����������min, max֮�䣬[min, max)
	int rand(int min, int max); // [min, max)

private:
	SEED_TYPE m_seed;
	bool      m_is_rand_seed;
};


M_BASE_NAMESPACE_END
