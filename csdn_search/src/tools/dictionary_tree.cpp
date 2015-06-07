#include "dictionary_tree.hpp"

using namespace std;

extern zlog_category_t* g_dtree_log;

dictionary_tree* dictionary_tree::m_dict_s = NULL;

static void _str2wstr_(const string& str,wstring& wstr)
{
	size_t str_len = str.length();

	char save_buf[SM_BUFF] = {0};

	snprintf(save_buf, SM_BUFF - 4, "%s", str.c_str());
	
	for(size_t i = 0; i < str_len;)
	{
		uint8_t* ptr = (uint8_t*)&(save_buf[i]);
		wchar_t wch = 0;
		
		switch((*ptr) & 0xF0)
		{
		case 0xF0:
			wch = (*((uint32_t*)(ptr)));
			i += 4;
			break;
		case 0xE0:
			wch = (*(uint32_t*)(ptr)) & 0x00FFFFFF;
			i += 3;
			break;
		case 0xC0:
			wch = (*(uint16_t*)(ptr)) & 0xFFFF;
			i += 2;
			break;
		default:
			wch = (*ptr) & 0xFF;
			i+= 1;
			break;
		}

		wstr += wch;
	}
}

static void _wstr2str_(const wstring& wstr, string& str)
{
	for(size_t i = 0; i< wstr.size(); i++)
	{
		wchar_t c = wstr[i];
		char* arr = (char*)(&c);
		
		switch(c & 0x000000F0)
		{
		case 0x000000F0: // B1111 ~
			str += arr[0];
			str += arr[1];
			str += arr[2];
			str += arr[3];
			break;
		case 0x000000E0: // B1110 ~
			str += arr[0];
			str += arr[1];
			str += arr[2];
			break;
		case 0x00000080: // B1100 ~
			str += arr[0];			
			str += arr[1];
			break;
		default:
			str += arr[0];

		}
	}
}

typedef map<wchar_t, s_dictionary_node>::iterator key_it;

const bool dictionary_tree::wstr2str(const wstring& wstr, string& str)
{
	
	str.clear();

	if(wstr.empty())
	{
		return true;
	}
		
	_wstr2str_(wstr, str);

	if(str.empty())
	{
		return false;
	}

	return true;
}

const bool dictionary_tree::str2wstr(const string& str, wstring& wstr)
{
	wstr.clear();

	if(str.empty())
	{
		return false;
	}

	_str2wstr_(str, wstr);

	if(wstr.empty())
	{
		return false;
	}

	return true;
}

s_dictionary_node* dictionary_tree::__search_node(const wstring& wkey, bool ignore)
{
	s_dictionary_node* current_node = &m_root;
	for(size_t i = 0; i < wkey.size(); i ++)
	{
		wchar_t c = wkey[i];
		
		map<wchar_t, s_dictionary_node>::iterator it = current_node->m_child_nodes.find(c);
		
		if(it == current_node->m_child_nodes.end())
		{
			if(ignore)
			{
				return current_node;
			}
			else
			{
				return NULL;
			}
		}
		
		current_node = &((*it).second);
	}
	return current_node;
}

s_dictionary_node* dictionary_tree::_search_node(const string& key, bool ignore)
{
	wstring wkey;
	
	if(!str2wstr(key, wkey))
	{
		zlog_error(g_dtree_log, "str2wstr error: key: %s", key.c_str());
		return NULL;
	}

	return __search_node(wkey, ignore);
}

ostream& inner_print(ostream& os, s_dictionary_node& node, uint32_t level)
{
	key_it end = node.m_child_nodes.end();
	key_it beg = node.m_child_nodes.begin();

	os << boost::format("%-3d") % level;
	
	for(size_t l = 0; l < level; l ++)
	{
		os << " | ";
	}

	os << boost::format(" 0x%08x->%d") % node.m_key % node.m_key_map_id << endl;

	if(beg == end)
	{
		return os;
	}

	while(beg != end)
	{
		inner_print(os, beg->second, level + 1);
		beg++;
	}
	
	return os;
}

ostream& dictionary_tree::print(ostream& os)
{
	os << "==================== "CSL_PRT_PR(CSL_HIL CSL_BLU, "dictionary_tree") " ====================" << endl;
	os << boost::format("we have %lu nodes") % m_node_n << endl;
	
	m_rwlock.rdlock();
	inner_print(os, m_root, 0);
	m_rwlock.unlock();
	
	os << "==================== "CSL_PRT_PR(CSL_HIL CSL_GRN, "dictionary_tree") " ====================" << endl;
	
	return os;
}

