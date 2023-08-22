#pragma once
#include <iostream>
#include <memory>
#include <string>

#include "Empty.h"
#include "ObservableDestroyTrigger.h"
#include "Observable.h"
#include "Subject.h"

// オブザーバー連結パターン
static void ObserverChainTestWithLog(std::shared_ptr<Subject<std::string>> subject)
{
    const auto observable = subject->GetObservable();
    observable->Select<std::string>([](const std::string& s)
              {
                  std::cout << s << " is coming. Repeat twice." << std::endl;
                  return s + s;
              })
              ->Where([](const std::string& s)
              {
                  if (s == "FugaFuga")
                  {
                      std::cout << "FugaFuga don't output" << std::endl;
                      return false;
                  }

                  return true;
              })
              ->Subscribe([](const std::string& s)
              {
                  std::cout << "value: " << s << std::endl;
              });

    // 実行処理
    std::cout << "Send value." << std::endl;
    subject->OnNext("test");

    std::cout << "Send value. (2)" << std::endl;
    subject->OnNext("Fuga");

    std::cout << "Send value. (3)" << std::endl;
    subject->OnNext("HogeHoge");
}

// オブザーバー連結パターン を実際に利用するときの感じの例
static void ObserverChainTest(std::shared_ptr<Subject<std::string>> subject)
{
    const auto observable = subject->GetObservable();
    observable->Select<std::string>([](const std::string& s) { return s + s; })
              ->Where([](const std::string& s) { return s != "FugaFuga"; })
              ->Subscribe([](const std::string& s)
              {
                  std::cout << "value: " << s << std::endl;
              });

    // 実行処理
    std::cout << "Send value." << std::endl;
    subject->OnNext("test");

    std::cout << "Send value. (2)" << std::endl;
    subject->OnNext("Fuga");

    std::cout << "Send value. (3)" << std::endl;
    subject->OnNext("HogeHoge");
}

// Subscribeを複数回行うテスト
static void SubscribeManyTimesTest(std::shared_ptr<Subject<std::string>> subject)
{
    const auto observable = subject->GetObservable();
    observable->Select<std::string>([](const std::string& s) { return s + s; })
              ->Where([](const std::string& s) { return s != "FugaFuga"; })
              ->Subscribe([](const std::string& s)
              {
                  std::cout << "value: " << s << std::endl;
              });

    observable->Subscribe([](const std::string& s)
    {
        std::cout << "--------------------------------------" << std::endl;
    });

    // 実行処理
    std::cout << "Send value." << std::endl;
    subject->OnNext("test");

    std::cout << "Send value. (2)" << std::endl;
    subject->OnNext("Fuga");

    std::cout << "Send value. (3)" << std::endl;
    subject->OnNext("HogeHoge");
}

// Diposeテスト
static void DisposeTest(std::shared_ptr<Subject<std::string>> subject)
{
    const auto observable = subject->GetObservable();
    auto d1 =
        observable->Select<std::string>([](const std::string& s) { return s + s; })
                  ->Where([](const std::string& s) { return s != "FugaFuga"; })
                  ->Subscribe([](const std::string& s)
                  {
                      std::cout << "value: " << s << std::endl;
                  });

    static std::shared_ptr<Disposable> d2;
    d2 = observable->Subscribe([](const std::string& s)
    {
        std::cout << "--------------------------------------" << std::endl;

        // 登録した処理内でのDisposeテスト → この例の場合、初回出力後に廃棄される
        d2->Dispose();
    });

    // 実行処理
    std::cout << "Send value." << std::endl;
    subject->OnNext("test");

    std::cout << "Send value. (2)" << std::endl;
    subject->OnNext("Fuga");
    d1->Dispose();

    std::cout << "Send value. (3)" << std::endl;
    subject->OnNext("HogeHoge");
}

// AddToテスト
static void AddToTest(std::shared_ptr<Subject<std::string>> subject)
{
    // 寿命同期テスト用オブジェクト
    struct LifeTimeObject : ObservableDestroyTrigger
    {
    };
    auto lifetimeObj = std::make_shared<LifeTimeObject>();

    const auto observable = subject->GetObservable();
    auto d =
        observable->Select<std::string>([](const std::string& s) { return s + s; })
                  ->Where([](const std::string& s) { return s != "FugaFuga"; })
                  ->Subscribe([](const std::string& s)
                  {
                      std::cout << "value: " << s << std::endl;
                  })
                  ->AddTo(lifetimeObj);

    // 実行処理
    std::cout << "Send value." << std::endl;
    subject->OnNext("test");

    // d->Dispose(); // 寿命が来る前に手動でDispose
    lifetimeObj = nullptr; // 寿命同期用オブジェクトを削除

    std::cout << "Send value. (2)" << std::endl;
    subject->OnNext("Fuga");

    std::cout << "Send value. (3)" << std::endl;
    subject->OnNext("HogeHoge");
}

// 渡すものがない場合
static void EmptyTest(std::shared_ptr<Subject<Empty>> subject)
{
    const auto observable = subject->GetObservable();
    auto d =
        observable->Subscribe([](Empty _)
        {
            std::cout << "Empty Test." << std::endl;
        });

    // 実行処理
    std::cout << "Send value." << std::endl;
    subject->OnNext(Empty());
}
