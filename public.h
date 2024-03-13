#pragma once
#include<iostream>
//反斜杠 \ 在宏定义中用于连接两行
#define LOG(str)\
    std::cout << __FILE__ << ":" << __LINE__ << " " << \
    __TIMESTAMP__ << ":" << str << std::endl;
