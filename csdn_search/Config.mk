
CC = gcc

CUR_DIR = /home/nethin/gpro/csdn_search

BASE_INCS = /usr/local/include

BASE_LIBS_DIR = /usr/local/lib
BASE_LIBS_LIB = event zlog


USER_INCS     = $(CUR_DIR)/include

USER_LIBS_DIR = $(CUR_DIR)/lib
USER_LIBS_LIB =


CONF_INC  = $(BASE_INCS) $(USER_INCS)
CONF_LIB  = $(BASE_LIBS_LIB) $(USER_LIBS_LIB)
CONF_LIBD = $(USER_LIBS_DIR) $(USER_LIBS_DIR)

CONF_CFLAGS = -Wall

MYSQL_CONF   = /usr/lib64/mysql/mysql_config

MYSQL_INCS   = $(shell $(MYSQL_CONF) --include)
MYSQL_LIBS   = $(shell $(MYSQL_CONF) --libs)
MYSQL_CFLAGS = $(shell $(MYSQL_CONF) --cflags)


INC  = $(foreach i,$(CONF_INC),-I$(i)) $(MYSQL_INCS) 
LIB  = $(foreach l,$(CONF_LIB),-l$(l)) $(MYSQL_LIBS) 
LIBD = $(foreach L,$(CONF_LIBD),-L$(L))