#include <functional>
#include <iostream>

#include "TestFunc.h"
#include "Subject.h"

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // メモリリークチェック用

    auto subject = std::make_shared<Subject<std::string>>();

    // オブザーバー連結パターン
    // ObserverChainTestWithLog(subject);

    // オブザーバー連結パターン を実際に利用するときの感じの例
    // ObserverChainTest(subject);

    // Subscribeを複数回行うテスト
    // SubscribeManyTimesTest(subject);

    // Diposeテスト
    // DisposeTest(subject);

    // AddToテスト
    AddToTest(subject);


    // メモリリーク検出チェックに引っかかるためスマートポインタ解放しておく
    subject = nullptr;

    _CrtDumpMemoryLeaks(); // メモリリークチェック用
}
