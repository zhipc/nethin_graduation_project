/* create a database named gpro */
drop database if exists gpro;

create database gpro 
       default character set utf8 
       default collate utf8_unicode_ci;

use gpro;
create table crawl (
       id int(10) primary key auto_increment,
       url varchar(128),
       url_md5 varchar(65) unique,
       content_md5 varchar(65) unique,
       title varchar(65),
       author varchar(65),
       status int(10),
       content text,
       index index__md5 using btree (url_md5, content_md5)
);

create table split (
       id int(10) primary key auto_increment,
       crawl_id int(10) unique,
       split_words text,
       split_key_words text
);

create table key_map (
       id int(10) primary key auto_increment,
       key_word varchar(16) unique,
       link_count int(10),
       link_crawls text
);




       
