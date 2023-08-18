#pragma once
#include <functional>

#include "Observer.h"

template <typename T>
class Observable
{
    std::function<void(std::shared_ptr<Observer<T>>)> subscribe;

public:
    explicit Observable(std::function<void(std::shared_ptr<Observer<T>>)> subscribe): subscribe(std::move(subscribe))
    {
    }

    void Subscribe(std::shared_ptr<Observer<T>> observer) const
    {
        return subscribe(observer);
    }

    void Subscribe(std::function<void(T)> sub) const
    {
        Subscribe(std::make_shared<Observer<T>>(
            [=](const T& v)
            {
                sub(v);
            }
        ));
    }

    template <typename U>
    std::shared_ptr<Observable<U>> Select(std::function<U(T)> select)
    {
        return std::make_shared<Observable<U>>(
            [=](std::shared_ptr<Observer<T>> o)
            {
                Subscribe(
                    [=](const T& v)
                    {
                        o->OnNext(select(v));
                    }
                );
            }
        );
    }

    std::shared_ptr<Observable<T>> Where(std::function<bool(T)> where)
    {
        return std::make_shared<Observable<T>>(
            [=](std::shared_ptr<Observer<T>> o)
            {
                Subscribe(
                    [=](const T& v)
                    {
                        if (where(v))
                        {
                            o->OnNext(v);
                        }
                    }
                );
            }
        );
    }
};
