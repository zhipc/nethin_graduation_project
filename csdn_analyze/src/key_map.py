#!/usr/bin/python
#-*- coding:utf-8 -*-
import json
import tools

iu_sql = "insert into key_map(%(keys)s) values('%(vals)s') on duplicate key update %(kvs)s"
get_sql = "select * from key_map where key_word = '%s'"
get_split_sql = "select * from split where id > %d limit 200"


conn = None
cursor = None

data_file = 'km_data'

def iu_key_map(row):
    query = {}
    query['kvs'] = ", ".join([k + " = '" + tools.escape_str(v) + "'" for k, v in row.iteritems()])
    query['keys'] = ", ".join(row.keys())
    query['vals'] = "', '".join(map(tools.escape_str, row.values()))

    cursor.execute(iu_sql % query)
    conn.commit()


def get_key_map(key):
    cursor.execute(get_sql % tools.escape_str(key))
    return cursor.fetchone()

def get_f(key):
    if len(key) == 3:
        return key[2]
    else:
        return '0.1'

def set_key_map(key, crawl_id):
    row = get_key_map(key[0])
    
    if row == None:
        row = {'key_word': key[0], 'link_crawls': '', 'link_count': 0}
        
    row['link_crawls'] += (',' if row['link_count'] else '') + str(crawl_id) + '_' + get_f(key)
    row['link_count'] += 1
    
    iu_key_map(row)


def get_key_words(text):
    key_words = map(lambda x: x.split('/'), text.split('\n'))
    return key_words


def update_status(status):
    with open(data_file, 'wb') as fp:
        json.dump(status, fp)


def get_status():
    status = {'id': 0}
    try:
        status = json.load(status)
    except:
        pass

    return status


def get_split(start_id):
    cursor.execute(get_split_sql % int(start_id))
    return cursor.fetchall()


def init_all():
    global conn, cursor
    conn = tools.get_mysql_conn()
    cursor = tools.get_mysql_dic_cursor()


def get_max_id():
    sql = "select max(id) as id from split"
    cursor.execute(sql)
    row = cursor.fetchone()
    return row['id']
    

def deal_key_map(start_id):
    splits = get_split(start_id)
    for split in splits:
        start_id = split['id']
        update_status({'id': start_id})
        key_words = get_key_words(split['split_key_words'])
        crawl_id = split['crawl_id']
        for key_word in key_words:
            set_key_map(key_word, crawl_id)

    return start_id


def main():
    init_all()
    status = get_status()
    max_id = get_max_id()
    start_id = status['id']

    while max_id > start_id:
        print "start_id : %d, max_id : %d" % (start_id, max_id)
        start_id = deal_key_map(start_id)
    

if __name__ == "__main__":
    main()
    
