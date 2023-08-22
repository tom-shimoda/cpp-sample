#pragma once
#include <iostream>
#include <random>
#include <vector>

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
    std::string name;
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
    std::vector<Enemy> enemies;
    for (int i = 0; i < 3; i++)
    {
        enemies.emplace_back(i);
    }

    auto subject = std::make_shared<Subject<Enemy*>>();

    // dotダメージ処理
    static std::shared_ptr<Disposable> dotDamageDisposer;
    dotDamageDisposer = subject->GetObservable()
                               ->Where([](const Enemy* e)
                               {
                                   return !e->IsDead();
                               })
                               ->Subscribe([](Enemy* e)
                               {
                                   auto prev = e->hp;
                                   e->Damage(10);
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
        std::cout << "------------------- count " << counter << " -------------------" << std::endl;
        
        for (auto&& v : enemies)
        {
            subject->OnNext(&v);
        }
    }
}
