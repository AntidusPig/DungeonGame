#include"hurtable.h"
#include<string>

#ifndef ENEMY_H
#define ENEMY_H 

namespace game{
    class Enemy : public Hurtable{ // mobile entity, initialize from external file
        public:
            std::wstring acronym;  // string to show in game
            int damage;
            bool friendly;
            bool alerted;
            int speed;
            int defense;
            Enemy(std::wstring name, int health, int damage, int speed, int friendly, int defense):
                acronym(name.begin(),name.begin()+3),
                damage(damage),
                speed(speed),
                friendly(friendly),
                defense(defense),
                alerted(false){
                    this->health = health;
                    this->maxHealth = health;
                }
    };
}

#endif
