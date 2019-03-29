#ifndef M_REDIS_POOL_INCLUDE
#define M_REDIS_POOL_INCLUDE

#include "redis_connection.hpp"
#include "mutex.hpp"

// redis���ӳ�
class RedisPool {
public:
	// ÿ���̶߳�����һ������,��ñ���߳�ʹ��,���򲻱�֤�̰߳�ȫ
	static RedisConnection GetConnection(const std::string& ip, 
		unsigned short port);

	// ÿ���̶߳�����һ������,��ñ���߳�ʹ��,���򲻱�֤�̰߳�ȫ
	static RedisConnection GetConnection(const std::string& ip,
		unsigned short port, 
		unsigned short database);

	static RedisConnection GetConnection(const std::string& ip,
		unsigned short port,
		unsigned short database, 
		const std::string& user, 
		const std::string passwd);

	// �ͷ�����
	static void ReleaseConnection(RedisConnection& con);

private:
	static void _releaseConnection(shard_ptr_t<_rediscontext_> context);

	static bool _selectdb(redisContext* context, unsigned short db);

	static void _auth(redisContext* context, const char* user, const char* passwd);

	static void _freeRedisContext(redisContext* context);
};
#endif