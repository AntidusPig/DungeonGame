#include<vector>
#include<map>
#include<utility>
#include<string>
#include<fstream>
#include<stdlib.h>
#include"gameutil.h"
#include"player.h"
#include"enemy.h"
#include"v2di.h"
#include"entity.h"
#include"loot.h"

#ifndef DUNGEON_H
#define DUNGEON_H
namespace game{
    class Dungeon{
        // the dungeon and logic about the stuff in it
        friend class GameManager;
        private:
            Entity*** scene;
            // to describe an instance of enemy, use 
            // enemies
            // scene + enemyPos
            // to describe an instance of a player, use
            // players
            // scene + playerPos

            std::map<Entity*, Enemy*> enemies;
            std::map<Entity*, Player*> players;
            std::vector<v2di> enemyPos;  // you can find anything else with the exact position, needed for later computation
            std::vector<v2di> playerPos; // 3 players at maximum
            size_t activePlayerIndex = 0;
            std::vector<int> playerInventoryActiveItemIndex = {0,0,0};
            int score=0;
            
            // read from file
            // key of map must be unique but also easy to use later
            std::map<entityid, std::pair<std::wstring,Enemy*>> enemieTemplates;  // each type of enemy have different id
            std::map<entityid, std::pair<std::wstring,loot::Loot*>> loots;  // loots are the same, each stored only once in the program, here
            std::map<std::wstring, std::pair<entityid,Player*>> playerTemplates;  // each player have different name, same id 1
            // selection for attack
            v2di selectedPos;
            int width;
            int height;
            void loadTemplate(std::string path = "config/", std::string enemyConfigName = "enemy", std::string playerConfigName = "player", std::string lootConfigName = "loot"){
                std::ifstream fin(path+enemyConfigName+".json");
                if (fin.fail()){
                    std::wcout << L"Cannot open enemy config." << std::endl;
                }else{
                    json data;
                    try{
                        data = json::parse(fin);
                    }catch (json::parse_error& e){
                        std::wcout << "enemy config parsing error!" << std::endl;
                        std::wcout << e.what() << std::endl;
                        exit(-1);
                    }
                    
                    for (auto it = data.begin(); it!=data.end(); ++it){
                        auto name = std::wstring(it.key().begin(),it.key().end());
                        enemieTemplates[it.value()["id"]] = std::pair<std::wstring,Enemy*>(name, 
                                                    new Enemy(name,it.value()["health"],
                                                    it.value()["damage"],it.value()["speed"],
                                                    it.value()["friendly"], it.value()["defense"]));
                    }
                    fin.close();
                }
                std::ifstream fin2(path+playerConfigName+".json");
                if (fin2.fail()){
                    std::wcout << L"Cannot open player config." << std::endl;
                }else{
                    json data;
                    try{
                        data = json::parse(fin2);
                    }catch (json::parse_error& e){
                        std::wcout << "player config parsing error!" << std::endl;
                        std::wcout << e.what() << std::endl;
                        exit(-1);
                    }
                    
                    for (auto it = data.begin(); it!=data.end(); ++it){
                        auto name = std::wstring(it.key().begin(),it.key().end());
                        playerTemplates[name] = 
                        std::pair<entityid,Player*>(it.value()["id"], 
                                                    new Player(name,it.value()["health"],
                                                    it.value()["mana"],it.value()["meleeDamage"],
                                                    it.value()["rangedDamage"],it.value()["magicDamage"],
                                                    it.value()["speed"], it.value()["defense"], it.value()["male"]));
                    }
                    fin2.close();
                }
                std::ifstream fin3(path+lootConfigName+".json");
                if (fin3.fail()){
                    std::wcout << L"Cannot open loot config." << std::endl;
                }else{
                    json data;
                    try{
                        data = json::parse(fin3);
                    }catch (json::parse_error& e){
                        std::wcout << "loot config parsing error!" << std::endl;
                        std::wcout << e.what() << std::endl;
                        exit(-1);
                    }
                    
                    for (auto it = data.begin(); it!=data.end(); ++it){
                        auto name = std::wstring(it.key().begin(),it.key().end());
                        loots[it.value()["id"]] = std::pair<std::wstring, loot::Loot*>(name, new loot::Loot(it.value()["id"],name,it.value()["lootType"],
                                                    it.value()["healthRecoveryAmount"],it.value()["manaRecoveryAmount"],
                                                    it.value()["meleeWeaponDamageBonus"],it.value()["rangedWeaponDamageBonus"],
                                                    it.value()["magicWeaponDamageBonus"], it.value()["shieldDefenseBonus"], 
                                                    it.value()["helmetDefenseBonus"], it.value()["cuirassDefenseBonus"]));
                    }
                    fin3.close();
                }
            }
            void spawnEnemy(int row, int column, entityid id){
                // only used at map generation
                if (id < 2 || id > 50 ){
                    std::wcout << L"Enemy No." << id << L" undefined" << std::endl;
                    return;
                }
                // to avoid memory leak
                if (enemies.count(scene[row][column]) == 1){ 
                    // if occupied
                    std::wcout << L"Position (" << row << L", " << column << L") occupied by an enemy." << std::endl;
                    return;
                }
                if (players.count(scene[row][column]) == 1){ 
                    // if occupied
                    std::wcout << L"Position (" << row << L", " << column << L") occupied by a player." << std::endl;
                    return;
                }
                
                scene[row][column]->id = id;
                scene[row][column]->name = enemieTemplates[id].first;
                enemies[scene[row][column]] = new Enemy(*(enemieTemplates[id].second));
                enemyPos.push_back(v2di{row,column});
            }
            void spawnPlayer(int row, int column, std::wstring name){ // spawn a player, only used at map generation
                if (playerPos.size() == 3) {return;} // 3 players at maximum
                if (enemies.count(scene[row][column]) == 1){ 
                    // if occupied
                    std::wcout << L"Position (" << row << L", " << column << L") occupied by an enemy." << std::endl;
                    return;
                }
                if (players.count(scene[row][column]) == 1){ 
                    // if occupied
                    std::wcout << L"Position (" << row << L", " << column << L") occupied by a player." << std::endl;
                    return;
                }
                // if we have a player template called "name"
                if (playerTemplates.count(name)==1){
                    // add the player to the dungeon
                    scene[row][column]->id = playerTemplates[name].first;
                    scene[row][column]->name = name;
                    players[scene[row][column]] = new Player(*(playerTemplates[name].second));
                    playerPos.push_back(v2di{row, column});
                }else{
                    std::wcout << L"Player " << std::wstring(name.begin(),name.end()) << L" undefined" << std::endl;
                    return;
                }
            }
            void spawnWall(int row, int column){
                scene[row][column]->id = entityid::wall;
                scene[row][column]->name = L"wall";
            }
            void spawnEmpty(int row, int column){
                scene[row][column]->id = entityid::empty;
                scene[row][column]->name = L"empty";
            }
            void createEnvironment(){
                // create empty
                for (int i=0;i<height;i++){
                    for (int j=0;j<width;j++){
                        scene[i][j] = new Entity(entityid::empty, L"empty");   //tbh smart_ptr should be used
                    }
                }
                // create walls
                for (int i=0;i<width;i++){
                    spawnWall(0,i);
                    spawnWall(height-1,i);
                }
                for (int i=0;i<height;i++){
                    spawnWall(i,0);
                    spawnWall(i,width-1);
                }
                std::vector<int> wx{5,4,5,5,4,4, 1,1,1,1,     2,3,   6,7,7,8,8,9,       7,8,9,10,11,12,13 ,8,9,9,10,11,11,12,12, 13,13,13,13,13,13,13,13, 12,12,11,10,9,12};
                std::vector<int> wy{5,5,6,3,3,4, 2,10,11,12, 11,10,  18,17,18,17,18,18, 5,5,5,5,5,5,5,     4,4,3,3,2,1,2,1       ,6,7,8,9,10,11,12,13,14, 14,15,16,16,17,3};
                for (int i=0;i<wx.size();++i){
                    spawnWall(wx[i],wy[i]);
                }
            }
            void swapEntity(const v2di& grid1, const v2di& grid2){
                // Exchange the Entity* at grid1 and grid2
                Entity* temp = scene[grid1.x][grid1.y];
                scene[grid1.x][grid1.y] = scene[grid2.x][grid2.y];
                scene[grid2.x][grid2.y] = temp;
            }
            bool movePlayer(int dir){
                // return true if movement legal
                v2di targetPos;
                v2di pPos = playerPos[activePlayerIndex];
                switch (dir)
                {
                case 'w':
                    targetPos = pPos + v2di{-1,0};  // 0,0 is top left
                    break;
                case 'a':
                    targetPos = pPos + v2di{0,-1};
                    break;
                case 'd':
                    targetPos = pPos + v2di{0,1};
                    break;
                case 's':
                    targetPos = pPos + v2di{1,0};
                    break;
                }
                
                if (scene[targetPos.x][targetPos.y]->id == empty){
                    // swap Entity*
                    swapEntity(targetPos, pPos);
                    // playerPos updated
                    playerPos[activePlayerIndex].x = targetPos.x;
                    playerPos[activePlayerIndex].y = targetPos.y;
                    std::wstring temp = (dir=='w')?L"up":(dir=='a')?L"left":(dir=='s')?L"down":L"right";
                    log.add(getPlayerName(activePlayerIndex) + L" moved " + temp);
                    return true;
                }else if (scene[targetPos.x][targetPos.y]->id == wall){
                    log.add(getPlayerName(activePlayerIndex) + L" cannot move into the rock");
                    return false;
                }else if (scene[targetPos.x][targetPos.y]->id == you){
                    // swap Entity* with another player character
                    swapEntity(targetPos, pPos);
                    int tempIndex;
                    for(int i=0;i<playerPos.size();i++){
                        if (playerPos[i] == targetPos){
                            tempIndex = i;
                        }
                    }
                    // playerPos updated
                    playerPos[activePlayerIndex] = targetPos;
                    playerPos[tempIndex] = pPos;
                    log.add(scene[targetPos.x][targetPos.y]->name + L" swapped position with " + scene[pPos.x][pPos.y]->name);
                    return true;
                }else if (scene[targetPos.x][targetPos.y]->id >= crow && scene[targetPos.x][targetPos.y]->id <= 50){
                    log.add(getPlayerName(activePlayerIndex) + L" cannot move any closer to " + scene[targetPos.x][targetPos.y]->name);
                    return false;
                }else if(scene[targetPos.x][targetPos.y]->id >= 51 && scene[targetPos.x][targetPos.y]->id <= healthpotion){
                    // item is picked up
                    log.add(getPlayerName(activePlayerIndex) + L" picked up the " + loots[scene[targetPos.x][targetPos.y]->id].first + L" from the ground");
                    if (players[scene[pPos.x][pPos.y]]->inventory.count(scene[targetPos.x][targetPos.y]->id)==1){
                        players[scene[pPos.x][pPos.y]]->inventory[scene[targetPos.x][targetPos.y]->id]++;
                    }else{
                        players[scene[pPos.x][pPos.y]]->inventory[scene[targetPos.x][targetPos.y]->id] = 1;
                    }
                    // swap Entity*
                    swapEntity(targetPos, pPos);
                    // playerPos updated
                    playerPos[activePlayerIndex].x = targetPos.x;
                    playerPos[activePlayerIndex].y = targetPos.y;
                    spawnEmpty(pPos.x,pPos.y);
                    return true;
                }else {return false;} //or raise error?
            }
            std::wstring getPlayerName(size_t playerIndex) const{ // careful, I don't check for out of bounds
                return scene[playerPos[playerIndex].x][playerPos[playerIndex].y]->name;
            }
            Player* getPlayerByIndex(int playerIndex){
                return players[scene[playerPos[playerIndex].x][playerPos[playerIndex].y]];
            }
            std::wstring getEnemyName(v2di enemyPos) const{
                return scene[enemyPos.x][enemyPos.y]->name;
            }
            void nextPlayer(){
                // active player becomes next one
                activePlayerIndex++;
                activePlayerIndex%=playerPos.size();
            }
            void lastPlayer(){
                // active player becomes last one
                activePlayerIndex--;
                activePlayerIndex%=playerPos.size();
            }
            void nextItem(){
                // active player change active item to next one
                getPlayerByIndex(activePlayerIndex)->nextItem();
            }
            void lastItem(){
                // active player change active item to last one
                getPlayerByIndex(activePlayerIndex)->lastItem();
            }
            bool useItem(){
                //active player use active item
                Player* ptemp = getPlayerByIndex(activePlayerIndex);
                auto pname = getPlayerName(activePlayerIndex);
                auto name = loots[ptemp->getActiveItemID()].first;
                loot::Loot* item = loots[ptemp->getActiveItemID()].second;
                switch (item->type)
                {
                case loot::arrow:{
                    log.add(L"Use "+name+L" by shooting it.");
                    return false;
                    break;}
                case loot::potion:{
                    std::wstring verb;
                    std::wstring effect;
                    if (item->healthRecoveryAmount!=0 && item->manaRecoveryAmount!=0){
                        int dHP = ptemp->heal(item->healthRecoveryAmount);
                        int dMP = ptemp->recoverMana(item->manaRecoveryAmount);
                        if (item->id == spidereye){
                            verb = L"ate";
                            effect = L"Ewww! "+std::to_wstring(item->manaRecoveryAmount)+L" MP was restored but "+pname+L" was disgusted.";
                        }
                    }else if (item->id == goldenapple){
                            verb = L"ate";
                            int dHP = ptemp->heal(item->healthRecoveryAmount);
                            effect = L"Tasty! "+std::to_wstring(dHP)+L" HP was restored.";
                        }else if (item->healthRecoveryAmount!=0){
                        int dHP = ptemp->heal(item->healthRecoveryAmount);
                        verb = L"drank";
                        effect = std::to_wstring(dHP) + L" health point was restored.";
                    }else{
                        int dMP = ptemp->recoverMana(item->manaRecoveryAmount);
                        verb = L"drank";
                        effect = std::to_wstring(dMP) + L" mana point was restored.";
                    }
                    log.add(pname+L" "+verb+L" the "+name+L", "+effect);
                    break;
                    }
                // note that new equipment will replace the old one, even if the old one is better
                case loot::meleeWeapon:{
                    log.add(pname+L" equipped "+name+L", gaining "+std::to_wstring(item->meleeWeaponDamageBonus)+L" more damage in close combat.");
                    ptemp->meleeWeaponDamageBonus = item->meleeWeaponDamageBonus;
                    break;
                    }
                case loot::rangedWeapon:{
                    log.add(pname+L" equipped "+name+L", gaining "+std::to_wstring(item->rangedWeaponDamageBonus)+L" more damage with bows.");
                    ptemp->rangedWeaponDamageBonus = item->rangedWeaponDamageBonus;
                    break;
                    }
                case loot::magicWeapon:{
                    log.add(pname+L" equipped "+name+L", gaining "+std::to_wstring(item->magicWeaponDamageBonus)+L" more damage when conjuring.");
                    ptemp->magicWeaponDamageBonus = item->magicWeaponDamageBonus;
                    break;
                    }
                case loot::shield:{
                    log.add(pname+L" equipped "+name+L", getting "+std::to_wstring(item->shieldDefenseBonus)+L" denfense skill.");
                    ptemp->shieldDefenseBonus = item->shieldDefenseBonus;
                    break;
                }
                case loot::helmet:{
                    log.add(pname+L" equipped "+name+L", getting "+std::to_wstring(item->helmetDefenseBonus)+L" denfense skill.");
                    ptemp->helmetDefenseBonus = item->helmetDefenseBonus;
                    break;
                }
                case loot::cuirass:{
                    log.add(pname+L" equipped "+name+L", getting "+std::to_wstring(item->cuirassDefenseBonus)+L" denfense skill.");
                    ptemp->cuirassDefenseBonus = item->cuirassDefenseBonus;
                    break;
                }
                default:
                    break;
                }
                ptemp->removeItem();
                return true;
            }
            void dropItem(){
                // active player drop active item
                Player* ptemp = getPlayerByIndex(activePlayerIndex);
                auto name = loots[ptemp->getActiveItemID()].first;
                loot::Loot* item = loots[ptemp->getActiveItemID()].second;
                log.add(getPlayerName(activePlayerIndex) + L" dropped one " + name);
                ptemp->removeItem();
            }
            void select(){
                selectedPos = playerPos[activePlayerIndex];
            }
            void moveSelection(char dir){
                switch (dir)
                {
                case 'w':
                    selectedPos += v2di{-1,0};
                    break;
                case 'a':
                    selectedPos += v2di{0,-1};
                    break;
                case 'd':
                    selectedPos += v2di{0,1};
                    break;
                case 's':
                    selectedPos += v2di{1,0};
                    break;
                }
            }
            std::pair<int,bool> hurtEnemy(v2di ePos, int amount){
                // ePos is the position of enemy to hurt
                // return damage and if causes death
                // enemy index is the index in enemyPos
                Entity* enEntity = scene[ePos.x][ePos.y];
                int damageDealt = enemies[enEntity]->hurt(amount);
                bool isDead = enemies[enEntity]->dead;
                if (isDead){
                    Enemy* enemy = enemies[enEntity];
                    enemies.erase(enEntity);
                    for(auto i=enemyPos.begin();i<enemyPos.end();i++){
                        if (*i==ePos){
                            enemyPos.erase(i);
                            break;
                        }
                    }
                    if (randint(0,1)==0){
                        enEntity->id = (entityid)randint(84,99);
                    }else{enEntity->id = arrow;}
                    enEntity->name = loots[enEntity->id].first;
                    delete enemy;
                }
                score += damageDealt;
                return std::pair<int,bool>(damageDealt, isDead);
            }
            std::pair<int,bool> hurtPlayer(int playerIndex, int amount){
                Player* player = getPlayerByIndex(playerIndex);
                int damageDealt = player->hurt(amount);
                bool isDead = player->dead;
                if (isDead){
                    v2di pp = playerPos[playerIndex];
                    Entity* entityAtPlayerPos = scene[pp.x][pp.y];
                    // erase player
                    players.erase(entityAtPlayerPos);
                    playerPos.erase(playerPos.begin()+playerIndex);
                    entityAtPlayerPos->name = L"empty";
                    entityAtPlayerPos->id = game::empty;
                    delete player;
                    if (playerPos.size() == 0){
                        activePlayerIndex = -1;  // game should exit
                    }else if (playerIndex < activePlayerIndex){
                        lastPlayer();
                    }else if (playerIndex==activePlayerIndex){
                        nextPlayer();
                    }
                }
                return std::pair<int,bool>(damageDealt, isDead);
            }
            bool playerAttack(char attackType){  // j melee, k ranged, l magic
                // if legal move, return true (a turn is used)
                Player* ptemp = getPlayerByIndex(activePlayerIndex);
                auto pname = getPlayerName(activePlayerIndex);
                auto id = scene[selectedPos.x][selectedPos.y]->id;
                if (id==you){
                    if (playerPos[activePlayerIndex]==selectedPos){
                        std::wstring pronoun = ptemp->male?L"himself":L"herself";
                        log.add(pname+L" do not want to hit "+pronoun+L".");
                    }else{
                        std::wstring pronoun = ptemp->male?L"his":L"her";
                        log.add(pname+L" will never betray "+pronoun+L" friend.");
                    }
                    return false;
                }else if(id==empty){
                    log.add(L"There is no one there.");
                    return false;
                }else if(id==wall){
                    log.add(L"It's not time to practice now.");
                    return false;
                }else if(id>=crow && id<=50){
                    auto name = scene[selectedPos.x][selectedPos.y]->name;
                    auto enemyEntity = scene[selectedPos.x][selectedPos.y];
                    if (attackType == 'j'){
                        if(playerPos[activePlayerIndex].adjacent(selectedPos)){  // melee
                            auto name = L"the "+enemyEntity->name;
                            auto result = hurtEnemy(selectedPos,ptemp->meleeDamage+ptemp->meleeWeaponDamageBonus-enemies[enemyEntity]->defense);
                            std::wstring temp = L" points of damage, ";
                            if (result.second){
                                temp += name + L" was killed.";
                            }else if (result.first > enemies[enemyEntity]->maxHealth/2){
                                temp += name + L" was badly hurt.";
                            }else{
                                temp = L" points of damage.";
                            }
                            log.add(pname+L" "+choice<std::wstring>(meleeVerbs)+L" "+name+L", dealing "+ std::to_wstring(result.first) +temp);
                            return true;
                        }else{
                            log.add(L"Enemy is too far.");
                            return false;
                        }
                    }else if(attackType=='k'){
                        if (playerPos[activePlayerIndex].adjacent(selectedPos)){
                            std::wstring pronoun = ptemp->male?L"his":L"her";
                            log.add(L"Too late to draw out "+pronoun+L" bow.");
                            return false;
                        }else if(ptemp->hasArrow()){
                            auto name = L"the "+enemyEntity->name;
                            auto result = hurtEnemy(selectedPos,ptemp->rangedDamage+ptemp->rangedWeaponDamageBonus-enemies[enemyEntity]->defense);
                            std::wstring temp = L" points of damage, ";
                            if (result.second){
                                temp += name+L" was killed.";
                            }else if (result.first > enemies[enemyEntity]->maxHealth/2){
                                temp += name+L" was badly hurt.";
                            }else{
                                temp = L" points of damage.";
                            }
                            log.add(pname+L" "+choice<std::wstring>(rangedVerbs)+L" "+name+L", dealing "+ std::to_wstring(result.first) +temp);
                            ptemp->removeArrow();
                            return true;
                        }else{
                            log.add(L"No arrow left.");
                            return false;
                        }
                    }else{
                        int manaCost = randint(ptemp->minManaCost, ptemp->maxManaCost);
                        if (ptemp->useMana(manaCost)){
                            auto name = L"the "+enemyEntity->name;
                            auto result = hurtEnemy(selectedPos,ptemp->magicDamage+ptemp->magicWeaponDamageBonus+(manaCost/2)-enemies[enemyEntity]->defense);
                            std::wstring temp = L" points of damage, ";
                            if (result.second){
                                temp += name+L" was killed.";
                            }else if (result.first > enemies[enemyEntity]->maxHealth/2){
                                temp += name+L" was badly hurt.";
                            }else{
                                temp = L" points of damage.";
                            }
                            log.add(pname+L" "+choice<std::wstring>(magicVerbs)+L" "+name+L", dealing "+ std::to_wstring(result.first) +temp);
                            log.add(pname+L"'s MP reduces by "+std::to_wstring(manaCost)+L".");
                        }else{
                            log.add(pname+L" felt tired and couldn't concentrate on the spell.");
                        }
                        return true;
                    }
                }
                return false;
            }
            void enemyAttack(Entity* enEntity, int playerIndex){
                Enemy* enemy = enemies[enEntity];
                std::wstring playerName = scene[playerPos[playerIndex].x][playerPos[playerIndex].y]->name;
                Player* player = getPlayerByIndex(playerIndex);
                auto result = hurtPlayer(playerIndex, enemy->damage-player->defense);
                log.add(enEntity->name+L" hurt "+playerName+L" by "+std::to_wstring(result.first)+L" points.");
                if (result.second){
                    log.add(playerName+L" sacrificed in the battle.");
                }else if (player->health < player->maxHealth/3){
                    log.add(playerName+L" is badly wounded.");
                }
            }
            std::vector<std::wstring> meleeVerbs{L"slashed", L"struck", L"attacked", L"punched", L"hurt", L"wounded", L"bruised", L"injured", L"hit", L"bashed",L"smote"};
            std::vector<std::wstring> rangedVerbs{L"shot", L"blasted", L"whacked", L"battered", L"hurt", L"wounded", L"struck", L"injured", L"hit", L"pelted"};
            std::vector<std::wstring> magicVerbs{L"zapped", L"magicked", L"jinxed", L"hexed", L"cursed", L"burned", L"struck", L"blasted", L"hit", L"dazzled"};
        public:
            Dungeon(int width, int height){
                // make room for dungeon cells
                this->width = width;
                this->height = height;
                scene = new Entity**[height];
                for (int i=0;i<height;i++){
                    scene[i] = new Entity*[width];
                }
                // read some json
                loadTemplate();

                // add some wall
                createEnvironment();
                
                // add player
                spawnPlayer(8,7,L"Alex");
                spawnPlayer(3,4,L"Jenova");
                auto pactive = getPlayerByIndex(0);
                pactive->pickup(arrow);
                pactive->pickup(arrow);
                pactive->pickup(arrow);
                pactive->pickup(manapotion);
                pactive->pickup(healthpotion);
                pactive->pickup(scrapemetal);
                pactive->pickup(strawhat);
                pactive->pickup(brokenwheel);
                pactive->pickup(woodensword);

                pactive = getPlayerByIndex(1);
                pactive->pickup(arrow);
                pactive->pickup(arrow);
                pactive->pickup(arrow);
                pactive->pickup(manapotion);
                pactive->pickup(healthpotion);
                pactive->pickup(discardedcuirass);
                pactive->pickup(crackedshield);
                pactive->pickup(tyrostaff);
                pactive->pickup(makeshiftbow);
                
                //add enemy
                spawnEnemy(2,5,entityid::bat);
                spawnEnemy(8,8,entityid::crow);
                spawnEnemy(13,7,entityid::goblin);
                spawnEnemy(15,4,entityid::crow);
                spawnEnemy(15,13,entityid::bat);
            }
            ~Dungeon(){
                for(int i=0;i<height;i++){
                    for (int j=width;j<width;j++){
                        delete scene[i][j];
                    }
                    delete[] scene[i];
                }
                delete[] scene;
            }
            
    };
}

#endif
