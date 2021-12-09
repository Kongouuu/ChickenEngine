#pragma once

#include "Engine/Core.h"
namespace ChickenEngine
{
    template<typename T>
    class CHICKEN_API Singleton {
    public:
        static T& instance() {
            static T instance;
            return instance;
        }

        Singleton(const Singleton&) = delete;
        Singleton& operator= (const Singleton) = delete;

    protected:
        Singleton() {}
    };

}
