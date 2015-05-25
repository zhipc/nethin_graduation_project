
CC  = gcc
CXX = g++ 


USR = /usr
LOCAL = /usr/local

SYS_DIR = $(USR) $(LOCAL)

BASE_INCS = $(foreach d,$(SYS_DIR),$(d)/include)

BASE_LIBS_DIR = $(foreach d,$(SYS_DIR),$(d)/lib) $(foreach d,$(SYS_DIR),$(d)/lib64)
BASE_LIBS_LIB = 

CUR_DIR = /home/nethin/gpro/csdn_search

USER_INCS     = $(CUR_DIR)/include

USER_LIBS_DIR = $(CUR_DIR)/lib $(CUR_DIR)/lib64
USER_LIBS_LIB = event jsoncpp mysqlcppconn


CONF_INC  = $(BASE_INCS) $(USER_INCS)
CONF_LIB  = $(BASE_LIBS_LIB) $(USER_LIBS_LIB)
CONF_LIBD = $(BASE_LIBS_DIR) $(USER_LIBS_DIR)

CONF_CXXFLAGS = -static 

MYSQL_CONF   = /usr/lib64/mysql/mysql_config

MYSQL_INCS   = $(shell $(MYSQL_CONF) --include)
MYSQL_LIBS   = $(shell $(MYSQL_CONF) --libs)
MYSQL_CFLAGS = $(shell $(MYSQL_CONF) --cflags)


INC  = $(foreach i,$(CONF_INC),-I$(i)) 
LIB  = $(foreach l,$(CONF_LIB),-l$(l)) 
LIBD = $(foreach L,$(CONF_LIBD),-L$(L))

CXXFLAGS = $(CONF_CXXFLAGS)