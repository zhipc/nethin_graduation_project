#!/usr/bin/python
#-*- coding: utf-8 -*-
import re
import conf
import MySQLdb
import PyNLPIR

__mysql_conn__ = None



def __conn_mysql():
    global __mysql_conn__
    try:
        __mysql_conn__ = MySQLdb.Connect(host = conf.MYSQL_HOST, user = conf.MYSQL_USER, port = conf.MYSQL_PORT,
                                         passwd = conf.MYSQL_PASS, db = conf.MYSQL_DB, charset = conf.MYSQL_CSET )
    except Exception, e:
        print e

def get_mysql_conn():
    if __mysql_conn__ is None:
        __conn_mysql()
        
    return __mysql_conn__


def get_mysql_dic_cursor():
    cursor = None
    try:
        if __mysql_conn__ is None:
            __conn_mysql()

        cursor = __mysql_conn__.cursor(MySQLdb.cursors.DictCursor)

    except Exception, e:
        print e

    return cursor

def get_mysql_cursor(cursor_type = None):
    cursor = None
    try:
        if __mysql_conn__ is None:
            __conn_mysql()

        cursor = __mysql_conn__.cursor(cursor_type)

    except Exception, e:
        print e

    return cursor

def close_mysql_conn():
    if __mysql_conn__ is not None:
        __mysql_conn__.close()
    
    __mysql_conn__ = None


class NLPIRInitError(Exception):
    pass

def str_qb(string, charset = "utf-8"):
    """全角转半角"""
    ustring = unicode(string, charset)
    rstring = ""
    for uchar in ustring:
        inside_code=ord(uchar)
        if inside_code == 12288:                              #全角空格直接转换            
            inside_code = 32 
        elif (inside_code >= 65281 and inside_code <= 65374): #全角字符（除空格）根据关系转化
            inside_code -= 65248

        rstring += unichr(inside_code)
    return rstring.encode(charset)
    
def str_bq(string, charset = "utf-8"):
    """半角转全角"""
    ustring = unicode(string, charset)
    rstring = ""
    for uchar in ustring:
        inside_code=ord(uchar)
        if inside_code == 32:                                 #半角空格直接转化                  
            inside_code = 12288
        elif inside_code >= 32 and inside_code <= 126:        #半角字符（除空格）根据关系转化
            inside_code += 65248

        rstring += unichr(inside_code)
    return rstring.encode(charset)

def escape_str(text):
    if isinstance(text, unicode):
        text = text.encode('utf-8')
    else:
        text = str(text)

    return MySQLdb.escape_string(text)


def to_valid_str(text):
    if isinstance(text, unicode):
        text = text.encode('utf-8')
    elif not isinstance(text, str):
        text = str(text)

    text = text.replace("\xc2\xa0","")
    text = str_qb(text)
    text = re.sub(r"[\s]+", " ", text)
        
    return text.strip()


def nlpir_init():
    if not PyNLPIR.init(conf.DATA_DIR, PyNLPIR.Constants.CodeType.UTF8_CODE):
        raise NLPIRInitError("can't init NLPIR")


def analyze_str(text, pos_tag = False):
    ret_str = PyNLPIR.paragraph_process(text, pos_tag)
    return filter(lambda x: x != "", map(str.strip, ret_str.split(" ")))


def get_keywords(text, max_limit = 10, weight_out = False):
    ret_str = PyNLPIR.get_keywords(str(text), max_limit, weight_out)
    return filter(lambda x: x != "", map(str.strip, ret_str.split("#")))

    
def nlpir_exit():
    PyNLPIR.exit()


if __name__ == "__main__":
    import time
    cursor = get_mysql_dic_cursor()
    cursor.execute("select id,content from crawl where id = 3457")
    rows = cursor.fetchall()
    
    avg_analyze_speed = 0
    avg_keyword_speed = 0
    
    print "select done"
    filter_vn = lambda x: "/v" in x or "/n" in x
    nlpir_init()
    for row in rows:
        content = row["content"]
        content_len =  len(content)
        start_time = time.time()
        print "\n".join(filter(filter_vn, analyze_str(content, True)))
        use_time = time.time() - start_time
        analyze_speed = int(content_len / use_time)
        avg_analyze_speed += analyze_speed
        print "analyze_str:   %10d\t%10d chars use %10lf seconds, %10d c/s" % (row["id"],content_len, use_time, analyze_speed)
        start_time = time.time()
        print "\n".join(filter(filter_vn,get_keywords(content, weight_out = True)))
        use_time = time.time() - start_time
        analyze_speed = int(content_len / use_time)
        avg_keyword_speed += analyze_speed
        print "get_keywords:  %10d\t%10d chars use %10lf seconds, %10d c/s" % (row["id"],content_len, use_time, analyze_speed)
    nlpir_exit()
    print
    avg_analyze_speed /= len(rows)
    avg_keyword_speed /= len(rows)
    print "avg analyze speed: %20d, avg keywords speed: %20d" % (avg_analyze_speed, avg_keyword_speed)
