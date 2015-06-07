#include "rwlock.hpp"

extern zlog_category_t* g_thread_log;

rwlock::rwlock(const std::string& name):
	m_name(),
	m_lock()
{
	snprintf(m_name, SM_BUFF, "%s", name.c_str());
	int ret = 0;
	if((ret = pthread_rwlock_init(&m_lock, NULL)))
	{
		exit(ret);
	}
}

rwlock::~rwlock()
{
	pthread_rwlock_destroy(&m_lock);
}

const bool rwlock::rdlock()
{
	return 0 == pthread_rwlock_rdlock(&m_lock);
}

const bool rwlock::wrlock()
{
	return 0 == pthread_rwlock_wrlock(&m_lock);
}

const bool rwlock::unlock()
{
	return 0 == pthread_rwlock_unlock(&m_lock);
}

const bool rwlock::tryrdlock()
{
	return 0 == pthread_rwlock_tryrdlock(&m_lock);
}

const bool rwlock::trywrlock()
{
	return 0 == pthread_rwlock_trywrlock(&m_lock);
}



