#ifndef __MUTEX_HPP__
#define __MUTEX_HPP__

#include "../global.hpp"

class mutex
{
public:
	mutex(std::string& name):
		m_name(),
		m_lock()
	{
		int ret = 0;
		snprintf(m_name, MD_BUFF, "%s", name.c_str());
		if((ret = pthread_mutex_init(&m_lock, NULL)))
		{
			exit(ret);
		}
	}

	~mutex()
	{
		pthread_mutex_destroy(&m_lock);
	}

	const bool lock();

	const bool unlock();

	const bool trylock();

private:
	mutex(mutex&);
	char m_name[MD_BUFF];
	pthread_mutex_t m_lock;
};

#endif//__MUTEX_HPP__
