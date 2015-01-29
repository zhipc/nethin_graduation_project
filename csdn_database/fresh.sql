/* create a database named gpro */
drop database if exists gpro;

create database gpro 
       default character set utf8 
       default collate utf8_unicode_ci;

use gpro;
create table crawl (
       id int(10) primary key auto_increment,
       url varchar(128),
       md5 varchar(65) unique,
       title varchar(65),
       author varchar(65),
       status int(10),
       content text,
       index index__md5 using btree (md5)
);

create table split (
       id int(10) primary key auto_increment,
       crawl_id int(10) unique, 
       split_words text
);

create table guess (
       id int(10) primary key auto_increment,
       key_word varchar(16) unique,
       suggest_words text,
       suggest_count int(10)
);

create table match_map (
       id int(15) primary key auto_increment,
       f_key varchar(16),
       b_key varchar(16),
       match_count int(10),
       index index__matching using btree (f_key, b_key)
);

create table key_map (
       id int(10) primary key auto_increment,
       key_word varchar(16),
       link_count int(10),
       link_crawls text,
       index index__key_word using btree (key_word)
);




       