const bool dictionary_tree::insert_node(const string& key, const uint32_t& key_map_id)
{


	//zlog_info(g_dtree_log, "insert key: %s, id: %u", key.c_str(), key_map_id);
	
	wstring wkey;
	if(!str2wstr(key, wkey))
	{
		zlog_error(g_dtree_log, "str2wstr error: key: %s", key.c_str());
		return false;
	}
	
	s_dictionary_node* next_node = NULL;
	s_dictionary_node* current_node = &m_root;

	m_rwlock.rdlock();
	
	for(size_t i = 0; i < wkey.size(); i ++)
	{
		wchar_t c = wkey[i];
		next_node = &((*current_node)[c]);
		next_node->m_key = c;
		current_node = next_node;
	}

	m_node_n ++;

	current_node->m_key_map_id = key_map_id;

	m_rwlock.unlock();

	//zlog_info(g_dtree_log, "done");

	return true;
}

const bool dictionary_tree::delete_node(const string& key)
{

	zlog_info(g_dtree_log, "delete key: %s", key.c_str());
	
	m_rwlock.wrlock();

	s_dictionary_node* current_node = _search_node(key, false);
	
	zlog_info(g_dtree_log, "delete id: %d", current_node->m_key_map_id);

	current_node->m_key_map_id = 0;
	
	m_node_n --;
	
	m_rwlock.unlock();

	zlog_info(g_dtree_log, "done");

	return true;
}

const uint32_t dictionary_tree::get_node(const string& key)
{
	zlog_info(g_dtree_log, "get key: %s", key.c_str());

	m_rwlock.rdlock();

	s_dictionary_node* current_node = _search_node(key, false);

	m_rwlock.unlock();

	if(current_node == NULL)
	{
		return 0;
	}

	zlog_info(g_dtree_log, "got id: %d", current_node->m_key_map_id);

	zlog_info(g_dtree_log, "done");

	return current_node->m_key_map_id;
}

void _get_suggestion(s_dictionary_node* node, set<uint32_t>& key_map_ids_set, vector<uint32_t>& key_map_ids, const uint32_t& limit)
{
	key_it end = node->m_child_nodes.end();
	key_it beg = node->m_child_nodes.begin();

	key_it it = beg;

	string key;

	wstring wkey;

	wkey = node->m_key;

	dictionary_tree::wstr2str(wkey, key);
	
	zlog_debug(g_dtree_log, "into subkey: 0x%08x %s", node->m_key, key.c_str());

	uint32_t current_key_map_id = node->m_key_map_id;

	if(current_key_map_id > 0 && (key_map_ids_set.insert(current_key_map_id)).second)
	{
		key_map_ids.push_back(current_key_map_id);
	}

	while(it != end && key_map_ids.size() < limit)
	{
		uint32_t key_map_id = it->second.m_key_map_id;

		if(key_map_id > 0 && (key_map_ids_set.insert(key_map_id)).second /* unique */)
		{
			key_map_ids.push_back(key_map_id);
		}
		
		it++;
	}

	it = beg;
	
	while(it != end && key_map_ids.size() < limit)
	{
		_get_suggestion(&(it->second), key_map_ids_set, key_map_ids, limit);
		it++;
	}

	return;
}

const bool dictionary_tree::suggest_node(const string& key, vector<uint32_t>& key_map_ids, const uint32_t& limit)
{
	stringstream ss;

	for(size_t i = 0; i < key.size(); i++)
	{
		ss << boost::format(" 0x%02x") % (uint16_t)(key[i] & 0xff);
	}

	wstring wkey;
	stringstream wss;

	str2wstr(key, wkey);
	for(size_t i = 0; i < wkey.size(); i++)
	{
		wss << boost::format(" 0x%08x") % (uint32_t)wkey[i];
	}

	zlog_info(g_dtree_log, "get suggestion of %s hex: %s, whex: %s", key.c_str(), ss.str().c_str(), wss.str().c_str());

	m_rwlock.rdlock();

	s_dictionary_node* current_node = NULL;

	set<uint32_t> key_map_ids_set;
	
	for(size_t i = 0; i < wkey.size() && key_map_ids.size() < limit; i++)
	{
		current_node = __search_node(&(wkey[i]), true);
		if( current_node && current_node->m_key != 0)
		{
			_get_suggestion(current_node, key_map_ids_set, key_map_ids, limit);
		}
	}
	
	m_rwlock.unlock();

	zlog_info(g_dtree_log, "key %s got %lu suggestions", key.c_str(), key_map_ids.size());
	
	return true;
}
