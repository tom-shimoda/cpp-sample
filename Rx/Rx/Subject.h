#pragma once
#include <list>
#include <memory>

#include "Observable.h"
#include "Observer.h"

template <typename T>
class Subject
{
    struct Source
    {
        std::shared_ptr<Observer<T>> observer;
        std::shared_ptr<Disposable> disposer;

        Source(std::shared_ptr<Observer<T>> observer, std::shared_ptr<Disposable> disposer)
            : observer(std::move(observer)),
              disposer(std::move(disposer))
        {
        }
    };

    std::list<Source> source;

    struct Disposer : Disposable
    {
        std::list<Source>* src;

        Disposer(std::list<Source>* src): src(src)
        {
        }

        ~Disposer() override = default;

        void Dispose() override
        {
            for (auto itr = src->begin(); itr != src->end();)
            {
                if ((*itr).disposer.get() == this)
                {
                    itr = src->erase(itr);
                    break;
                }
                ++itr;
            }
        }
    };

public:
    void OnNext(T v) const
    {
        for (auto&& e : source)
        {
            e.observer->OnNext(v);
        }
    }

    std::shared_ptr<Observable<T>> GetObservable()
    {
        return std::make_shared<Observable<T>>(
            [=](std::shared_ptr<Observer<T>> o) -> std::weak_ptr<Disposable>
            {
                auto disposer = std::make_shared<Disposer>(&source);
                source.emplace_back(o, disposer);
                return disposer;
            }
        );
    }
};
