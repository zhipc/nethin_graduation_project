#ifndef __MYSQL_CONN_HPP__
#define __MYSQL_CONN_HPP__

#include "../global.hpp"

class mysql_conn
{
public:
	static mysql_conn* get_mysql_conn(const std::string& host, const std::string& user,
			      const std::string& passwd, const std::string& db);
	
	~mysql_conn();

	const bool reconnect();

	const bool QuerySql(const std::string& sql);
	
	const bool QuerySql(const std::string& sql, sql::ResultSet*& rs);

private:
	mysql_conn(const std::string& host, const std::string& user,
		   const std::string& passwd, const std::string& db);

	const bool connect();
	
	mysql_conn(mysql_conn&);

	std::string m_db;
	std::string m_host;
	std::string m_user;
	std::string m_passwd;

	sql::Connection* m_con;
};

#endif//__MYSQL_CONN_HPP__
