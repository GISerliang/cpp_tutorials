#ifndef SINGLETON_H_INCLUDED
#define SINGLETON_H_INCLUDED

#include <stdexcept>
#include <cstdlib>
#include <new>
#include <type_traits>
#include <atomic>
#include <mutex>

template <typename T>
class CreateStatic
{
private:
    using storage_type = typename std::aligned_storage<sizeof(T), alignof(T)>::type; // T 存储时内存对齐
public:

    static T* create()
    {
        static storage_type static_memory;
        return new(&static_memory) T;
    }
    static void destroy(volatile T* obj) // 保证实例销毁时总是从内存读取的，而非有时从寄存器读取，保证数据安全
    {
        obj->~T();
    }
};

template <typename T,
template <typename U> class CreatePolicy=CreateStatic> class singleton
{
    static std::atomic<T*> instance_;
    static std::atomic<bool> destroyed_;
    singleton(const singleton &rhs);
    singleton& operator=(const singleton&);

    static void OnDeadReference()
    {
        throw std::runtime_error("dead reference!");
    }

    static void DestroySingleton()
    {
        CreatePolicy<T>::destroy(instance_);
        instance_ = nullptr;
        destroyed_ = true;
    }

protected:
    static std::mutex mutex_;
    singleton() {}

public:
    static T& instance()
    {
        T * tmp_instance_ = instance_.load(std::memory_order_acquire); // 查找是否有实例
        if (tmp_instance_ == nullptr)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tmp_instance_ = instance_.load(std::memory_order_relaxed);
            if (tmp_instance_ == nullptr)
            {
                if (destroyed_)
                {
                    destroyed_ = false;
                    OnDeadReference();
                }
                else
                {
                    tmp_instance_ = CreatePolicy<T>::create();
                    instance_.store(tmp_instance_, std::memory_order_release);
                    // register destruction
                    std::atexit(&DestroySingleton);
                }
            }
        }
        return *tmp_instance_;
    }
};

template <typename T,
          template <typename U> class CreatePolicy> std::mutex singleton<T,CreatePolicy>::mutex_;
template <typename T,
          template <typename U> class CreatePolicy> std::atomic<T*> singleton<T,CreatePolicy>::instance_;
template <typename T,
          template <typename U> class CreatePolicy> std::atomic<bool> singleton<T,CreatePolicy>::destroyed_(false);

#endif // SINGLETON_H_INCLUDED
