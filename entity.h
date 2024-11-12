#include<string>

#ifndef ENTITY_H
#define ENTITY_H

namespace game{
    enum entityid{
        empty = 0,
        you = 1,          // all players have the same id 1, names tell them apart
        crow = 2,
        bat = 3,
        spider = 4,
        goblin = 5,
        ogre = 6,

        wall = 100,
        healthpotion = 99,
        manapotion = 98,
        goldenapple = 97,
        strawhat = 96,
        brokenwheel = 95,
        scrapemetal = 94,
        rustyhelmet = 93,
        crackedshield = 92,
        discardedcuirass = 91,
        largehealthpotion = 90,
        largemanapotion = 89,
        spidereye = 88,
        arrow = 87,
        woodensword = 86,
        makeshiftbow = 85,
        tyrostaff = 84
    };
    class Entity{  // wall and empty, and base class for other things
        // a place holder that stores some common data for all units in the dungeon
        // with entity, you can find all other information about enemy and player through the dictionaries in dungeon
        public:
            Entity(entityid id, std::wstring name){
                this->id = id;
                this->name = name;
            }
            entityid id;
            std::wstring name;
    };
}

#endif