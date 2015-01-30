# -*- coding: utf-8 -*-

# Scrapy settings for csdn_crawl project
#
# For simplicity, this file contains only the most important settings by
# default. All the other settings are documented here:
#
#     http://doc.scrapy.org/en/latest/topics/settings.html
#

BOT_NAME = 'csdn_crawl'

SPIDER_MODULES = ['csdn_crawl.spiders']
NEWSPIDER_MODULE = 'csdn_crawl.spiders'

# Crawl responsibly by identifying yourself (and your website) on the user-agent
USER_AGENT = '(+http://www.nethin.net) "just for my graduation project, if you are angry with it, mail me <dipijvz7@gmail.com>"'

ITEM_PIPELINES = {'csdn_crawl.pipelines.CsdnCrawlPipeline':800}

DOWNLOAD_DELAY = 1.0
AUTOTHROTTLE_ENABLED = True
AUTOTHROTTLE_MAX_DELAY = 120.0
AUTOTHROTTLE_START_DELAY = 1.0

LOG_FILE = log/crawl.log
LOG_LEVEL = ERROR
