#pragma once
#include <iostream>
#include <memory>
#include <string>

#include "../Observable.h"
#include "../ObservableDestroyTrigger.h"
#include "../ObservableUtil.h"
#include "../Subject.h"
#include "../Unit.h"

class TestFunc
{
    // オブザーバー連結パターン
    static void ObserverChainTestWithLog(const std::shared_ptr<Subject<std::string>>& subject)
    {
        const auto observable = subject->GetObservable();
        auto _ =
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

    // Selectの戻り値の型が異なる場合のテスト
    static void SelectObservableTest(const std::shared_ptr<Subject<std::string>>& subject)
    {
        const auto observable = subject->GetObservable();
        auto _ =
            observable->Select<int>([](const std::string& s) { return atoi(s.c_str()); })
                      ->Where([](int i) { return i > 0; })
                      ->Subscribe([](int i)
                      {
                          std::cout << "value: " << i << std::endl;
                      });

        // 実行処理
        std::cout << "Send value." << std::endl;
        subject->OnNext("123");

        std::cout << "Send value. (2)" << std::endl;
        subject->OnNext("0");

        std::cout << "Send value. (3)" << std::endl;
        subject->OnNext("-456");
    }

    // Subscribeを複数回行うテスト
    static void SubscribeManyTimesTest(const std::shared_ptr<Subject<std::string>>& subject)
    {
        const auto observable = subject->GetObservable();
        auto d1 =
            observable->Select<std::string>([](const std::string& s) { return s + s; })
                      ->Where([](const std::string& s) { return s != "FugaFuga"; })
                      ->Subscribe([](const std::string& s)
                      {
                          std::cout << "value: " << s << std::endl;
                      });

        auto d2 = observable->Subscribe([](const std::string& s)
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
    static void DisposeTest(const std::shared_ptr<Subject<std::string>>& subject)
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
    static void AddToTest(const std::shared_ptr<Subject<std::string>>& subject)
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
    static void EmptyTest(const std::shared_ptr<Subject<Unit>>& subject)
    {
        const auto observable = subject->GetObservable();
        auto d =
            observable->Subscribe([](Unit _)
            {
                std::cout << "Empty Test." << std::endl;
            });

        // 実行処理
        std::cout << "Send value." << std::endl;
        subject->OnNext(Unit());
    }

    // 毎フレーム更新処理として登録する例
    static void EveryUpdateTest()
    {
        // 毎フレーム更新したい処理の登録例 (UnirxのObservable.EveryUpdate()的な)
        auto _ =
            ObservableUtil::EveryUpdate()->Subscribe([](Unit _)
            {
                std::cout << "Update." << std::endl;
            });
    }

public:
    static void DoTest()
    {
        // オブザーバー連結パターン
        // ObserverChainTestWithLog(std::make_shared<Subject<std::string>>());

        // Selectの戻り値の型が異なる場合のテスト
        SelectObservableTest(std::make_shared<Subject<std::string>>());

        // Subscribeを複数回行うテスト
        // SubscribeManyTimesTest(std::make_shared<Subject<std::string>>());

        // Diposeテスト
        // DisposeTest(std::make_shared<Subject<std::string>>());

        // AddToテスト
        // AddToTest(std::make_shared<Subject<std::string>>());

        // 渡すものがない場合
        // EmptyTest(std::make_shared<Subject<Unit>>());

        // 毎フレーム更新処理として登録する例
        // EveryUpdateTest();
    }
};
