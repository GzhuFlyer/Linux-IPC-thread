#在显式请求时执行命令的名字
.PHONY:clean all
cc=gcc
CFLAGS=-Wall -g
BIN=print
all:$(BIN)
%.o:%.c
	$(cc) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)

