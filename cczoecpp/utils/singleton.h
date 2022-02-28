#ifndef __ZOE_SINGLETON_H__
#define __ZOE_SINGLETON_H__

#include <memory>

namespace cczoe {

template <class T>
class Singleton
{
public:
    static T *getInstance()
    {
        static T instance;
        return &instance;
    }
};

template <class T>
class SingletonPtr
{
public:
    static std::shared_ptr<T> getInstance()
    {
        static std::shared_ptr<T> instancePtr(new T);
        return instancePtr;
    }
};

};

#endif