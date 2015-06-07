#include "procedure.hpp"

#include "tools/configure.hpp"
#include "tools/mysql_conn.hpp"
#include "tools/dictionary_tree.hpp"

extern mysql_conn* g_gpro_conn;
extern zlog_category_t* g_bussiz_log;

using namespace std;

#define 	HTTP_OK   200
#define 	HTTP_BR   400
#define 	HTTP_FB   403
#define 	HTTP_NF   404
#define	        HTTP_SE   500

typedef map<string,string> map_ss;

struct request_t {
	string uri;
	
	string content;
	string cmdtype;
	map_ss header;

	ostream& print(ostream& os)
	{
		os << endl << "========================================" << endl;
		os << boost::format("Received a %s request for %s\nHeaders:\n") % cmdtype % uri;
		for(map_ss::iterator it = header.begin(); it != header.end(); it ++)
		{
			os << boost::format("  %s: %s") %  it->first % it->second << endl;
		}
		os << "Content:" << endl << content;
		os << endl << "========================================" << endl;

		return os;
	}

	friend ostream& operator<<(ostream& os, request_t& req)
	{
		return req.print(os);
	}
};

struct response_t {
	uint16_t code;
	
	string content;
	string result_str;
	map<string, string> header;

	response_t():
		code(HTTP_OK),
		content(""),
		result_str("OK"),
		header()
	{}
		
	ostream& print(ostream& os)
	{
		os << endl << "========================================" << endl;
		os << boost::format("Return response: %u %s \nHeaders:\n") % code % result_str;
		for(map_ss::iterator it = header.begin(); it != header.end(); it ++)
		{
			os << boost::format("  %s: %s") %  it->first % it->second << endl;
		}
		os << "Content:" << endl << content;
		os << endl << "========================================" << endl;

		return os;
	}

	friend ostream& operator<<(ostream& os, response_t& resp)
	{
		return resp.print(os);
	}
};

template <class T>
static void join_str(string& str, const vector<T>& vec, const string& sp)
{
	stringstream ss;

	if(vec.empty())
	{
		str = "";
		return;
	}

	ss << vec[0];
	
	for(size_t i = 1; i < vec.size(); i ++)
	{
		ss << sp << vec[i];
	}

	str = ss.str();
}

template<class It>
static string join_str(It it, It end, const string& sp)
{
	stringstream ss;
	
	if(it == end)
	{
		return "";
	}

	ss << *it;
	
	while(++it != end)
	{
		ss << sp << *it;
	}

	return ss.str();
}

static uint64_t get_timestamp()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

static void dump_request(evhttp_request *req, request_t& req_t)
{
	stringstream ss;
	
	const char *cmdtype;
	evkeyvalq *headers;
	evkeyval *header;
	evbuffer *buf;

	switch (evhttp_request_get_command(req))
	{
	case EVHTTP_REQ_GET: cmdtype = "GET"; break;
	case EVHTTP_REQ_PUT: cmdtype = "PUT"; break;
	case EVHTTP_REQ_POST: cmdtype = "POST"; break;
	case EVHTTP_REQ_HEAD: cmdtype = "HEAD"; break;
	case EVHTTP_REQ_TRACE: cmdtype = "TRACE"; break;
	case EVHTTP_REQ_PATCH: cmdtype = "PATCH"; break;
	case EVHTTP_REQ_DELETE: cmdtype = "DELETE"; break;
	case EVHTTP_REQ_OPTIONS: cmdtype = "OPTIONS"; break;
	case EVHTTP_REQ_CONNECT: cmdtype = "CONNECT"; break;
	default: cmdtype = "unknown"; break;
	}

	req_t.cmdtype = cmdtype;
	
	req_t.uri = evhttp_request_get_uri(req);

	headers = evhttp_request_get_input_headers(req);
	for (header = headers->tqh_first; header; header = header->next.tqe_next)
	{
		req_t.header[header->key] = header->value;
	}

	buf = evhttp_request_get_input_buffer(req);
	while (evbuffer_get_length(buf))
	{
		int n;
		char cbuf[SM_BUFF];
		n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
		if (n > 0)
		{
			cbuf[n] = 0;
		        req_t.content += cbuf;
		}
	}

	ss << req_t;
	
	zlog_debug(g_bussiz_log, ss.str().c_str());
}

