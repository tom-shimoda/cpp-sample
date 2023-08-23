#include <conio.h>
#include <functional>

#include "TestFunc.h"
#include "Sample\EnemySample.h"

#include "ObservableUtil.h"

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // メモリリークチェック用

    // 機能テスト
    // TestFunc::DoTest();

    // 実用的なサンプル
    EnemySample();

    auto counter = 0;
    while (counter++ < 10) // メインループの想定
    {
        // 毎フレームEveryUpdateSubjectのOnNextを叩く
        ObservableUtil::DoEveryUpdate();
    }

    // メモリーリークチェックのタイミングではリークとして検知されてしまうので解放しておく
    ObservableUtil::everyUpdateSubject = nullptr;

    _CrtDumpMemoryLeaks(); // メモリリークチェック用
}
