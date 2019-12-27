src=$(wildcard ./*.c)                         #获取当前目录下所有的c文件
#obj=main.o add.o sub.o mul.o
obj=$(patsubst ./%.c, ./%.o, $(src))          #将变量src中所有的.c替换为.o
target=app
CC=gcc
$(target):$(obj)
	$(CC) $(obj) -o $(target)

%.o:%.c
	$(CC) -c $< -o $@

clean:
	rm $(obj) $(target) -rf 
