#include <functional>

#include "ObservableUtil.h"
#include "Sample/EnemySample.h"
#include "Sample/SampleFunc.h"
#include "Test/Test.h"

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // メモリリークチェック用

    // --- テスト ---
    Test::DoTest();

    // --- 機能テスト ---
    {
        // SampleFunc::DoIt();
    }

    // --- 実用的なサンプル ---
    {
        // 寿命同期テスト用オブジェクト
        struct LifeTimeObject : ObservableDestroyTrigger
        {
        };
        auto lifetimeObj = std::make_shared<LifeTimeObject>();

        // EnemySample();
        EnemySampleUseEveryUpdateObservable(lifetimeObj);

        auto counter = 0;
        while (counter++ < 10) // メインループの想定
        {
            std::cout << "------------------- Main loop frame " << counter << " -------------------" << std::endl;

            // 毎フレームEveryUpdateSubjectのOnNextを叩く
            ObservableUtil::DoEveryUpdate();
        }

        lifetimeObj = nullptr;
    }

    // メモリーリークチェックのタイミングではリークとして検知されてしまうので解放しておく
    ObservableUtil::everyUpdateSubject = nullptr;

    _CrtDumpMemoryLeaks(); // メモリリークチェック用
}
