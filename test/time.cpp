#include <iostream>
#include <time.h>
std::string getTime();
int main(){
    std::cout << getTime() << std::endl;
    return 0;
    
}


std::string getTime()
{
    time_t timep;
    time (&timep); //获取time_t类型的当前时间
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );//对日期和时间进行格式化
    return tmp;
}
