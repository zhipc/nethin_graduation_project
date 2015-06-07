#include "global.hpp"

#include "procedure.hpp"

#include "tools/configure.hpp"
#include "tools/mysql_conn.hpp"
#include "tools/dictionary_tree.hpp"

extern mysql_conn* g_gpro_conn;

extern zlog_category_t* g_dtree_log;
extern zlog_category_t* g_mysql_log;
extern zlog_category_t* g_thread_log;
extern zlog_category_t* g_bussiz_log;

using namespace std;

void finish(int sig)
{
	dzlog_info("get signal: %d exit", sig);
	exit(0);
}

void init_log()
{
	configure& c = configure::instance();
	
	dzlog_info("change zlog configure:" CSL_PRT_PR(CSL_HIL CSL_BLG, "%s"), c["log_conf"].c_str());

	int ret = 0;
	
	if(!(ret = zlog_reload(c["log_conf"].c_str())))
	{
		dzlog_info("zlog_init failed, return val: %d", ret);
	}

	typedef pair<zlog_category_t*&, const char*> category_name;

	vector<category_name*> category_names;

	category_name dtree(g_dtree_log, LOG_DTREE);
	category_name mysql(g_mysql_log, LOG_MYSQL);
	category_name thread(g_thread_log, LOG_THREAD);
	category_name bussiz(g_bussiz_log, LOG_NETAPI);

	category_names.push_back(&dtree);
	category_names.push_back(&mysql);
	category_names.push_back(&thread);
	category_names.push_back(&bussiz);

	dzlog_set_category("dzlog");
	
	for(size_t i = 0; i < category_names.size(); i++)
	{
		category_name& c_n = *(category_names[i]);
		if(!(c_n.first = zlog_get_category(c_n.second)))
		{
			dzlog_info("zlog_get_category " CSL_PRT_PR(CSL_HIL CSL_RED, "%s") " failed, return val: %d", c_n.second, ret);
			exit(-1);
		}

		dzlog_info("zlog_get_category " CSL_PRT_PR(CSL_HIL CSL_BLU, "%s") " success, address: %p", c_n.second, c_n.first);
	}

	dzlog_info("change zlog configure success, we use zlog now. check the configure file: " CSL_PRT_PR(CSL_HIL CSL_BLU, "%s") " to get more detail.", c["log_conf"].c_str());
}

void init_mysql()
{
	configure& c = configure::instance();
	dzlog_info("start link to mysql");

	string url = boost::str(boost::format("%s:%s") %  c["mysql_host"] % c["mysql_port"]);

	g_gpro_conn = mysql_conn::get_mysql_conn(url, c["mysql_user"], c["mysql_pass"], GPRO_DB);

	if(!g_gpro_conn)
	{
		dzlog_error("link to mysql success failed");
		exit(-1);
	}
	
	dzlog_info("link to mysql success");
}

void init_dict()
{
	dictionary_tree& dic = dictionary_tree::instance();
	
	string sql = LOAD_KEY_MAP_SQL;

	sql::ResultSet* rs = NULL;

	dzlog_info("start to query key_map");

	if(!g_gpro_conn->QuerySql(sql, rs))
	{
		dzlog_error("query key_map failed: query sql failed");
		exit(-1);
	}

	if(!rs)
	{
		dzlog_error("query key_map failed: rs is null");
		exit(-1);
	}

	while(rs->next())
	{
		dic.insert_node(rs->getString("key_word"), rs->getUInt("id"));
	}

	delete rs;

	dzlog_info("load key_map finished, load %u keys", dic.get_node_num());
}

void init_sig()
{
	dzlog_info("ignore sigpipe");

	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, finish);
	signal(SIGTERM, finish);
	
	dzlog_info("ignore sigpipe done");
}


void init()
{
	init_sig();
	init_log();
	init_mysql();
	init_dict();
}

