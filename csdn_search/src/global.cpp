#include "global.hpp"

// ==================== variables ====================

// zlog

zlog_category_t* g_mysql_log  = NULL;
zlog_category_t* g_dtree_log  = NULL;
zlog_category_t* g_thread_log = NULL;
zlog_category_t* g_bussiz_log = NULL;

#include "tools/mysql_conn.hpp"
// mysql_conn
mysql_conn* g_gpro_conn = NULL;
