#ifndef __DICTIONALY_TREE_HPP__
#define __DICTIONALY_TREE_HPP__

#include "rwlock.hpp"

#include "../global.hpp"

typedef struct _s_dictionary_node {

	wchar_t m_key;
	uint32_t m_key_map_id;
	std::map<wchar_t, struct _s_dictionary_node> m_child_nodes;

	_s_dictionary_node& operator[](const wchar_t& key)
	{
		return m_child_nodes[key];
	}

	_s_dictionary_node():
		m_key(0),
		m_key_map_id(0),
		m_child_nodes()
	{}
	
} s_dictionary_node;

class dictionary_tree
{
public:
	std::ostream& print(std::ostream& os);

	static const bool wstr2str(const std::wstring& wstr, std::string& str);

	static const bool str2wstr(const std::string& str, std::wstring& wstr);
	
	const uint32_t get_node(const std::string& key);

	const bool delete_node(const std::string& key);
	
	const bool insert_node(const std::string& key, const uint32_t& key_map_id);

	const bool suggest_node(const std::string& key, std::vector<uint32_t>& key_map_ids, const uint32_t& limit);

	const uint32_t operator[](const std::string& key)
	{
		return get_node(key);
	}

	const uint32_t get_node_num() const 
	{
		return m_node_n;
	}

	static dictionary_tree& instance()
	{
		if(m_dict_s == NULL)
		{
			m_dict_s = new dictionary_tree();
		}
		
		return (*m_dict_s);
	}


	static void destory()
	{
		if(m_dict_s != NULL)
		{
			delete m_dict_s;
			m_dict_s = NULL;
		}
		else
		{
			dzlog_error("dict is NULL, can't release it, ignore");
		}
	}

private:
	dictionary_tree():
		m_node_n(0),
		m_rwlock("dictionary_tree"),
		m_root()
	{}

	dictionary_tree(dictionary_tree&):
		m_rwlock("")
	{}

	~dictionary_tree()
	{
		dzlog_info("release dictionary_tree: " CSL_PRT_PR(CSL_GRN, "SUCCESS"));
	}
	
	dictionary_tree(const dictionary_tree&);

	s_dictionary_node* _search_node(const std::string& key, bool ignore);
	s_dictionary_node* __search_node(const std::wstring& key, bool ignore);


	size_t m_node_n;

	rwlock m_rwlock;

	s_dictionary_node m_root;

	static dictionary_tree* m_dict_s;
};

#endif	// __DICTIONALY_TREE_H__
