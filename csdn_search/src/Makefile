include ../Config.mk

CFLAGS += -Wall

SRCDIR = tools

DEFINE = -DVERSION="0.0.1"

TGTDIR = . $(SRCDIR)
TGTSRC = $(foreach dir,$(TGTDIR),$(wildcard $(dir)/*.c))
OBJ    = $(patsubst %.c,%.o,$(TGTSRC))
DEP    = $(patsubst %.o,%.d,$(OBJ))

TARGET = csdn_search

TEST   = csdn_search_test

TEST_DIR  = test $(SRCDIR)
TEST_SRC  = $(foreach dir,$(TEST_DIR),$(wildcard $(dir)/*.c))
TEST_OBJ  = $(patsubst %.c,%.o,$(TEST_SRC))
TEST_DEP  = $(patsubst %.c,%.d,$(TEST_SRC))

TEST_INC  = $(INC)
TEST_LIB  = $(LIB)
TEST_LIBD = $(LIBD)

TEST_CFLAGS = $(CFALGS) -g -DDEBUG

DELETE = $(TARGET) $(OBJ) $(DEP) $(TEST_OBJ) $(TEST_DEP) $(TEST)

all: $(TARGET)

test: $(TEST)
	chmod +x ./$<
	./$<

install: $(TARGET)
	mv $(TARGET) ../bin

$(TEST): $(TEST_OBJ)
	$(CC) $(TEST_CFLAGS) $(DEFINE) $(TEST_INC) $(TEST_LIBD) $(TEST_LIB) $^ -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(DEFINE) $(INC) $(LIBD) $(LIB) $^ -o $@

%.o: %.c
	$(CC) -c $< -o $@

include $(DEP)
include $(TEST_DEP)

%.d: %.c
	$(CC) -MM $< > $@

.PHONY: clean all

clean:
	-rm -f $(DELETE)
