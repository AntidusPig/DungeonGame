#include"nlohmann/json.hpp"
using json = nlohmann::json;
#include"entity.h"
#include"hurtable.h"

#ifndef LOOT_H
#define LOOT_H
namespace game{
    namespace loot{
        enum lootType{
            potion, arrow, meleeWeapon, rangedWeapon, magicWeapon, shield, helmet, cuirass
        };

        class Loot : public Entity, public Hurtable{ // lootable object
            public:
                std::wstring acronym = L"";
                Loot(entityid id, std::wstring name, lootType type, int healthRecoveryAmount, int manaRecoveryAmount, int meleeWeaponDamageBonus, int rangedWeaponDamageBonus, int magicWeaponDamageBonus, int shieldDefenseBonus, int helmetDefenseBonus, int cuirassDefenseBonus) : 
                            Entity(id, name), 
                            type(type),
                            healthRecoveryAmount(healthRecoveryAmount),
                            manaRecoveryAmount(manaRecoveryAmount),
                            meleeWeaponDamageBonus(meleeWeaponDamageBonus),
                            rangedWeaponDamageBonus(rangedWeaponDamageBonus),
                            magicWeaponDamageBonus(magicWeaponDamageBonus),
                            shieldDefenseBonus(shieldDefenseBonus),
                            helmetDefenseBonus(helmetDefenseBonus),
                            cuirassDefenseBonus(cuirassDefenseBonus){
                                int c=0;
                                for (int i=0;i<name.length();i++){
                                    if (isalpha(name[i])){
                                        acronym += wchar_t(std::tolower(name[i]));
                                        c++;
                                    }
                                    if (c==3){
                                        break;
                                    }
                                }
                            }
                lootType type;
                int healthRecoveryAmount;
                int manaRecoveryAmount;
                int meleeWeaponDamageBonus;
                int rangedWeaponDamageBonus;
                int magicWeaponDamageBonus;
                int shieldDefenseBonus;
                int helmetDefenseBonus;
                int cuirassDefenseBonus;
        };
    }
}

#endif