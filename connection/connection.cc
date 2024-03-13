#include"connection.h"
#include<assert.h>

 Connection::Connection()
 {
    //初始化数据库连接
    _conn = mysql_init(nullptr);
 }

 Connection::~Connection()
{
    //释放数据库连接资源
    if(_conn!= nullptr)
    {
        mysql_close(_conn);
    }
}

bool Connection::connect(string ip, unsigned short port,string user, string passwd,string dbname)
{
    //连接数据库
    MYSQL *p = mysql_real_connect(_conn, ip.c_str(), user.c_str(), passwd.c_str(), dbname.c_str(), port, nullptr, 0);
    assert(p!=nullptr);
    return p != nullptr;
}
//更新操作 insert delete update
bool Connection::updated(string sql)
{
    if(mysql_query(_conn, sql.c_str()))
    {
        LOG("更新失败："+ sql);
        std::cout<<mysql_error(_conn)<<std::endl;
        return false;
    }
    return true;
}
//查询操作
MYSQL_RES* Connection::query(string sql)
{   
    if(mysql_query(_conn, sql.c_str()))
    {
        LOG("查询失败："+ sql);
        return nullptr;
    }
    return mysql_use_result(_conn);
}
