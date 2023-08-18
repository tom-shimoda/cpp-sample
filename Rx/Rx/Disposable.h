#pragma once

class Disposable
{
public:
    virtual ~Disposable() = default;
    virtual void Dispose()
    {
    }
};
