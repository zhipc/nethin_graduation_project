#include <fstream>
#include <sstream>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include "configure.hpp"

using namespace std;
using namespace boost;

string configure::m_configure_null_s = "";
configure* configure::m_instance_s = NULL;


configure::configure():
	m_parsed(false),
	m_rwlock("configure"),
	m_config()
{
	dzlog_info("start init configure");
	
	set_defualt_configure();
}

configure::~configure()
{
	dzlog_info("start destory configure");
	m_config.clear();
	dzlog_info("destory configure: " CSL_PRT_PR(CSL_GRN,"SUCCESS"));
}

const string& configure::get(const string& key)
{
	m_rwlock.rdlock();
	map<string,string >::const_iterator it = m_config.find(key);
	m_rwlock.unlock();
	return (*it).second;
}

void configure::set(const string& key, const string& val)
{
	m_rwlock.wrlock();
	m_config[key] = val;
	m_rwlock.unlock();
}

ostream& configure::print(ostream& os, bool wrap) const
{
	if(wrap)
	{
		os << "==================== " CSL_YLW "configure" CSL_CLR " ====================" << endl;
	}
	
	for(map<string, string>::const_iterator it = m_config.begin(); it != m_config.end(); it ++)
	{
		os << format(CSL_YLW "%18s" CSL_HIL CSL_BLU " = " CSL_GRN "%-40s" CSL_CLR) % (*it).first % (*it).second << endl;
	}
	
	if(wrap)
	{
		os << "==================== " CSL_YLW "configure" CSL_CLR " ====================" << endl;
	}
	
	return os;
}

void configure::set_defualt_configure()
{
	m_rwlock.rdlock();
	configure_default(m_config);
	m_rwlock.unlock();
}

const bool configure::parse(const char* file)
{
	stringstream ss;
	
	set("parse", "fail");

	dzlog_info("parse configure file: " CSL_PRT_PR(CSL_HIL CSL_BLG, "%s"), file);
	
	if(m_parsed)
	{
		dzlog_warn(CSL_YLW "parse configure file failed: we have parse once, if you want to change it, please change your configure file and restart." CSL_CLR);
		
		ss << "configure:" << endl;
		
		print(ss);

		dzlog_info(ss.str().c_str());
		
		return false;
	}
	
	m_parsed = true;

	string line;
	
	ifstream in;
	
	in.open(file, ifstream::in);

	if(!in)
	{
		dzlog_warn( CSL_YLW "parse configure file failed: can't read file: " CSL_PRT_PR(CSL_HIL CSL_RED, "%s") CSL_YLW ". we will use defualt configure:" CSL_CLR, file);

		ss << "configure:" << endl;
		
		print(ss);

		dzlog_info(ss.str().c_str());
		
		return false;
	}

	
	cmatch what;

	size_t line_num = 0;

	regex configure_regex(configure_line_rule);

	m_rwlock.wrlock();
	try
	{
		while(getline(in, line))
		{
			line_num ++;

			trim(line);

			if(line.size() == 0)
			{
				continue;
			}
		
			if(!regex_match(line.c_str(), what, configure_regex))
			{
				dzlog_error( CSL_PRT_PR(CSL_HIL CSL_RED, "%s") " line: %lu: unkown line: " CSL_PRT_PR(CSL_HIL CSL_RED, "%s") ", ignore.", file, line_num, line.c_str());
				continue;
			}

			if(what.size() != 3)
			{
				dzlog_error( CSL_PRT_PR(CSL_HIL CSL_RED, "%s") " line: %lu: match line not complete: " CSL_PRT_PR(CSL_HIL CSL_RED, "%s") ", ignore.", file, line_num, line.c_str());
				continue;
			}

			set(what.str(1), what.str(2));
		}
	}
	catch(std::exception& e)
	{
		dzlog_error("parse configure file failed: %s", e.what());
		return false;
	}

	set("parse", "success");

	m_rwlock.unlock();
	return true;
}

