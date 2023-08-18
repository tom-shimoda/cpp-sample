#pragma once
#include <functional>

template <typename T>
class Observer
{
    std::function<void(T)> onNext;

public:
    explicit Observer(std::function<void(T)> onNext) : onNext(std::move(onNext))
    {
    }

    void OnNext(T v) const
    {
        if (onNext == nullptr) return;
        onNext(v);
    }
};

