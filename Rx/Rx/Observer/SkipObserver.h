#pragma once
#include "../Observer.h"

template <typename T>
class SkipObserver : public Observer<T>
{
    int counter;
    int skipCount;

public:
    explicit SkipObserver(const std::function<void(T)>& onNext, int skipCount)
        : Observer<T>(onNext), counter(0), skipCount(skipCount)
    {
    }

    void OnNext(T v) override
    {
        if (this->_onNext == nullptr) return;

        if (counter++ < skipCount - 1) return;

        counter = 0;
        this->_onNext(v);
    }
};
