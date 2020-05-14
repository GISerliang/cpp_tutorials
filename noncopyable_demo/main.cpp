#include <iostream>
#include <stdio.h>
#include <string.h>
#include <typeinfo>

using namespace std;

#define var_name(x) #x


class noncopyable
{
private:

protected:
    noncopyable() = default;
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};

class NonCopyableClass:noncopyable
{
};

class CopyableClass
{
public:
    CopyableClass(int test): m_test(test), m_buffer(new char[4])
    {
        cout << "CopyableClass(int test) " << m_test << endl;
    }
    CopyableClass operator = (const CopyableClass &copy_class)
    {
        cout << "CopyableClass operator = (const CopyableClass &copy_class) " << m_test << endl;
        m_test = copy_class.m_test;
        m_buffer = new char[strlen(copy_class.m_buffer)];
        memcpy(m_buffer, copy_class.m_buffer, strlen(copy_class.m_buffer));
    }
    CopyableClass(const CopyableClass &copy_class): m_test(copy_class.m_test)
    {
        cout << "CopyableClass(const CopyableClass &copy_class) " << m_test << endl;
        m_buffer = new char[strlen(copy_class.m_buffer)];
        memcpy(m_buffer, copy_class.m_buffer, strlen(copy_class.m_buffer));
    }

    ~CopyableClass()
    {
        cout << "CopyableClass release " << m_test << endl;
        delete m_buffer;
    }

    int m_test;
    char* m_buffer;
};

int main()
{
    CopyableClass copy_class1(1);
    cout << var_name(copy_class1) << endl;
    CopyableClass copy_class2(2);
    cout << var_name(copy_class2) << endl;
    copy_class2 = copy_class1;

    NonCopyableClass noncopy_class1, noncopy_class2;
    noncopy_class2 = noncopy_class1;
    NonCopyableClass noncopy_class3(noncopy_class1);

    return 0;
}
