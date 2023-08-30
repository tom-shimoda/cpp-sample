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

    // --- 機能単体のサンプル ---
    {
        SampleFunc::DoIt();
    }

    // --- 実用的なサンプル ---
    {
        switch (0)
        {
        case 0:
            // サンプル1
            std::cout << "\033[35m" << "##### EnemySample #####" << "\033[m" << std::endl;
            EnemySample();
            break;
        case 1:
            // サンプル2
            // 寿命同期テスト用オブジェクト
            struct LifeTimeObject : ObservableDestroyTrigger
            {
            };
            auto lifetimeObj = std::make_shared<LifeTimeObject>();

            std::cout << "\033[35m" << "##### EnemySampleUseEveryUpdateObservable #####" << "\033[m" << std::endl;
            EnemySampleUseEveryUpdateObservable(lifetimeObj);

            auto counter = 0;
            while (counter++ < 20) // メインループの想定
            {
                std::cout << "\033[34m" << "------------------- Main loop frame " << counter <<
                    " -------------------"
                    << "\033[m" << std::endl;

                // 毎フレームEveryUpdateSubjectのOnNextを叩く
                ObservableUtil::DoEveryUpdate();
            }

            lifetimeObj = nullptr;
            break;
        }
    }

    // メモリーリークチェックのタイミングではリークとして検知されてしまうので解放しておく
    ObservableUtil::everyUpdateSubject = nullptr;

    _CrtDumpMemoryLeaks(); // メモリリークチェック用
}
