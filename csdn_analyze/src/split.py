#!/usr/bin/python
#-*- coding:utf-8 -*-
import json
import tools
import cPickle as pickle

conn = None
cursor = None

rows_once = 200

get_sql = "select * from crawl where id >= %%d and status = 0 limit %d" % rows_once

get_max_id_sql = "select max(id) as id from crawl"

update_sql = "update crawl set status = %d where id = %d"

insert_sql = "insert ignore into "

max_id = 0

pickle_data = 'pdata'

def update_pickle(row):
    try:
        data = {"id": row["id"], "status": row["status"]}
        fp = open(pickle_data, 'wb')
        pickle.dump(data, fp)
        fp.close()
    except Exception, e:
        print e


def load_pickle():
    row = {"id": 0, "status": 0}
    try:
        fp = open(pickle_data, 'rb')
        row = pickle.load(fp)
        fp.close()
    except Exception ,e:
        pass

    return row


def insert_split(row):
    keys = "split(" + ", ".join(row.keys())  + ")"
    vals = "values('" + "', '".join(map(tools.escape_str, row.values())) + "')"
    sql = insert_sql + " ".join([keys, vals])
    cursor.execute(sql)
    conn.commit()


def update_status(row, status):
    row['status'] = status
    cursor.execute(update_sql % (status, row["id"]))
    conn.commit()
    update_pickle(row)


def inner_analyze(row):
    split = {"crawl_id": str(row["id"])}
    content = tools.to_valid_str(row['content'])
    split["split_words"] = "\n".join(tools.analyze_str(content, True))
    split["split_key_words"] = "\n".join(tools.get_keywords(content, 50, True))
    insert_split(split)


def analyze_handle(row):
    update_status(row, 2) # fail
    inner_analyze(row)
    update_status(row, 1) # success
    

def get_rows(start_id):
    sid = int(start_id)
    cursor.execute(get_sql % sid)

    return cursor.fetchall()


def get_max_id():
    cursor.execute(get_max_id_sql)
    row = cursor.fetchone()
    return row["id"]
    

def init_analyze():
    global conn, cursor, max_id
    try:
        tools.nlpir_init()
        conn = tools.get_mysql_conn()
        cursor = tools.get_mysql_dic_cursor()
        max_id = get_max_id()
    except Exception, e:
        print e
        exit(1)


def exit_analyze():
    tools.nlpir_exit()


handle_pipeline = [analyze_handle]

def start_analyze(start_id):
    rows = get_rows(start_id)
    end_id = 0
    for row in rows:
        for handle in handle_pipeline:
            handle(row)
            
        end_id = row["id"]

    return end_id


def analyze(start_id):
    init_analyze()
        
    while max_id > start_id:
        print "strat from %d, max_id is %d" % (int(start_id), int(max_id))
        start_id = start_analyze(start_id)

    exit_analyze()


def main():
    row = load_pickle()
    analyze(row['id'] + 1)
    exit(0)


if __name__ == '__main__':
    main()

