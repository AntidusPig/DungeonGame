#ifndef HURTABLE_H
#define HURTABLE_H

namespace game{
    class Hurtable{
        public:
            int health; // can be destroyed
            bool dead = false;
            Hurtable(){
                this->health = health;
            }
            virtual int hurt(int amount){ //return amount hurt
                if (amount <= 0){return 0;}
                health -= amount;
                if (health<=0){
                    die();
                    return amount + health;
                }
                return amount;
            }
            virtual int heal(int amount){  //return amount healed
                health += amount;
                if (health > maxHealth){
                    int temp = health - maxHealth;
                    health = maxHealth;
                    return amount - temp;
                }
                return amount;
            }
            virtual void die(){
                dead = true; // class Dungeon check this regularly
            }
            int maxHealth;
    };
}

#endif