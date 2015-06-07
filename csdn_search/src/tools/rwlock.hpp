#ifndef __RWLOCK_HPP__
#define __RWLOCK_HPP__

#include "../global.hpp"

class rwlock
{
public:
	rwlock(const std::string& name);
	~rwlock();

	const bool rdlock();

	const bool wrlock();

	const bool unlock();

	const bool tryrdlock();

	const bool trywrlock();

private:
	char m_name[SM_BUFF];
	pthread_rwlock_t m_lock;
};


#endif //__RWLOCK_HPP__
