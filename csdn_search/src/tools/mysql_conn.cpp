#include "mysql_conn.hpp"

extern zlog_category_t* g_mysql_log;

using namespace std;
using namespace sql;
using namespace boost;

mysql_conn* mysql_conn::get_mysql_conn(const string& host, const string& user,
				  const string& pass, const string& db)
{
	mysql_conn* ret_con = new mysql_conn(host, user, pass, db);

	if(NULL == ret_con->m_con)
	{
		delete ret_con;
		return NULL;
	}
	
	return ret_con;
}

mysql_conn::mysql_conn(const string& host, const string& user,
		       const string& pass, const string& db):
	m_db(db),
	m_host(host),
	m_user(user),
	m_passwd(pass)
{
	connect();
}

mysql_conn::~mysql_conn()
{
	delete m_con;
}

const bool mysql_conn::connect()
{
	string link_str = str(format("host: %s, user: %s, pass: %s, db: %s") % m_host % m_user % m_passwd % m_db);

	if(NULL == g_mysql_log)
	{
		dzlog_error("g_mysql_log is null, can't init mysql.");
		return false;
	}
	
	try
	{
		zlog_info(g_mysql_log, "link to mysql %s", link_str.c_str());
		
		zlog_info(g_mysql_log, "start get driver");
		Driver* driver = get_driver_instance();
		zlog_info(g_mysql_log, "start get success");

		zlog_info(g_mysql_log, "start link to host: %s", m_host.c_str());
		m_con = driver->connect(m_host, m_user, m_passwd);
		zlog_info(g_mysql_log, "start link success");

		zlog_info(g_mysql_log, "start change to db: %s", m_db.c_str());
		m_con->setAutoCommit(1);
		m_con->setSchema(m_db);
		zlog_info(g_mysql_log, "start change success");
	}
	catch(SQLException& e)
	{
		zlog_error(g_mysql_log, "link mysql failed: %s", e.what());
		m_con = NULL;
		return false;
	}
	catch(std::exception& e)
	{
		zlog_error(g_mysql_log, "unknow exception: %s", e.what());
		m_con = NULL;
		return false;
	}

	zlog_info(g_mysql_log, "all things done");
	
	return true;
}

const bool mysql_conn::reconnect()
{
	delete m_con;
	m_con = NULL;
	return connect();
}

const bool mysql_conn::QuerySql(const string& sql)
{
	try
	{
		auto_ptr<Statement> stat(m_con->createStatement());
		zlog_info(g_mysql_log, "query sql: %s", sql.c_str());
		stat->execute(sql);
	}
	catch(SQLException& e)
	{
		zlog_error(g_mysql_log, "query mysql failed: %s", e.what());
		return false;
	}
	catch(std::exception& e)
	{
		zlog_error(g_mysql_log, "unknow exception: %s", e.what());
		return false;
	}

	zlog_info(g_mysql_log, "query sql success");
	
	return true;
}

const bool mysql_conn::QuerySql(const string& sql, ResultSet*& rs)
{
	try
	{
		auto_ptr<Statement> stat(m_con->createStatement());
		zlog_info(g_mysql_log, "query sql: %s", sql.c_str());
		rs = stat->executeQuery(sql);
	}
	catch(SQLException& e)
	{
		zlog_error(g_mysql_log, "query mysql failed: %s", e.what());
		return false;
	}
	catch(std::exception& e)
	{
		zlog_error(g_mysql_log, "unknow exception: %s", e.what());
		return false;
	}

	zlog_info(g_mysql_log, "query sql success");
	
	return true;
}
