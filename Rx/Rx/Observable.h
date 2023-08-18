#pragma once
#include <functional>

#include "Observer.h"

template <typename T>
class Observable
{
    std::function<void(Observer<T>*)> subscribe;

public:
    explicit Observable(std::function<void(Observer<T>*)> subscribe): subscribe(std::move(subscribe))
    {
    }

    void Subscribe(Observer<T>* observer) const
    {
        return subscribe(observer);
    }

    void Subscribe(std::function<void(T)> sub) const
    {
        Subscribe(new Observer<T>{
            [=](const T& v)
            {
                sub(v);
            }
        });
    }

    template <typename U>
    Observable<U>* Select(std::function<U(T)> select)
    {
        return new Observable{
            [=](Observer<T>* o)
            {
                Subscribe(
                    [=](const T& v)
                    {
                        o->OnNext(select(v));
                    }
                );
            }
        };
    }

    Observable<T>* Where(std::function<bool(T)> where)
    {
        return new Observable{
            [=](Observer<T>* o)
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
        };
    }
};
