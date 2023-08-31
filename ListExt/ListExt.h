#pragma once
#include <functional>
#include <list>

template <typename T>
class ListExt
{
    std::list<T> _list;
    
public:
    using Predicate = std::function<bool(const T*)>;

    void Add(T&& element);
    void Add(const T& element);
    void AddRange(ListExt list);
    T* Find(Predicate predicate);
    void Remove(Predicate predicate);
    void Clear();
};

template <typename T>
void ListExt<T>::Add(T* element)
{
    _list.push_back(element);
}

template <typename T>
void ListExt<T>::AddRange(ListExt list)
{
    for (auto itr = list.begin(); itr != list.end(); ++itr)
    {
        auto copy = (*itr);
        _list.push_back(copy);
    }
    return true;
}

template <typename T>
T* ListExt<T>::Find(Predicate predicate)
{
    for (auto&& v : _list)
    {
        if (predicate(v))
        {
            return &v;
        }
    }

    return nullptr;
}

template <typename T>
void ListExt<T>::Remove(Predicate predicate)
{
    for (auto itr = _list.begin(); itr != _list.end();)
    {
        if (predicate(&*itr))
        {
            itr = _list.erase(itr);
            break;
        }
        ++itr;
    }
}
