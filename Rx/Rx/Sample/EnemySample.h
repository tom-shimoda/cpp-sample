#pragma once
#include <iostream>
#include <random>

#include "../Subject.h"

static std::random_device rd;
static std::mt19937 gen(rd());

static int random(int low, int high)
{
    std::uniform_int_distribution<> dist(low, high);
    return dist(gen);
}

struct Enemy
{
    int uid;
    int hp;

    explicit Enemy(int uid): uid(uid), hp(100)
    {
        hp = random(1, 100);
    }

    void Damage(int value) { hp -= value; }
    bool IsDead() const { return hp <= 0; }
};

void EnemySample()
{
    Enemy enemy(0);

    auto subject = std::make_shared<Subject<Enemy*>>();

    // dotダメージ処理
    static std::shared_ptr<Disposable> dotDamageDisposer;
    dotDamageDisposer = subject->GetObservable()
                               ->Skip(3)
                               // ->Take(3)
                               // ->Take(3, [=] { dotDamageDisposer->Dispose(); })
                               ->Where([](const Enemy* e)
                               {
                                   return !e->IsDead();
                               })
                               ->Subscribe([](Enemy* e)
                               {
                                   auto prev = e->hp;
                                   e->Damage(15);
                                   std::cout << e->uid << "'s hp: " << prev << " -> " << e->hp << std::endl;
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
                                   std::cout << e->uid << " is dead." << std::endl;
                               });

    // 実行
    int counter = 0;
    while (counter++ < 10)
    {
        std::cout << "------------------- frame " << counter << " -------------------" << std::endl;

        subject->OnNext(&enemy);
    }

    // メモリーリークチェックのタイミングではリークとして検知されてしまうので解放しておく
    dotDamageDisposer = nullptr;
    checkDeadDisposer = nullptr;
    subject = nullptr;
}
