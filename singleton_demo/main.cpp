#include <iostream>

#include "include/singleton.h"

using namespace std;

class TestSingleton: public singleton<TestSingleton, CreateStatic>
{
public:
    void SetTestStr(std::string str)
    {
        test_str = str;
    }

    std::string GetTestStr()
    {
        return test_str;
    }

private:
    std::string test_str;
};

int main()
{
    TestSingleton& test1 = TestSingleton::instance(); // 获取类实例
    TestSingleton& test2 = TestSingleton::instance();
    test1.SetTestStr("Test1");
    std::cout << test1.GetTestStr() << "\n" << test2.GetTestStr() << std::endl; // test1和test2访问同一个test_str对象，输出一致

    return 0;
}
