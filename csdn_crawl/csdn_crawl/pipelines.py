# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: http://doc.scrapy.org/en/latest/topics/item-pipeline.html
import re
import MySQLdb
from scrapy import log

INSERT_SQL = "insert into crawl(url, url_md5, title, author, content, content_md5, status) values('%(url)s', md5('%(url)s'), '%(title)s', '%(author)s', '%(content)s', md5('%(content)s'), 0)"

spliter = re.compile(r'[\s\n]+')

def DelSpace(string):
    strarr = spliter.split(string)
    return ' '.join(strarr)

class CsdnCrawlPipeline(object):
    conn = None
    cursor = None
    def open_spider(self, spider):
        try:
            log.msg('start connect to mysql', log.INFO)
            self.conn = MySQLdb.Connection(host = 'localhost', user = 'root', db = 'gpro', charset = 'utf8')
            self.cursor = self.conn.cursor()
        except Exception,e:
            log.msg('connect to mysql error : %s' % e, log.ERROR)
            exit()
        log.msg('connected  mysql : %s' % self.conn, log.INFO);

    def process_item(self, item, spider):
        try:
            log.msg('start insert item to mysql' , log.INFO)
            sql = INSERT_SQL % { i : DelSpace(item[i]).replace("'","\'") for i in item }
            self.cursor.execute(sql)
        except Exception,e:
            log.msg('insert item error : %s\n sql : \n %s' % (e, sql), log.ERROR)
            
    def close_spider(self, spider):
        try:
            log.msg('start disconnect from mysql', log.INFO)
            self.cursor.close()
            self.conn.close()
        except Exception,e:
            log.msg('disconnect error : %s' % e, log.ERROR)
            exit()

