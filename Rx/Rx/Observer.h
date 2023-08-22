#pragma once
#include <functional>

template <typename T>
class Observer
{
protected:
    std::function<void(T)> _onNext;

public:
    explicit Observer(std::function<void(T)> onNext) : _onNext(std::move(onNext))
    {
    }

    virtual ~Observer() = default;

    virtual void OnNext(T v)
    {
        if (_onNext == nullptr) return;
        _onNext(v);
    }
};
