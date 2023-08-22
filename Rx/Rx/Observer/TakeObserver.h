#pragma once
#include "../Observer.h"

template <typename T>
class TakeObserver : public Observer<T>
{
    int counter;
    int takeCount;
    std::function<void()> onTakeEnd;

public:
    explicit TakeObserver(const std::function<void(T)>& onNext, int takeCount, std::function<void()> onTakeEnd)
        : Observer<T>(onNext), counter(0), takeCount(takeCount), onTakeEnd(std::move(onTakeEnd))
    {
    }

    void OnNext(T v) override
    {
        if (this->_onNext == nullptr) return;

        if (counter++ >= takeCount)
        {
            if (onTakeEnd != nullptr) onTakeEnd();
            return;
        }

        this->_onNext(v);
    }
};
