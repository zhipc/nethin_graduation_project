# -*- coding: utf-8 -*-
import scrapy
from scrapy import log
from csdn_crawl.items import Article
from scrapy.contrib.spiders import CrawlSpider, Rule
from scrapy.contrib.linkextractors import LinkExtractor

class CsdnSpider(CrawlSpider):
    name = "csdn"
    allowed_domains = ["csdn.net"]
    start_urls = (
        'http://blog.csdn.net/',
    )
    
    rules = (
        Rule(LinkExtractor(allow = ('article/details/[0-9]+'),
                           allow_domains = ('blog.csdn.net'),
                           tags = ('a'),
                           attrs = ('href')),
                           callback = 'parse_item'),
        );

    def parse_item(self, response):
        item = Article()
        try:
            item['url'] = response.url
            item['title'] = ''.join(response.xpath('//div[@class="article_title"]//span[@class="link_title"]/a//text()').extract()).strip()
            item['author'] = ''.join(response.xpath('//div[@id="blog_userface"]//a[@class="user_name"]//text()').extract()).strip()
            item['content']= '\n'.join(response.xpath('//div[@id="article_content"]//text()').extract()).strip()
        except Exception,e:
            log.msg("%s" % e, level = log.ERROR)
            return

        if item['url'] == '':
            log.msg("url is empty", level = log.WARNING)
            return
        if item['title'] == '': 
            log.msg("title is empty, title field : %s" % response.xpath('//div[@class="article_title"]').extract(), level = log.WARNING)
            return
        if item['author'] == '': 
            log.msg("author is empty, author field : %s" % response.xpath('//div[@id="blog_userface"]').extract(), level = log.WARNING)
            return
        if item['content'] == '': 
            log.msg("content is empty, content field : %s" % response.xpath('//div[@id="article_content"]').extract(), level = log.WARNING)
            return
        return item

