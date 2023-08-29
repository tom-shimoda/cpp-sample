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

    void Damage(int value) { hp -= value; }
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
                               ->Subscribe([](Enemy* e) // ダメージ処理
                               {
                                   auto prev = e->hp;
                                   e->Damage(damageValue);
                                   std::cout << e->name << "'s hp: " << prev << " -> " << e->hp << std::endl;
                               });

    // 死亡検知
    static std::shared_ptr<Disposable> checkDeadDisposer;
    checkDeadDisposer = subject->GetObservable()
                               ->Where([](const Enemy* e)
                               {
                                   return e->IsDead();
                               })
                               ->Subscribe([](const Enemy* e)
                               {
                                   std::cout << e->name << " is dead." << std::endl;
                               });

    // 実行
    int counter = 0;
    while (counter++ < 20)
    {
        std::cout << "------------------- frame " << counter << " -------------------" << std::endl;

        subject->OnNext(enemy.get());
    }

    // メモリーリークチェックのタイミングではリークとして検知されてしまうので解放しておく
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
                        ->Take(5, [=] { dotDamageDisposer->Dispose(); }) // 計5回 (5回完了したらDisposeする)
                        ->Where([=](Unit _) // 生存している敵に
                        {
                            return !enemy->IsDead();
                        })
                        ->Subscribe([=](Unit _) // ダメージ処理
                        {
                            auto prev = enemy->hp;
                            enemy->Damage(damageValue);
                            std::cout << enemy->name << "'s hp: " << prev << " -> " << enemy->hp << std::endl;
                        })
                        ->AddTo(lifetimeObj);

    // 死亡検知
    ObservableUtil::EveryUpdate()
        ->Where([=](Unit _)
        {
            return enemy->IsDead();
        })
        ->Subscribe([=](Unit _)
        {
            std::cout << enemy->name << " is dead." << std::endl;
        })
        ->AddTo(lifetimeObj);

    // 実行はmain()内のメインループで行われる
}
