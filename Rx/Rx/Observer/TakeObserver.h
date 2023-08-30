#pragma once
#include "../Observer.h"

template <typename T>
class TakeObserver : public Observer<T>
{
    int counter;
    int takeCount;

public:
    explicit TakeObserver(std::function<void(T)> onNext,
                          std::function<void()> onCompleted,
                          int takeCount)
        : Observer<T>(onNext, onCompleted),
          counter(0),
          takeCount(takeCount)
    {
    }

    void OnNext(T v) override
    {
        if (this->isStopped) return;

        this->_onNext(v);

        if (++counter >= takeCount)
        {
            if (this->_onCompleted != nullptr) this->_onCompleted();
        }
    }
};
