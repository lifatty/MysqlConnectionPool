#include"Connectionpool.h"

//线程安全的懒汉单例函数接口
Connectionpool* Connectionpool::getConnectionpool()
{
    static Connectionpool pool;
    return &pool;
}

Connectionpool::Connectionpool()
{
    if(!loadConfigFile())
    {
        return;
    }
    for(int i = 0; i < _initSize; ++i)
    {
        Connection *p = new Connection();
        p->connect(_ip, _port, _userame, _password, _dbname);
        p->refreshAlivetime();
        _connectionQue.push(p);
        _connectionCnt++;
    }

    //生产者线程
    std::thread produce(std::bind(&Connectionpool::produceConnTask,this));
    produce.detach();
    //超过_MaxIdTime的空闲连接，进行连接回收
    std::thread scanner(std::bind(&Connectionpool::scannerConnTask,this));
    scanner.detach();
}

void Connectionpool::produceConnTask()
{
    //std::unique_lock<std::mutex> lock(_queueMutex);
        for(;;)
        {
            std::unique_lock<std::mutex> lock(_queueMutex);
            while(!_connectionQue.empty())
            {
                cv.wait(lock);
            }
            //连接数量没有到达上限，继续创建新连接
            if(_connectionCnt < _MaxSize)
            {
                Connection *p = new Connection();
                p->connect(_ip, _port, _userame, _password, _dbname);
                p->refreshAlivetime();
                _connectionQue.push(p);
                _connectionCnt++;
            }
            //通知消费者线程可以连接
            cv.notify_all();    
        }
}

void Connectionpool::scannerConnTask()
{
    for(;;)
    {
        //通过sleep模拟定时效果
        std::this_thread::sleep_for(std::chrono::seconds(_MaxIdTime));
        //扫描整个队列，释放多余连接
        std::unique_lock<std::mutex> lock(_queueMutex);
        while (_connectionCnt > _initSize)
        {
            Connection *p = _connectionQue.front();
            if(p->getAlivetime() >= (_MaxIdTime*1000))
            {
                _connectionQue.pop();
                _connectionCnt--;
                delete p;
            }
            else{
                break;
            }
        }
        
    }
}

std::shared_ptr<Connection> Connectionpool::getConnection()
{
     std::unique_lock<std::mutex> lock(_queueMutex);
     while(_connectionQue.empty())
     {
        if(std::cv_status::timeout ==  cv.wait_for(lock, std::chrono::milliseconds(_connectionTimeout)))
        {   
            if(_connectionQue.empty())
            {
                LOG("获取空闲连接超时...连接失败");
                return nullptr;
            }
        }
     }
     //shared_ptr智能指针析构时，会把connection的资源直接close掉<=>调用connection的析构函数，connection就被close掉了
     //需要自定义shared_ptr的释放资源的方式，吧connection直接归还到queue中
     std::shared_ptr<Connection> sp(_connectionQue.front(),
        [&](Connection *pcon){
        //队列的线程安全操作
         std::unique_lock<std::mutex> lock(_queueMutex);
         pcon->refreshAlivetime();
        _connectionQue.push(pcon);
        });
     _connectionQue.pop();
     
    cv.notify_all();
    
     return sp;
}

bool Connectionpool::loadConfigFile()
{
    FILE *pf = fopen("./mysql.cnf","r");
    if(pf == nullptr)
    {
        LOG("mysql.cnf file is not exit");
        return false;
    }
    //feof是检测流上的文件结束符的函数，若文件结束返回0
    while (!feof(pf))
    {
        char line[1024]={0};
        fgets(line, 1024, pf);
        std::string str = line;
        int idx = str.find("=",0);
        if(idx == -1)
        {
            continue;
        }
        int endidx = str.find("\n",idx);
        string key = str.substr(0,idx);
        string value = str.substr(idx+1,endidx-idx-1);

        if(key == "ip")
        {
            _ip = value;
        }
        else if(key == "port")
        {
            _port = atoi(value.c_str());
        }
        else if(key == "username")
        {
            _userame = value;
        }
         else if(key == "password")
        {
            _password = value;
        }
        else if(key == "dbname")
        {
            _dbname = value;
        }
         else if(key == "initSize")
        {
            _initSize = atoi(value.c_str());
        }
         else if(key == "MaxSize")
        {
            _MaxSize = atoi(value.c_str());
        }
         else if(key == "MaxIdTime")
        {
            _MaxIdTime = atoi(value.c_str());
        }
        else if(key == "connectionTimeout"){
            _connectionTimeout = atoi(value.c_str());
        }

    }
    return true;
}