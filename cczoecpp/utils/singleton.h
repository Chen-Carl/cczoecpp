#ifndef __CCZOE_SINGLETON_H__
#define __CCZOE_SINGLETON_H__

#include <memory>

namespace cczoe {

template <class T>
class Singleton
{
public:
    static T *GetInstance()
    {
        static T instance;
        return &instance;
    }
};

template <class T>
class SingletonPtr
{
public:
    static std::shared_ptr<T> GetInstance()
    {
        static std::shared_ptr<T> instancePtr = std::make_shared<T>();
        return instancePtr;
    }
};

};

#endif