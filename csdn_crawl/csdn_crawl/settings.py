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
AUTOTHROTTLE_MAX_DELAY = 30.0
AUTOTHROTTLE_START_DELAY = 2.0

LOG_LEVEL = 'WARNING'

#DEPTH_PRIORIRY = 5
#DEPTH_STATS_VERBOSE = True

# duplicate filter
DUPEFILTER_DEBUG = True

MEMDEBUG_ENABLED = True
MEMDEBUG_NOTIFY = ['dipijvz7@126.com']

MEMUSAGE_ENABLE = True
MEMUSAGE_LIMIT_MB = 128
MEMUSAGE_NOTIFY_MAIL = MEMDEBUG_NOTIFY

RANDOMIZE_DOWLOAD_DELAY = True

REDIRECT_MAX_TIMES = 10

ROBOTSTXT_OBEY = True
TELNETCONSOLE_PORT = 0


COOKIES_ENABLED = False
RETRY_ENABLED = False
REDIRECT_ENABLED = False
AJAXCRAWL_ENABLED = True
