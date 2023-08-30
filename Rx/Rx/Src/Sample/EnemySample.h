#pragma once
#include <iostream>
#include <random>

#include "../ObservableDestroyTrigger.h"

static std::random_device rd;
static std::mt19937 gen(rd());

static int random(int low, int high)
{
    std::uniform_int_distribution<> dist(low, high);
    return dist(gen);
}

struct Enemy
{
    std::string name;
    int hp;

    explicit Enemy(std::string name): name(std::move(name)), hp(100)
    {
        hp = random(1, 100);
    }

    void Damage(int value)
    {
        hp -= value;
        hp = hp >= 0 ? hp : 0;
    }

    bool IsDead() const { return hp <= 0; }
};

// Enemy用のObservableを用意し、OnNextにEnemyを流す例
inline void EnemySample()
{
    constexpr int damageValue = 15;
    auto enemy = std::make_shared<Enemy>("Slime");
    auto subject = std::make_shared<Subject<Enemy*>>();

    // dotダメージ処理
    static std::shared_ptr<Disposable> dotDamageDisposer;
    dotDamageDisposer = subject->GetObservable()
                               ->Skip(3) // 3フレームおきに
                               ->Take(3) // 計3回
                               ->Where([](const Enemy* e) // 生存している敵に
                               {
                                   return !e->IsDead();
                               })
                               ->Subscribe(
                                   [](Enemy* e) // ダメージ処理
                                   {
                                       auto prev = e->hp;
                                       e->Damage(damageValue);
                                       std::cout << e->name << "'s hp: " << prev << " -> " << e->hp << std::endl;
                                   },
                                   []
                                   {
                                       // ストリーム終了 (この例の場合はTakeの指定回数完了時に呼ばれる)
                                       std::cout << "Completed." << std::endl;
                                       dotDamageDisposer->Dispose();
                                   }
                               );

    // 死亡検知
    static std::shared_ptr<Disposable> checkDeadDisposer;
    checkDeadDisposer = subject->GetObservable()
                               ->Where([](const Enemy* e)
                               {
                                   return e->IsDead();
                               })
                               ->Subscribe([](const Enemy* e)
                               {
                                   // 死亡ログは毎フレーム出力
                                   std::cout << e->name << " is dead." << std::endl;
                               });

    // 実行
    int counter = 0;
    while (counter++ < 20)
    {
        std::cout << "\033[34m" << "------------------- frame " << counter << " -------------------" << "\033[m" <<
            std::endl;

        subject->OnNext(enemy.get());
    }

    // 解放
    dotDamageDisposer->Dispose();
    dotDamageDisposer = nullptr;
    checkDeadDisposer->Dispose();
    checkDeadDisposer = nullptr;
    subject = nullptr;
}

// EveryUpdateObservableを利用した例
inline void EnemySampleUseEveryUpdateObservable(const std::shared_ptr<ObservableDestroyTrigger>& lifetimeObj)
{
    constexpr int damageValue = 15;
    auto enemy = std::make_shared<Enemy>("Slime");

    // dotダメージ処理
    static std::shared_ptr<Disposable> dotDamageDisposer;
    dotDamageDisposer = ObservableUtil::EveryUpdate()
                        ->Skip(2) // 2フレームおきに
                        ->Take(5) // 計5回
                        ->Where([=](Unit _) // 生存している敵に
                        {
                            return !enemy->IsDead();
                        })
                        ->Subscribe(
                            [=](Unit _) // ダメージ処理
                            {
                                auto prev = enemy->hp;
                                enemy->Damage(damageValue);
                                std::cout << enemy->name << "'s hp: " << prev << " -> " << enemy->hp << std::endl;
                            },
                            [=]
                            {
                                // ストリーム終了 (この例の場合はTakeの指定回数完了時に呼ばれる)
                                dotDamageDisposer->Dispose();
                            }
                        )
                        ->AddTo(lifetimeObj);

    // 死亡検知
    static std::shared_ptr<Disposable> checkDeadDisposer;
    checkDeadDisposer = ObservableUtil::EveryUpdate()
                        ->Where([=](Unit _)
                        {
                            return enemy->IsDead();
                        })
                        ->Take(1) // 死亡ログは一度だけ出す
                        ->Subscribe(
                            [=](Unit _)
                            {
                                std::cout << enemy->name << " is dead." << std::endl;
                            },
                            [=]
                            {
                                // ストリーム終了
                                checkDeadDisposer->Dispose();
                            }
                        )
                        ->AddTo(lifetimeObj);

    // 実行はmain()内のメインループで行われる
}
