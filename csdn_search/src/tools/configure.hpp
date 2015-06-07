#ifndef __CONFIGURE_HPP__
#define __CONFIGURE_HPP__

#include "rwlock.hpp"
#include "../global.hpp"

class configure
{
public:
	static configure& instance()
	{
		if(m_instance_s == NULL)
		{
			m_instance_s = new configure();
		}
		return (*m_instance_s);
	}
	
	static void destory()
	{
		if(m_instance_s != NULL)
		{
			delete m_instance_s;
			m_instance_s = NULL;
		}
		else
		{
			dzlog_error("distory configure failed: instance is null");
		}
	}

	const std::string& operator[] (const std::string& key)
	{
		return this->get(key);
	}

	const bool parse(const char* file = DEF_CONF);

	std::ostream& print(std::ostream& os, bool wrap = true) const;
	
	const std::string& get(const std::string& key);

private:
	configure();
	~configure();
	configure(configure& c):
		m_parsed(),
		m_rwlock(""),
		m_config()
		{}

	void set_defualt_configure();

	void set(const std::string& key, const std::string& val);

	bool m_parsed;

	rwlock m_rwlock;

	std::map<std::string, std::string> m_config;
	
	static std::string m_configure_null_s;
	
	static configure* m_instance_s;
};

#endif // __CONFIGURE_HPP__