void run()
{
	event_base *base;
	evhttp *http;
	evhttp_bound_socket *handle;

	base = event_base_new();

	dzlog_info("create event_base");
	
	if (!base)
	{
		dzlog_error("couldn't create an event_base: exiting");
		exit(-1);
	}
	
	dzlog_info("create event_base done");

	http = evhttp_new(base);

	dzlog_info("create event_http");
	
	if (!http)
	{
		dzlog_error("couldn't create evhttp. exiting.");
		exit(-1);
	}

	dzlog_info("create event_http done");
	configure& c = configure::instance();

	string http_base = c["http_base"];
	

	dzlog_info("bind search_cb");
	string search_base = http_base + "/search";
	evhttp_set_cb(http, search_base.c_str(), search_cb, NULL);

	dzlog_info("bind suggest_cb");
	string suggest_base = http_base + "/suggest";
	evhttp_set_cb(http, suggest_base.c_str(), suggest_cb, NULL);

	dzlog_info("bind any_cb");
	evhttp_set_gencb(http, any_cb, NULL);



	const char* addr = c["http_addr"].c_str();
	const uint16_t port = boost::lexical_cast<uint16_t>(c["http_port"]);

	dzlog_info("bind socket: %s:%u", addr, port);
	
	handle = evhttp_bind_socket_with_handle(http, addr, port);
	
	if (!handle)
	{
		dzlog_error("couldn't bind to  %s:%d. exiting.", addr, port);
		exit(-1);
	}

	dzlog_info("bind socket success");

        {
		struct sockaddr_storage ss;
		evutil_socket_t fd;
		ev_socklen_t socklen = sizeof(ss);
		char addrbuf[128];
		void *inaddr;
		const char *addr;
		int got_port = -1;
		
		fd = evhttp_bound_socket_get_fd(handle);
		
		memset(&ss, 0, sizeof(ss));
		if (getsockname(fd, (struct sockaddr *)&ss, &socklen))
		{
			dzlog_error("getsockname() failed");
			exit(-1);
		}
		if (ss.ss_family == AF_INET)
		{
			got_port = ntohs(((struct sockaddr_in*)&ss)->sin_port);
			inaddr = &((struct sockaddr_in*)&ss)->sin_addr;
		}
		else if (ss.ss_family == AF_INET6)
		{
			got_port = ntohs(((struct sockaddr_in6*)&ss)->sin6_port);
			inaddr = &((struct sockaddr_in6*)&ss)->sin6_addr;
		}
		else
		{
			dzlog_error("Weird address family %d", ss.ss_family);
			exit(-1);
		}
		
		addr = evutil_inet_ntop(ss.ss_family, inaddr, addrbuf, sizeof(addrbuf));
		
		if (addr)
		{
			dzlog_info("listening on %s:%d", addr, got_port);
		}
		else
		{
			dzlog_error("evutil_inet_ntop failed");
			exit(-1);
		}
	}
	
	dzlog_info("dispatch server");
	
	event_base_dispatch(base);
}

int main(int argc, char** argv)
{

	namespace po = boost::program_options;
	
	try
	{
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("conf,c", po::value<string>(), "set configure file")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help"))
		{
			cout << desc << endl;
			return 0;
		}

		configure& c = configure::instance();
		if (vm.count("conf"))
		{
			string conf_file = vm["conf"].as<string>().c_str();
			c.parse(conf_file.c_str());
		}
		
		dzlog_info("configure:");
		c.print(cout);

		init();

		run();
	}
	catch(exception& e)
	{
		cerr << "error: " << e.what() << endl;;
		return 1;
	}
	catch(...)
	{
		cerr << "Exception of unknown type!" << endl;
	}
	return 0;
} 

// magic init
MAGIC_INIT_BEGIN(global)
{
	using namespace boost;
	cout << "change to stdio dzlog: ";

	int ret = dzlog_init(NULL, "stdout");

	if(ret != 0)
	{
		cout << CSL_PRT_PR(CSL_RED, "FAIL") << endl;
		cout << CSL_PRT(CSL_RED, "ERROR") ":" << endl
		     << strerror(ret) << endl;
		exit(ret);
	}
	else
	{
		cout << CSL_PRT_PR(CSL_GRN, "SUCCESS") << endl;
	}

	configure::instance();
	dictionary_tree::instance();
}
MAGIC_INIT_END(global)

// magic finish
MAGIC_FINI_BEGIN(global)
{
	using namespace boost;
	dzlog_info("global fini");

	configure::destory();
	dictionary_tree::destory();

	dzlog_info("start to delete g_gpro_conn");
	
	if(NULL != g_gpro_conn)
	{
		delete g_gpro_conn;
	}
	else
	{
		dzlog_error("g_gpro_conn is null");
	}
	
	dzlog_info("delete g_gpro_conn end");

	dzlog_info("flush logs");
	zlog_fini();
}
MAGIC_FINI_END(global)

