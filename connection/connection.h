#pragma once
#include<iostream>
#include<mysql/mysql.h>
#include<string>
#include<ctime>
#include"../public.h"
using std::string;

//Mysql数据库的增删改查
class Connection
{
public:
    //初始化数据库连接
    Connection();
    //释放数据库连接资源
    ~Connection();
    //连接数据库
    bool connect(string ip, unsigned short port,string user, string passwd,string dbname);
    //更新操作 insert delete update
    bool updated(string sql);
    //查询操作
    MYSQL_RES* query(string sql);
    //刷新一下连接的起始空闲时间点
    void refreshAlivetime(){ _alivetime = clock(); }
    //返回存活时间
    clock_t getAlivetime()const { return clock() - _alivetime; }
private:
    MYSQL *_conn;
    clock_t _alivetime;//记录进入空闲状态后的起始存活时间
};