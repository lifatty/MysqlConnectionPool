# 定义变量
SRC_DIR := .
COMMON_CONNPOOL_DIR := $(SRC_DIR)/CommonConnectionpool
CONNECTION_DIR := $(SRC_DIR)/connection

# 定义源文件列表
SRCS := $(wildcard $(SRC_DIR)/*.cc) $(wildcard $(COMMON_CONNPOOL_DIR)/*.cc) $(wildcard $(CONNECTION_DIR)/*.cc)

# 定义目标文件列表
OBJS := $(SRCS:.cc=.o)

# 定义编译器
CC := g++

# 定义编译选项
CFLAGS := -g -Wall

# 定义链接选项
LDFLAGS := -lmysqlclient -lpthread

# 定义目标
TARGET := main

# 编译规则
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 单个文件的编译规则
%.o: %.cc
	$(CC) $(CFLAGS) -c -o $@ $<

# 清理规则
clean:
	rm -f $(OBJS) $(TARGET)
