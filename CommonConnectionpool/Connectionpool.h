#pragma once
#include<iostream>
#include<string>
#include<queue>
#include<mutex>
#include<atomic>
#include<thread>
#include<memory>
#include<functional>
#include<condition_variable>
#include"../connection/connection.h"
#include"../public.h"



//数据库连接池
class Connectionpool
{
public:
    //获取连接池对象实例
    static Connectionpool* getConnectionpool();

    //给外部提供接口，从连接池中获取一个可用的空闲连接
    std::shared_ptr<Connection> getConnection();
    //~Connectionpool();
    
private:
    Connectionpool();//单例 构造函数私有化

    bool loadConfigFile();//从配置文件中加载配置项

    void produceConnTask();//运行在独立的线程中，专门负责生产新连接
    
    void scannerConnTask();//超过_MaxIdTime的空闲连接，进行连接回收

    //Mysql的ip地址、端口号（3306）、用户名、密码
    std::string _ip;
    unsigned short _port;
    std::string _userame;
    std::string _password;
    std::string _dbname;
    int _initSize;//连接池的初始连接量
    int _MaxSize;//连接池的最大连接量
    int _MaxIdTime;//连接池最大连接空闲时间 s
    int _connectionTimeout;//连接池获取连接的超时时间 ms

    std::atomic_int _connectionCnt;//记录连接所创建的connection连接的总数量

    std::queue<Connection*> _connectionQue;//连接队列
    std::mutex _queueMutex;//维护连接队列的线程安全互斥锁

    std::condition_variable cv;//设置条件变量，用于连接生产线程和连接消费线程通信
};