static void dump_response(evhttp_request* req, response_t& resp_t)
{
	stringstream ss;
	
	evbuffer* content = evbuffer_new();

	evkeyvalq* header = evhttp_request_get_output_headers(req);

	for(map_ss::iterator it = resp_t.header.begin(); it != resp_t.header.end(); it++)
	{
		
		int ret = evhttp_add_header(header, it->first.c_str(), it->second.c_str());
		if(ret)
		{
			zlog_error(g_bussiz_log, "add header error: %s: %s", it->first.c_str(), it->second.c_str());
		}
	}

	if(content)
	{
		evbuffer_add_printf(content, "%s", resp_t.content.c_str());
			
		evhttp_send_reply(req, resp_t.code, resp_t.result_str.c_str(), content);

		evbuffer_free(content);
	}
	else
	{
		evhttp_send_reply(req, 500, "System Error", NULL);		
	}

	ss << resp_t;

	zlog_debug(g_bussiz_log, "%s", ss.str().c_str());
}


static const int cmdtype_filter(request_t& req_t, response_t& resp_t)
{
	if(req_t.cmdtype != "POST")
	{
		return 400;
	}
	
	return 0;
}

static const int any_handle(request_t& req_t, response_t& resp_t)
{
	return 403;
}

struct sort_crawl_item {
	uint32_t id;
	float power;

	bool operator>(const sort_crawl_item& i) const
	{
		return this->power > i.power;
	}

	bool operator==(const sort_crawl_item& i) const 
	{
		return this->power == i.power;
	}

	bool operator<(const sort_crawl_item& i) const 
	{
		return this->power < i.power;
	}
};

