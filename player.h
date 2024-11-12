#include"hurtable.h"
#include<map>
#include<string>
#include<iterator>
#include"entity.h"


#ifndef PLAYER_H
#define PLAYER_H

namespace game{
    class Player : public Hurtable{
        public:
            std::map<entityid,int> inventory;  //item and count
            int activeInvItemIndex = 0;
            int totalItemCount = 0;
            int inventoryLimit = 15;
            std::wstring acronym;
            bool male;
            int mana;
            int meleeDamage;
            int rangedDamage;
            int magicDamage;
            int speed;
            int defense;
            int meleeWeaponDamageBonus=0;
            int rangedWeaponDamageBonus=0;
            int magicWeaponDamageBonus=0;
            int shieldDefenseBonus=0;
            int helmetDefenseBonus=0;
            int cuirassDefenseBonus=0;
            Player(std::wstring name, int health, int mana, int meleeDamage, int rangedDamage, int magicDamage, int speed, int defense, bool male):
                acronym(name.begin(),name.begin()+3),
                meleeDamage(meleeDamage),
                rangedDamage(rangedDamage),
                magicDamage(magicDamage),
                speed(speed),
                mana(mana),
                maxMana(mana),
                male(male),
                defense(defense){
                    this->health = health;
                    this->maxHealth = health;
                }
            bool pickup(entityid loot){
                // return true if can pick up
                if (totalItemCount < inventoryLimit){
                    if (inventory.count(loot)==0){ // if no such item
                        inventory[loot] = 1;
                    }else{
                        inventory[loot]++;
                    }
                    totalItemCount++;
                    return true;
                }
                return false;
            }
            entityid getItemByIndex(int index){
                index %= inventory.size(); // feel free to use out of bounds index
                auto it = inventory.begin();
                std::advance(it, index);
                return it->first;
            }
            void removeItem(){
                if (!inventory.empty()){
                    // otherwise, it is a bug
                    auto loot = getActiveItemID();
                    if (inventory[loot]==1){
                        inventory.erase(loot);
                    }else{
                        inventory[loot]--;
                    }
                    totalItemCount--;
                }
            }
            void removeArrow(){
                if (inventory.count(arrow)==1){//otherwise, bug
                    if (inventory[arrow]==1){
                        inventory.erase(arrow);
                    }else{
                        inventory[arrow]--;
                    }
                }
            }
            entityid getActiveItemID(){
                return getItemByIndex(activeInvItemIndex);
            }
            void nextItem(){
                if (inventory.size() > 1){
                    activeInvItemIndex++;
                    activeInvItemIndex %= inventory.size();
                }
            }
            void lastItem(){
                if (inventory.size()>1){
                    activeInvItemIndex--;
                    activeInvItemIndex %= inventory.size();
                }
            }
            int getDefenseBonus(){
                return shieldDefenseBonus+cuirassDefenseBonus+helmetDefenseBonus;
            }
            int recoverMana(int amount){ //recover amount returned
                mana += amount;
                if (mana > maxMana){
                    int temp = mana - maxMana;
                    mana = maxMana;
                    return amount - temp;
                }
                return amount;
            }
            bool hasArrow(){
                if (inventory.count(entityid::arrow)==1){
                    return true;
                }
                return false;
            }
            bool useMana(int amount){
                if (mana >= amount){
                    mana -= amount;
                    return true;
                }
                return false;
            }
            int minManaCost = 4;
            int maxManaCost = 10;
            int maxMana;
            
    };
}

#endif