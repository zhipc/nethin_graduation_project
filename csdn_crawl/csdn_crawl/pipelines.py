# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: http://doc.scrapy.org/en/latest/topics/item-pipeline.html


class CsdnCrawlPipeline(object):
    def process_item(self, item, spider):
        print "url:",item['url']
        print "title:",item['title'].encode('utf-8')
        print "author:",item['author'].encode('utf-8')
        print "content:",(item['content'])[0:10].encode('utf-8') + '...'
        
