include ../Config.mk

CXXFLAGS += -Wall -g -O2

SRCDIR = tools

DEFINE = -DVERSION=\"0.0.1\" 

TGTDIR = . $(SRCDIR)
TGTSRC = $(foreach dir,$(TGTDIR),$(wildcard $(dir)/*.cpp))
OBJ    = $(patsubst %.cpp,%.o,$(TGTSRC))
DEP    = $(patsubst %.o,%.d,$(OBJ))

TARGET = csdn_search

TEST   = csdn_search_test

TEST_DIR  = test $(SRCDIR)
TEST_SRC  = $(foreach dir,$(TEST_DIR),$(wildcard $(dir)/*.cpp))
TEST_OBJ  = $(patsubst %.cpp,%.o,$(TEST_SRC))
TEST_DEP  = $(patsubst %.o,%.d,$(TEST_OBJ))

TEST_INC  = $(INC)
TEST_LIB  = $(LIB)
TEST_LIBD = $(LIBD)

TEST_CXXFLAGS = $(CFALGS) -g -DDEBUG

DELETE = $(TARGET) $(OBJ) $(DEP) $(TEST_OBJ) $(TEST_DEP) $(TEST)


all: $(TARGET)

test: $(TEST)
	@echo [START_TEST]
	@./$<
	@echo [END_TEST]

install: $(TARGET)
	mv $(TARGET) ../bin

$(TEST): $(TEST_OBJ)
	$(CXX) \
	$^ \
	$(TEST_CXXFLAGS) \
	$(DEFINE) \
	$(TEST_INC) \
	$(TEST_LIBD) \
	$(TEST_LIB) \
	-o $@


$(TARGET): $(OBJ)
	$(CXX) \
	$^ \
	$(CXXFLAGS) \
	$(DEFINE) \
	$(INC) \
	$(LIBD) \
	$(LIB) \
	-o $@

sinclude $(DEP)
sinclude $(TEST_DEP)

%.d: %.cpp
	$(CXX) \
	$< \
	$(INC) \
	-MM \
	> $@

%.o: %.cpp
	$(CXX) \
	-c \
	$< \
	$(CXXFLAGS) \
	$(DEFINE) \
	$(INC) \
	$(LIBD) \
	$(LIB) \
	-o $@

.PHONY: clean all

clean:
	-rm -f $(DELETE)
