#include "mutex.hpp"

const bool mutex::lock()
{
	return 0 == pthread_mutex_lock(&m_lock);
}

const bool mutex::unlock()
{
	return 0 == pthread_mutex_unlock(&m_lock);
}

const bool mutex::trylock()
{
	return 0 == pthread_mutex_trylock(&m_lock);
}
