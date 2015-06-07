基于csdn的文章检索系统设计与实现
================================

>从CSDN博客官网获取10万篇左右的文章作为源数据,并存储.
>解决方法:
>>使用开源爬虫架构Scrapy设计专门获取文章内容的爬虫.
>>爬取信息时需要保存文章URL,标题,内容,作者等字段,分别作为文章的入口和待处理内容.
>>使用MYSQL存储数据.

>分析文章内容,提取关键信息.
>解决方法:
>>使用开源分词库ICTCLAS对文章进行分词和关键词提取.
>>将分词结果与相关文章以一定的结构存储在MYSQL中.

>将提取内容作为索引,开发索引服务,监听请求.
>解决方法:
>>最初拟定使用哈希表索引,索引键使用提取值的MD5压缩值.
>>>当前方案是建立字典树,节省内存而且方便提供推荐功能.
>>分析数据静态存储于MYSQL中,启动服务时导出数据至内存中.
>>使用开源库libevent监听http-post请求,以此为基础开发服务.
>>使用nginx转发http请求.

本系统使用的开源库
================================
>[Scrapy](https://github.com/scrapy/scrapy)
>[ICTCLAS2015](http://ictclas.nlpir.org/)
>[boost](http://www.boost.org/)
>[jsoncpp](https://github.com/open-source-parsers/jsoncpp)
>[libevent](http://libevent.org/)
>[zlog](https://github.com/HardySimpson/zlog)