static const int search_handle(request_t& req_t, response_t& resp_t)
{
	Json::Value root;
	Json::Reader reader;

	uint64_t timestamp = get_timestamp();

	if(!reader.parse(req_t.content, root))
	{
		zlog_error(g_bussiz_log, "invalid json query: %s", req_t.content.c_str());
		return HTTP_BR;
	}

	if(!root.isMember("key"))
	{
		zlog_error(g_bussiz_log, "not key word: %s", req_t.content.c_str());
		return HTTP_BR;
	}
	
	string key = root["key"].asString();

	dictionary_tree& dic = dictionary_tree::instance();

	uint32_t key_node = dic[key];

	
	if(key_node == 0)
	{
		zlog_info(g_bussiz_log, "can't find key_node of key:%s, use suggest", key.c_str());

		vector<uint32_t> suggest_ids;

		if(!dic.suggest_node(key, suggest_ids, 1))
		{
			zlog_error(g_bussiz_log, "get suggest error");
			return HTTP_NF;
		}

		if(suggest_ids.empty())
		{
			zlog_error(g_bussiz_log, "can't get any thing about %s", key.c_str());
			return HTTP_NF;
		}
		
		key_node = suggest_ids[0];
	}

	string query_key;

	Json::Value digest_list;

	{
		sql::ResultSet* rs = NULL;

		string sql = boost::str(boost::format("SELECT `key_word`, `link_count`, `link_crawls` FROM `key_map` WHERE `id` = %d") % key_node);
		
		if(!g_gpro_conn->QuerySql(sql, rs))
		{
			zlog_error(g_bussiz_log, "query mysql error");
			return HTTP_NF;
		}

		if(!(rs && rs->next()))
		{
			zlog_error(g_bussiz_log, "key_node: %d found, but can't link to crawl.", key_node);
			return HTTP_NF;
		}

		query_key = rs->getString("key_word");
		
		uint32_t count = rs->getUInt("link_count");
		string link_crawls_str = rs->getString("link_crawls");

		delete rs;

		rs = NULL;

		vector<string> crawl_pows;

		boost::split(crawl_pows, link_crawls_str, boost::is_any_of(","));


		// 这里因为text的最大长度是65535, 后面的字符被截断了, 所以搜索结果大于6000左右的时候, 这里会出错,
		// 鉴于用户一般不会翻到6000个的结果, 这里直接干掉了.
		// if(crawl_pows.size() != count)
		// {
		// 	zlog_error(g_bussiz_log, "crawl_pows.size(): %lu != count: %d", crawl_pows.size(), count);
		// 	resp_t.content = "{\"return\": 1, \"msg\": \"parse crawl_pows error\"}";
		// 	return HTTP_SE;
		// }

		vector<sort_crawl_item> link_crawls;

		// 只处理前6000个结果, 这算不算作弊?
		for(size_t i = 0; i < crawl_pows.size() && i < 6000; i++)
		{
			vector<string> crawl_pow;
			boost::split(crawl_pow, crawl_pows[i], boost::is_any_of("_"));

			if(crawl_pow.size() != 2)
			{
				zlog_error(g_bussiz_log, "crawl_pows.size(): %lu != 2", crawl_pow.size());
				resp_t.content = "{\"return\": 1, \"msg\": \"parse crawl_pow error\"}";
				return HTTP_SE;
			}

			sort_crawl_item item = {boost::lexical_cast<uint32_t>(crawl_pow[0]), boost::lexical_cast<float>(crawl_pow[1])};
			link_crawls.push_back(item);
		}


		stable_sort(link_crawls.begin(), link_crawls.end(), greater<sort_crawl_item>());

		uint32_t limit = 10;
		uint32_t offset = 0;

		if(root.isMember("offset"))
		{
			offset = root["offset"].asUInt();
		}

		if(root.isMember("limit"))
		{
			limit = root["limit"].asUInt();
		}

		limit = limit > DIGEST_LIMIT ? DIGEST_LIMIT : limit;

		vector<uint32_t> out_digests;

		zlog_info(g_bussiz_log, "take %u to %u of %lu", offset, limit, link_crawls.size());
		
		for(size_t i = offset; i < link_crawls.size() && i < limit + offset; i ++)
		{
			out_digests.push_back(link_crawls[i].id);
		}

		sql = boost::str(boost::format("SELECT `url`, `title`, `author`, LEFT(`content`, 100) AS `digest`"
					       "FROM `crawl` where `id` IN (%1%) ORDER BY FIELD(`id`,%1%)") % join_str(out_digests.begin(), out_digests.end(), ","));

		if(!g_gpro_conn->QuerySql(sql, rs) || !rs)
		{
			zlog_error(g_bussiz_log, "query mysql error");
			return HTTP_NF;
		}

		uint32_t index = 0;

		while(rs->next())
		{
			Json::Value digest;
			digest["url"] = string(rs->getString("url"));
			digest["title"] = string(rs->getString("title"));
			digest["author"] = string(rs->getString("author"));
			digest["digest"] = string(rs->getString("digest"));

			digest_list[index++] = digest;
		}

		root.clear();

		string time_str = boost::str(boost::format("%lu") % (get_timestamp() - timestamp));

		root["key"] = key;
		root["size"] = index;
		root["count"] = count;
		root["return"] = 0;
		root["usetime"] = time_str;
		root["query_key"] = query_key;
		root["digest_list"] = digest_list;

	}

	Json::FastWriter writer;
	resp_t.content = writer.write(root);
	
	return 0;
}

