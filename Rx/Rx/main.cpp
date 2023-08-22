#include <functional>

#include "TestFunc.h"
#include "Sample\EnemySample.h"

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // メモリリークチェック用

    // 機能テスト
    TestFunc::DoTest();

    // 実用的なサンプル
    EnemySample();

    _CrtDumpMemoryLeaks(); // メモリリークチェック用
}
