#pragma once
#include <list>
#include <memory>

#include "Observable.h"
#include "Observer.h"

template <typename T>
class Subject
{
    std::list<std::shared_ptr<Observer<T>>> observerList;

public:
    void OnNext(T v) const
    {
        for (auto&& e : observerList)
        {
            e->OnNext(v);
        }
    }

    std::shared_ptr<Observable<T>> GetObservable()
    {
        return std::make_shared<Observable<T>>(
            [=](std::shared_ptr<Observer<T>> o)
            {
                observerList.emplace_back(o);
            }
        );
    }
};
