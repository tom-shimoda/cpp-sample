#include <functional>
#include <iostream>

#include "Observer.h"
#include "Observable.h"

using namespace std;

int main()
{
    auto* source = new Observer<string>(nullptr);
    auto root = new Observable<string>{
        [&](Observer<string>* o)
        {
            source = o;
        }
    };

    // オブザーバー１つパターン
    /*/
    root->Subscribe(
        [](const string& s)
        {
            std::cout << "value: " << s << std::endl;
        }
    );
    //*/

    // オブザーバー連結パターン
    /*/
    root->Select<string>([](const string& s)
        {
            std::cout << s << " is coming. Repeat double." << std::endl;
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
    root->Select<string>([](const string& s) { return s + s; })
        ->Where([=](const string& s) { return s != "FugaFuga"; })
        ->Subscribe([](const string& s)
        {
            std::cout << "value: " << s << std::endl;
        });
    //*/


    // 実行処理
    std::cout << "Send value." << std::endl;
    source->OnNext("test");
    std::cout << "Send value. (2)" << std::endl;
    source->OnNext("Fuga");
    std::cout << "Send value. (3)" << std::endl;
    source->OnNext("HogeHoge");
}
