# README

编译运行注意事项:

Step 1:确保根目录下有data文件夹，并且其中附带ipbuffer文件夹

Step 2:编译:make	清除编译:make clean

Step 3:执行

​			服务器: ./bin/server 端口号

​			客户端: ./bin/client ip地址 端口号 消息队列键值



示例

​			在文件根目录下

​			make

​			./bin/server 1000

​			./bin/client 127.0.0.1 1000 1010

​			./bin/client 127.0.0.2 1000 1011

​			./bin/client 127.0.0.3 1000 1012