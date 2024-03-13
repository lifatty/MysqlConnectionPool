#include <iostream>
#include<stdio.h>
#include"connection/connection.h"
#include"CommonConnectionpool/Connectionpool.h"
int main()
{
    clock_t begin = clock();
    //Connectionpool *cp = Connectionpool::getConnectionpool();
    for(int i = 0; i < 1000; ++i)
    {
        Connection conn;
        char sql[1024] = {0};
        sprintf(sql,"insert into user(name,age,sex) values('%s',%d, '%s')","zhangsan", 18, "male");
        //std::cout<<sql<<std::endl;
        
        conn.connect("localhost",3306,"root", "scc2002", "chat");
        conn.updated(sql);

        // std::shared_ptr<Connection> sp = cp->getConnection();
        // char sql[1024] = {0};
        // sprintf(sql,"insert into user(name,age,sex) values('%s',%d, '%s')","zhangsan", 18, "male");
        // sp->updated(sql);
    }
    clock_t end = clock();
    std::cout << (end - begin) << "ms" <<std::endl;

    return 0;
}