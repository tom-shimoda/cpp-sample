#define _CRTDBG_MAP_ALLOC // メモリリークチェック用

#include <functional>
#include <iostream>

#include "Subject.h"

using namespace std;

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // メモリリークチェック用

    auto subject = std::make_shared<Subject<string>>();
    auto root = subject->GetObservable();

    // オブザーバー連結パターン
    /*/
    root->Select<string>([](const string& s)
        {
            std::cout << s << " is coming. Repeat twice." << std::endl;
            return s + s;
        })
        ->Where([=](const string& s)
        {
            if (s == "FugaFuga")
            {
                std::cout << "FugaFuga don't output" << std::endl;
                return false;
            }

            return true;
        })
        ->Subscribe([](const string& s)
        {
            std::cout << "value: " << s << std::endl;
        });
    //*/

    // オブザーバー連結パターン を実際に利用するときの感じの例
    //*/
    auto d1 = root->Select<string>([](const string& s) { return s + s; })
                  ->Where([=](const string& s) { return s != "FugaFuga"; })
                  ->Subscribe([](const string& s)
                  {
                      std::cout << "value: " << s << std::endl;
                  });
    //*/

    // Observerを複数登録するテスト
    static std::weak_ptr<Disposable> d2;
    d2 = root->Subscribe([=](const string& s)
    {
        std::cout << "--------------------------------------" << std::endl;

        // 登録した処理内でのDisposeテスト
        if (auto p = d2.lock()) p->Dispose();
    });


    // 実行処理
    std::cout << "Send value." << std::endl;
    subject->OnNext("test");
    // if (auto p = d2.lock()) p->Dispose(); // Observerを複数登録するテストするためコメントアウト

    std::cout << "Send value. (2)" << std::endl;
    subject->OnNext("Fuga");

    std::cout << "Send value. (3)" << std::endl;
    subject->OnNext("HogeHoge");

    // メモリリーク検出チェックのためスマートポインタ解放しておく
    root = nullptr;
    subject = nullptr;

    _CrtDumpMemoryLeaks(); // メモリリークチェック用
}
