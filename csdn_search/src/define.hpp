#ifndef __DEFINE_HPP__
#define __DEFINE_HPP__

// ==================== console ====================

#define CSL_CLR "\033[0m"  // clear
#define CSL_HIL "\033[1m"  // high line
#define CSL_RED "\033[31m" // red
#define CSL_GRN "\033[32m" // green
#define CSL_YLW "\033[33m" // yellow
#define CSL_BLU "\033[34m" // blue
#define CSL_BLG "\033[44m" // blue backgroud

#define CSL_PRT(CLR, STR) CLR STR CSL_CLR
#define CSL_PRT_PR(CLR, STR) "[" CSL_PRT(CLR, STR) "]"

// ==================== macro ====================

// magic init
#define MAGIC_INIT_BEGIN(NAME) \
	struct _st_magic_init_##NAME \
	{ \
		_st_magic_init_##NAME() \
		{ \
			std::cout << "==================== " CSL_BLG "magic init "	\
				#NAME \
				": " \
				CSL_PRT(CSL_YLW, "START") \
				" ====================" << std::endl; 

#define MAGIC_INIT_END(NAME) \
	                std::cout << "====================  " CSL_BLG "magic init "	\
			#NAME \
			": " \
			CSL_PRT(CSL_YLW, "END")  \
			"  ====================" << std::endl;	\
		} \
	} st_init_magic_##NAME;		

// magic finish
#define MAGIC_FINI_BEGIN(NAME) \
	struct _st_magic_fini_##NAME \
	{ \
		~_st_magic_fini_##NAME() \
		{ \
			std::cout << "==================== " CSL_BLG "magic fini "	\
				#NAME \
				": " \
				CSL_PRT(CSL_YLW, "START") \
				" ====================" << std::endl; 

#define MAGIC_FINI_END(NAME) \
	                std::cout << "====================  " CSL_BLG "magic fini "	\
			#NAME \
			": " \
			CSL_PRT(CSL_YLW, "END")  \
			"  ====================" << std::endl;	\
		} \
	} st_fini_magic_##NAME;		

#define dzlog_info_format(FORMAT, ARGS...) dzlog_info(boost::str(FORMAT).c_str(), ##ARGS)
#define dzlog_warn_format(FORMAT, ARGS...) dzlog_warn(boost::str(FORMAT).c_str(), ##ARGS)
#define dzlog_debug_format(FORMAT, ARGS...) dzlog_debug(boost::str(FORMAT).c_str(), ##ARGS)
#define dzlog_error_format(FORMAT, ARGS...) dzlog_error(boost::str(FORMAT).c_str(), ##ARGS)
#define dzlog_fatal_format(FORMAT, ARGS...) dzlog_fatal(boost::str(FORMAT).c_str(), ##ARGS)
#define dzlog_notice_format(FORMAT, ARGS...) dzlog_notice(boost::str(FORMAT).c_str(), ##ARGS)

#define zlog_info_format(CAT, FORMAT, ARGS...) zlog_info(CAT, boost::str(FORMAT).c_str(), ##ARGS)
#define zlog_warn_format(CAT, FORMAT, ARGS...) zlog_warn(CAT, boost::str(FORMAT).c_str(), ##ARGS)
#define zlog_debug_format(CAT, FORMAT, ARGS...) zlog_debug(CAT, boost::str(FORMAT).c_str(), ##ARGS)
#define zlog_error_format(CAT, FORMAT, ARGS...) zlog_error(CAT, boost::str(FORMAT).c_str(), ##ARGS)
#define zlog_fatal_format(CAT, FORMAT, ARGS...) zlog_fatal(CAT, boost::str(FORMAT).c_str(), ##ARGS)
#define zlog_notice_format(CAT, FORMAT, ARGS...) zlog_notice(CAT, boost::str(FORMAT).c_str(), ##ARGS)

// ==================== variables ====================

#define LOG_CONF "conf/zlog.conf"
#define DEF_CONF "[internal]"

#define DEF_MYSQL_HOST "127.0.0.1"
#define DEF_MYSQL_USER "root"
#define DEF_MYSQL_PASS ""
#define DEF_MYSQL_CHST "utf8"
#define DEF_MYSQL_PORT "3306"

#define LOG_MYSQL  "mysql"
#define LOG_DTREE  "dtree"
#define LOG_THREAD "thread"
#define LOG_NETAPI "bussiz"

#define KB 1024
#define MB 1048576

#define SM_BUFF 512 
#define MD_BUFF KB * 4
#define BG_BUFF KB * 512
#define HG_BUFF MB * 8

#define GPRO_DB "gpro"

#define LOAD_KEY_MAP_SQL "SELECT `id`, `key_word` FROM `key_map` LIMIT 100";

// ==================== configure ====================

#define configure_default(config_map)			\
	config_map["log_conf"] = LOG_CONF;		\
	config_map["def_conf"] = DEF_CONF;		\
	config_map["mysql_host"] = DEF_MYSQL_HOST;	\
	config_map["mysql_port"] = DEF_MYSQL_PORT;	\
	config_map["mysql_user"] = DEF_MYSQL_USER;	\
	config_map["mysql_pass"] = DEF_MYSQL_PASS;	\
	config_map["mysql_chst"] = DEF_MYSQL_CHST;	\
	
#define configure_line_rule "([a-z0-9A-Z_]+)\\s*=\\s*([^\\s;]*);*"

#endif // __DEFINE_HPP__