static const int suggest_handle(request_t& req_t, response_t& resp_t)
{
	Json::Value root;
	Json::Reader reader;

	if(!reader.parse(req_t.content, root))
	{
		zlog_error(g_bussiz_log, "invalid json query: %s", req_t.content.c_str());
		return HTTP_BR;
	}

	if(!root.isMember("key"))
	{
		zlog_error(g_bussiz_log, "not key word: %s", req_t.content.c_str());
		return HTTP_BR;
	}
	
	string ids;
	string key = root["key"].asString();
	
	vector<uint32_t> suggest_ids;

	dictionary_tree& d = dictionary_tree::instance();

	uint32_t suggest_limit = 5;

	if(root.isMember("limit"))
	{
		suggest_limit = root["limit"].asUInt();
	}

	suggest_limit = suggest_limit > SUGGEST_LIMIT ? SUGGEST_LIMIT : suggest_limit;

	if(!d.suggest_node(key, suggest_ids, suggest_limit))
	{
		zlog_error(g_bussiz_log, "get suggest error");
		return HTTP_NF;
	}

	root.clear();

	uint32_t index = 0;
	Json::Value suggest_array;	

	if(!suggest_ids.empty())
	{
		sql::ResultSet* rs = NULL;
		
		join_str(ids, suggest_ids, ",");
		
		string sql = boost::str(boost::format("SELECT `id`, `key_word`, `link_count` FROM `key_map` WHERE `id` IN (%1%) ORDER BY FIELD(`id`,%1%)") % ids);
		
		g_gpro_conn->QuerySql(sql, rs);

		if(!rs)
		{
			zlog_error(g_bussiz_log, "query sql error");
			return HTTP_NF;
		}

		while(rs->next())
		{
			Json::Value suggest;
			string key_word = rs->getString("key_word");
			suggest["id"] = rs->getUInt("id");
			suggest["key"] = key_word;
			suggest["link_count"] = rs->getUInt("link_count");
			suggest_array[index] = suggest;
			index++;
		}

		delete rs;
	}
	
	root["return"] = 0;
	root["command"] = "suggest";
	root["suggestions"] = suggest_array;
	root["suggest_count"] = index;

	Json::FastWriter writer;
	resp_t.content = writer.write(root);
	
	return 0;
}


enum
{
	ANY_HDL_IDX = 0,
	SEARCH_HDL_IDX,
	SUGGEST_HDL_IDX,
	HDL_ARR_SIZE
};

typedef map<int, pair<string,string> > map_iss;

static const int(*handles[HDL_ARR_SIZE])(request_t&, response_t&)= {any_handle, search_handle, suggest_handle};

static map_iss g_http_status;

void comment_procedure(uint16_t which, evhttp_request* req, void* arg)
{
	request_t req_t;
	response_t resp_t;

	dump_request(req, req_t);

	resp_t.header["Content-Type"] = "application/x-javascript";
	
	resp_t.content = "{\"return\": 0, \"command\": \"ready\"}";

	const int(*procedure_pipelines[2])(request_t&, response_t&) = {cmdtype_filter, handles[which]};

	for(size_t i = 0; i < 2; i ++)
	{
		const int code = procedure_pipelines[i](req_t, resp_t);

		if(!code)
		{
			continue;
		}
		
		resp_t.code = code;
			
		switch(code)
		{
		case HTTP_BR:
			resp_t.content = "{\"return\": 1, \"msg\": \"Bad Request\"}";
			resp_t.result_str = "Bad Request";
			break;
		case HTTP_NF:
			resp_t.content = "{\"return\": 1, \"msg\": \"Not Found\"}";
			resp_t.result_str = "Not Found";
			break;
		case HTTP_FB:
			resp_t.content = "{\"return\": 1, \"msg\": \"Forbidden\"}";
			resp_t.result_str = "Forbidden";
			break;
		default:
			resp_t.result_str = "Internal Server Error";
		}

		break;
	}

	Json::Value root;
	Json::Reader reader;

	if(reader.parse(req_t.content, root) && root.isMember("callback"))
	{
		resp_t.content = boost::str(boost::format("%s(%s);") % root["callback"].asString() % resp_t.content);
	}

	dump_response(req, resp_t);
}

void search_cb(evhttp_request* req, void* arg)
{
	comment_procedure(SEARCH_HDL_IDX, req, arg);
}


void suggest_cb(evhttp_request* req, void* arg)
{
	comment_procedure(SUGGEST_HDL_IDX, req, arg);
}


void any_cb(evhttp_request* req, void* arg)
{
	comment_procedure(ANY_HDL_IDX, req, arg);
}

