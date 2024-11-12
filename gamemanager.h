#include<iostream>
#include<unistd.h>
#include<vector>
#include<sstream>
#include<utility>
#include"centerstring.h"
#include"dungeon.h"
#include"debug.h"
#include"loot.h"

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
namespace game{
    class GameManager{
        // UI and drawing
        // enemy AI
        public:
            Dungeon dungeon;
            v2di topleftCorner;
            int displaySize = 10;    // side of area that shows dungeon, unit is dungeon cell
            int deadzoneOffset = 2;  // camera not rigidly follow active player
            int enemyWaitTimeAfterMove = 500000;
            int selectionMode = false;
            int attackType;  // type of attack that enabled selection mode
            bool playerTurn = true;   // manually managed in game loop
            int playerTurnLeft[3];
            GameManager(int dwidth, int dheight):dungeon(dwidth,dheight){
                startNewPlayerTurn();
                adjustCamera(dungeon.playerPos[dungeon.activePlayerIndex],true);
            }
            
            void render(){
                if (gameover()){return;}  // oh, how many exception it triggers when we run out of players!
                // simply render a frame by printing
                system("clear");
                //std::wcout << L"frame " << frameCounter++ << std::endl;
                // upper edge
                if (dungeon.activePlayerIndex==0){
                    std::wcout << L"┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━"
                           << std::left << std::setw(27) << std::setfill(L'━') << wstoupper(dungeon.getPlayerName(0)) << L'┓' << std::endl;
                }else{
                    std::wcout << L"┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━"
                           << std::left << std::setw(27) << std::setfill(L'━') << dungeon.getPlayerName(0) << L'┓' << std::endl;
                }
                std::pair<std::wstring,std::wstring> dline;   // dline stands for dungeon line, just dungeon buffer
                std::pair<std::wstring,std::wstring> sidePanelBuffer;
                for (int i=0;i<displaySize;i++){
                    dline = getOneRowOfDungeon(i);
                    sidePanelBuffer = getSidePanel(i);
                    std::wcout << L'┃' <<  dline.first << sidePanelBuffer.first << std::endl;
                    std::wcout << L'┃' << dline.second << sidePanelBuffer.second << std::endl;
                }
                // last item in inventory
                dline = getOneRowOfDungeon(displaySize);
                std::wcout << L'┃' << dline.first;
                Player* ptemp = dungeon.getPlayerByIndex(dungeon.activePlayerIndex);
                std::wostringstream temp;
                if (ptemp->inventory.size() > 4){
                    entityid itemid = ptemp->getItemByIndex(ptemp->activeInvItemIndex+4);
                    temp << dungeon.loots[itemid].first << L" x" << ptemp->inventory[itemid];
                    std::wcout << L"e↓" << std::setfill(L' ') << std::internal << std::setw(29) << centered(temp.str()) << L"┃" << std::endl;
                }else{
                    std::wcout << L"e↓                             ┃" << std::endl;
                }
                std::wcout << L"┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫"<<std::endl;
                // event box
                std::wcout << L"┃"<< std::setfill(L' ') << std::left << std::setw(72) << log[0] << L"┃"<<std::endl;
                std::wcout << L"┃"<< std::setfill(L' ') << std::left << std::setw(72) << log[1] << L"┃"<<std::endl;
                std::wcout << L"┃"<< std::setfill(L' ') << std::left << std::setw(72) << log[2] << L"┃"<<std::endl;
                std::wcout << L"┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛"<<std::endl;

            }
            std::pair<std::wstring,std::wstring> getSidePanel(int i){
                // i is the i used in the forloop of render()
                // for details, see scheme.txt
                std::wostringstream l1;
                std::wostringstream l2;
                switch (i)
                {
                case 0:
                case 2:
                case 4:{
                    if (i/2 < dungeon.playerPos.size()){
                        Player* ptemp = dungeon.getPlayerByIndex(i/2);
                        std::wostringstream temp;
                        temp << L"┃ DEF " << ptemp->defense << L"(+" << ptemp->getDefenseBonus() << L")";
                        l1 << std::left << std::setw(16) << temp.str();
                        temp.str(std::wstring());
                        temp << L"melee  " << ptemp->meleeDamage <<  L"(+" << ptemp->meleeWeaponDamageBonus << L")";
                        l1 << std::left << std::setw(15) << temp.str();
                        l1 << L"┃";
                        temp.str(std::wstring());
                        temp << i/2+1 << L" HP " << ptemp->health << L" / " << ptemp->maxHealth;
                        l2 << std::left << std::setw(16) << temp.str();
                        temp.str(std::wstring());
                        temp << L"ranged " << ptemp->rangedDamage <<  L"(+" << ptemp->rangedWeaponDamageBonus << L")";
                        l2 << std::left << std::setw(15) << temp.str() << L"┃";
                        temp.str(std::wstring());
                    }else{
                        l1 << L"┃                              ┃";
                        l2 << i/2+1 << L"                              ┃";
                    }
                    break;
                }
                case 1:
                case 3:
                case 5:{
                    if ((i-1)/2 < dungeon.playerPos.size()){
                        Player* ptemp = dungeon.getPlayerByIndex((i-1)/2);
                        std::wostringstream temp;
                        temp << L"┃ MP " << ptemp->mana << L" / " << ptemp->maxMana;
                        l1 << std::left << std::setw(16) << temp.str();
                        temp.str(std::wstring());
                        temp << L"magic  " << ptemp->magicDamage <<  L"(+" << ptemp->magicWeaponDamageBonus << L")";
                        l1 << std::left << std::setw(15) << temp.str();
                        l1 << L"┃";
                        temp.str(std::wstring());
                    }else{
                        l1 << L"┃                              ┃";
                    }
                    
                    if (i==5){
                        Player* ptemp = dungeon.getPlayerByIndex(dungeon.activePlayerIndex);
                        std::wostringstream temp;
                        temp << L"┣━━━Item(" << ptemp->totalItemCount << L"/" << ptemp->inventoryLimit << L")";
                        l2 << std::left << std::setw(31) << std::setfill(L'━') <<  temp.str() << L'┫';
                    }else{
                        if ((i+1)/2 < dungeon.playerPos.size()){
                            std::wostringstream temp;
                            if ((i+1)/2 == dungeon.activePlayerIndex){
                                temp << L"┣━━━" << wstoupper(dungeon.getPlayerName((i+1)/2));
                            }else{
                                temp << L"┣━━━" << dungeon.getPlayerName((i+1)/2);
                            }
                            l2 << std::left << std::setw(31) << std::setfill(L'━') << temp.str() << L'┫';
                        }else{
                            l2 << L"┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫";
                        }
                    }
                    break;
                }
                case 6:{
                    Player* ptemp = dungeon.getPlayerByIndex(dungeon.activePlayerIndex);
                    std::wostringstream temp;
                    if (ptemp->inventory.size()>0){
                        entityid itemid = ptemp->getItemByIndex(ptemp->activeInvItemIndex);
                        temp << dungeon.loots[itemid].first << L" x" << ptemp->inventory[itemid];
                        l1 << L"q↑" << std::setw(29) << centered(temp.str()) << L"┃";
                    }else{
                        l1 << L"q↑                             ┃";
                    }
                    l2 << L"┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫";
                    break;}
                case 7:{
                    Player* ptemp = dungeon.getPlayerByIndex(dungeon.activePlayerIndex);
                    std::wostringstream temp;
                    if (ptemp->inventory.size()>1){
                        entityid itemid = ptemp->getItemByIndex(ptemp->activeInvItemIndex+1);
                        temp << dungeon.loots[itemid].first << L" x" << ptemp->inventory[itemid];
                        l1 << L"┃" << std::setw(30) << centered(temp.str()) << L"┃";
                    }else{
                        l1 << L"┃                              ┃";
                    }
                    l2 << L"┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫";
                    break;}
                case 8:{
                    Player* ptemp = dungeon.getPlayerByIndex(dungeon.activePlayerIndex);
                    std::wostringstream temp;
                    if (ptemp->inventory.size()>2){
                        entityid itemid = ptemp->getItemByIndex(ptemp->activeInvItemIndex+2);
                        temp << dungeon.loots[itemid].first << L" x" << ptemp->inventory[itemid];
                        l1 << L"i↕" << std::setw(29) << centered(temp.str()) << L"┃";
                    }else{
                        l1 << L"i↕                             ┃";
                    }
                    l2 << L"┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫";
                    break;}
                case 9:{
                    Player* ptemp = dungeon.getPlayerByIndex(dungeon.activePlayerIndex);
                    std::wostringstream temp;
                    if (ptemp->inventory.size()>3){
                        entityid itemid = ptemp->getItemByIndex(ptemp->activeInvItemIndex+3);
                        temp << dungeon.loots[itemid].first << L" x" << ptemp->inventory[itemid];
                        l1 << L"┃" << std::setw(30) << centered(temp.str()) << L"┃";
                    }else{
                        l1 << L"┃                              ┃";
                    }
                    l2 << L"┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫";
                    break;}
                default:
                    l1 << std::setw(32);
                    l2 << std::setw(32);
                    break;
                }
                return std::pair<std::wstring,std::wstring>(l1.str(),l2.str());
            }
            bool inDungeon(int i, int j){
                // small utility to check whether given position is in dungeon
                return (i>=0 && i< dungeon.height && j>=0 && j< dungeon.width);
            }
            std::pair<std::wstring,std::wstring> getOneRowOfDungeon(int row){
                // get one row of dungeon string for render()
                std::wstring l1(4*displaySize+1,L' ');
                std::wstring l2(4*displaySize+1,L' ');
                int i = topleftCorner.x+row;
                if (i >= dungeon.height || i < 0){
                    return std::pair<std::wstring,std::wstring>(l1,l2);
                }
                for (int j=0;j<displaySize+1;j++){        
                        // draw wall on left and top
                        bool haswall;
                        bool hasent;  // has entity on i,j
                        bool wallat1,wallat2,wallat4;
                        bool entat1, entat2, entat4; // entity, other than wall, at these positions
                        int column = j+topleftCorner.y;
                        if (inDungeon(i,column)){
                            haswall = dungeon.scene[i][column]->id == wall;
                            if (haswall){hasent = false;}else{hasent = dungeon.scene[i][column]->id != empty;}
                        }else{haswall = true; hasent = false;}
                        if (inDungeon(i-1,column)){
                            wallat2 = dungeon.scene[i-1][column]->id == wall;
                            if (wallat2){entat2 = false;}else{entat2 = dungeon.scene[i-1][column]->id != empty;}
                        }else{wallat2 = true; entat2 = false;}
                        if (inDungeon(i,column-1)){
                            wallat4 = dungeon.scene[i][column-1]->id == wall;
                            if (wallat4){entat4 = false;}else{entat4 = dungeon.scene[i][column-1]->id != empty;}
                        }else{wallat4 = true; entat4 = false;}
                        if (inDungeon(i-1,column-1)){
                            wallat1 = dungeon.scene[i-1][column-1]->id == wall;
                            if (wallat1){entat1 = false;}else{entat1 = dungeon.scene[i-1][column-1]->id != empty;}
                        }else{wallat1 = true; entat1 = false;}
                        if (j!=displaySize){
                            if (wallat2 && haswall){
                                l1[j*4+1] = L' ';
                                l1[j*4+2] = L' ';
                                l1[j*4+3] = L' ';
                            }else if(wallat2 || haswall){
                                l1[j*4+1] = L'━';
                                l1[j*4+2] = L'━';
                                l1[j*4+3] = L'━';
                            }else if (entat2 || hasent){
                                l1[j*4+1] = L'─';
                                l1[j*4+2] = L'─';
                                l1[j*4+3] = L'─';
                            }else{
                                l1[j*4+1] = L' ';
                                l1[j*4+2] = L' ';
                                l1[j*4+3] = L' ';
                            }
                        }
                        
                        if (wallat4 && haswall){
                            l2[j*4] = L' ';
                        }else if(wallat4 || haswall){
                            l2[j*4] = L'┃';
                        }else if (entat4 || hasent){
                            l2[j*4] = L'│';
                        }else{
                            l2[j*4] = L' ';
                        }
                        // or, use a string of thses chars of length 81. Much more painful to do that, though would run faster.
                        if (wallat1 && wallat2 && wallat4 && haswall){ // 4c4
                            l1[j*4] = L' ';
                        }else if (wallat2 && wallat4 && haswall){ // 4c3
                            l1[j*4] = L'┛';
                        }else if(wallat1 && wallat4 && haswall){
                            l1[j*4] = L'┗';
                        }else if(wallat1 && wallat2 && haswall){
                            l1[j*4] = L'┓';
                        }else if(wallat1 && wallat2 && wallat4){
                            l1[j*4] = L'┏';
                        }else if(wallat1 && wallat2 && (entat4 || hasent)){ // 4c2 * 2
                            l1[j*4] = L'┯';
                        }else if(wallat1 && wallat4 && (entat2 || hasent)){
                            l1[j*4] = L'┠';
                        }else if(wallat2 && haswall && (entat1 || entat4)){
                            l1[j*4] = L'┨';
                        }else if(wallat4 && haswall && (entat1 || entat2)){
                            l1[j*4] = L'┷';
                        }else if((wallat1 && haswall) || (wallat2 && wallat4)){
                            l1[j*4] = L'╋';
                        }else if ((wallat1 && wallat2) || (wallat4 && haswall)){
                            l1[j*4] = L'━';
                        }else if((wallat1 && wallat4) || (wallat2 && haswall)){
                            l1[j*4] = L'┃';
                        }else if (wallat1){  // 4c1 * 8
                            if (hasent || (entat2 && entat4)){
                                l1[j*4] = L'╃';
                            }else if (entat2){l1[j*4] = L'┹';}
                            else if (entat4){l1[j*4] = L'┩';}
                            else{l1[j*4] = L'┛';}
                        }else if (wallat2){
                            if (entat4 || (hasent && entat1)){
                                l1[j*4] = L'╄';
                            }else if (hasent){l1[j*4] = L'┡';}
                            else if (entat1){l1[j*4] = L'┺';}
                            else{l1[j*4] = L'┗';}
                        }else if (wallat4){
                            if (entat2 || (hasent && entat1)){
                                l1[j*4] = L'╅';
                            }else if (hasent){l1[j*4] = L'┱';}
                            else if (entat1){l1[j*4] = L'┪';}
                            else{l1[j*4] = L'┓';}
                        }else if (haswall){
                            if (entat1 || (entat2 && entat4)){
                                l1[j*4] = L'╆';
                            }else if (entat2){l1[j*4] = L'┢';}
                            else if (entat4){l1[j*4] = L'┲';}
                            else{l1[j*4] = L'┏';}
                        }else if ((hasent && entat1 && entat2)||(hasent && entat1 && entat4)||(entat4 && entat1 && entat2)||(hasent && entat4 && entat2)){ // four or three entities
                            l1[j*4] = L'┼';
                        }else if((hasent && entat1)||(entat2 && entat4)){ // two entities
                            l1[j*4] = L'┼';
                        }else if (hasent && entat2){
                            l1[j*4] = L'├';
                        }else if (entat1 && entat4){
                            l1[j*4] = L'┤';
                        }else if (hasent && entat4){
                            l1[j*4] = L'┬';
                        }else if (entat1 && entat2){
                            l1[j*4] = L'┴';
                        }else if(hasent){  // one entity
                            l1[j*4] = L'┌';
                        }else if(entat1){
                            l1[j*4] = L'┘';
                        }else if(entat2){
                            l1[j*4] = L'└';
                        }else if(entat4){
                            l1[j*4] = L'┐';
                        }else{ // all empty
                            l1[j*4] = L' ';
                        }
                        // draw entity acronym in this grid
                        if (j!=displaySize){
                            bool selected = selectionMode && dungeon.selectedPos == v2di{i,column};
                            std::wstring ac(L"   ");
                            if (hasent){
                                Entity* ent = dungeon.scene[i][column];
                                if (dungeon.enemies.count(ent)==1){
                                    ac = dungeon.enemies[ent]->acronym;
                                }else if (dungeon.players.count(ent)==1){
                                    ac = dungeon.players[ent]->acronym;
                                }else{  // must be in loots
                                    ac = dungeon.loots[ent->id].second->acronym;
                                }
                            }else if (!haswall){
                                ac[1] = L'·';
                            }
                            if (selected){
                                ac = wstoupper(ac);    // easiest way to show selected cell
                                if (!isalpha(ac[1])){
                                    ac[1] = L'+';
                                }
                            }
                            l2[4*j+1] = ac[0];
                            l2[4*j+2] = ac[1];
                            l2[4*j+3] = ac[2];
                        }
                        // rightmost wall
                        // if (j == dungeon.width-1){
                        //     int last = 4*j+4;
                        //     if (wallat2 && haswall){
                        //         l1[last] = L' ';
                        //     }else if (wallat2){
                        //         l1[last] = L'┓';
                        //     }else if (haswall){
                        //         l1[last] = L'┛';
                        //     }else{
                        //         l1[last] = L'┃';
                        //     }
                        //     if (haswall){
                        //         l2[last] = L' ';
                        //     }else{
                        //         l2[last] = L'┃';
                        //     }
                        // }
                    }
                    return std::pair<std::wstring,std::wstring>(l1,l2);
            }
            void adjustCamera(v2di pos, bool justActivated = false){
                if (justActivated){
                    // camera switch to active player/enemy
                    topleftCorner = pos - v2di(displaySize,displaySize)/2;
                }else{
                    // camera follow active player/enemy
                    v2di rp = pos - topleftCorner;   // rp stands for relative pos
                    v2di center = v2di(displaySize,displaySize)/2;
                    if (rp.x < center.x-deadzoneOffset){
                        topleftCorner -= v2di{1,0};
                    }
                    if (rp.x > center.x+deadzoneOffset-1){
                        topleftCorner += v2di{1,0};
                    }
                    if (rp.y < center.y-deadzoneOffset){
                        topleftCorner -= v2di{0,1};
                    }
                    if (rp.y > center.y+deadzoneOffset-1){
                        topleftCorner += v2di{0,1};
                    }
                }
            }
            void startNewPlayerTurn(){
                log.add(L"Player turn.");
                playerTurn = true;
                for (int i=0;i<dungeon.playerPos.size();i++){
                    playerTurnLeft[i] = dungeon.getPlayerByIndex(i)->speed;
                }
                adjustCamera(dungeon.playerPos[dungeon.activePlayerIndex],true);
                render();
            }
            void checkPlayerTurnEnd(){
                if (playerTurn){
                    for (int i=0;i<dungeon.playerPos.size();i++){
                        if (playerTurnLeft[i] != 0){
                            return;
                        }
                    }
                    log.add(L"Enemy turn.");
                    render();
                    usleep(enemyWaitTimeAfterMove);
                    playerTurn = false;
                }
            }
            bool handleInput(char input){ // return gameon
                switch (input)
                {
                case 27:  // esc
                    return false;
                case 'w':
                case 'a':
                case 's':
                case 'd':
                    if (selectionMode){ // moves selection
                        dungeon.moveSelection(input);
                    }else if (playerTurn){
                        if (playerTurnLeft[dungeon.activePlayerIndex]!=0){ //player moves
                            if(dungeon.movePlayer(input)){
                            	playerTurnLeft[dungeon.activePlayerIndex]--;
			    }
                            adjustCamera(dungeon.playerPos[dungeon.activePlayerIndex]);
                        }else{  // player cannot move, exhausted
                            v2di pp = dungeon.playerPos[dungeon.activePlayerIndex];
                            log.add(dungeon.scene[pp.x][pp.y]->name+L" cannot move in this turn.");
                        }
                    }
                    render();
                    break;
                case 'q':
                    dungeon.getPlayerByIndex(dungeon.activePlayerIndex)->lastItem();
                    render();
                    break;
                case 'e':
                    dungeon.getPlayerByIndex(dungeon.activePlayerIndex)->nextItem();
                    render();
                    break;
                case 'i':
                    dungeon.useItem();
                    render();
                    break;
                case '1':
                case '2':
                case '3':{
                    int in = input=='1'?0:(input=='2'?1:2);
                    if (dungeon.playerPos.size()>in){
                        dungeon.activePlayerIndex = in;
                    }else{break;}
                    if (playerTurn){
                        adjustCamera(dungeon.playerPos[dungeon.activePlayerIndex],true);
                        render();
                        break;
                    }
                    break;}
                case 'j':
                case 'k':
                case 'l':
                    if (selectionMode){
                        selectionMode = false;
                        if (attackType != input){
                            log.add(dungeon.getPlayerName(dungeon.activePlayerIndex)+L" gave up attack.");
                            render();
                            break;
                        }
                        if (playerTurnLeft[dungeon.activePlayerIndex]==0){
                            log.add(dungeon.getPlayerName(dungeon.activePlayerIndex)+L" cannot attack again in this turn.");
                            render();
                            break;
                        }else if (dungeon.playerAttack(attackType)){
                            playerTurnLeft[dungeon.activePlayerIndex]--;
                        }
                    }else{
                        selectionMode = true;
                        dungeon.select();
                        log.add(dungeon.getPlayerName(dungeon.activePlayerIndex)+L" is ready to attack.");
                        attackType = input;
                    }
                    render();
                    break;
                
                default:
                    checkPlayerTurnEnd();
                    break;
                }
                return true;
            }
            void nextPlayer(){
                // change active player
                dungeon.nextPlayer();
            }
            void moveAllEnemy(){
                // move all enemy
                for (auto v = dungeon.enemyPos.begin();v<dungeon.enemyPos.end();v++){
                    Entity* enEntity = dungeon.scene[v->x][v->y];
                    Enemy* e = dungeon.enemies[enEntity];
                    int movesLeft = e->speed; // how many times enemy can move dependes on its speed, same as players
                    // find out closest player to approach
                    

                    // focus on current enemy
                    adjustCamera(*v, true);
                    
                    // enemy take actions
                    while (movesLeft > 0 && dungeon.playerPos.size()>0){
                        // find closest player
                        double distanceToClosestPlayer = 0;
                        int closestPlayerIndex;
                        v2di closestPlayerPos;
                        for (int i=0;i<dungeon.playerPos.size();i++){
                            v2di v2 = dungeon.playerPos[i];  // i-th player pos
                            if (distanceToClosestPlayer==0){
                                distanceToClosestPlayer = std::sqrt((v->x-v2.x)*(v->x-v2.x)+(v->y-v2.y)*(v->y-v2.y));
                                closestPlayerIndex = i;
                                closestPlayerPos = v2;
                            }else{
                                double temp = std::sqrt((v->x-v2.x)*(v->x-v2.x)+(v->y-v2.y)*(v->y-v2.y));
                                if (temp<distanceToClosestPlayer){
                                    distanceToClosestPlayer = temp;
                                    closestPlayerIndex = i;
                                    closestPlayerPos = v2;
                                }
                            }
                        }

                        if (!e->alerted){ // alert enemy if haven't, takes one move to alert
                            if (distanceToClosestPlayer < 5){
                                e->alerted = true;
                                movesLeft--;  // takes a round
                                log.add(dungeon.getEnemyName(*v) + L" spotted " + dungeon.getPlayerName(closestPlayerIndex) + L".");
                                render();
                                continue;
                            }
                        }else{
                            // attack when adjacent to player
                            if (distanceToClosestPlayer<1.5){ 
                                dungeon.enemyAttack(enEntity, closestPlayerIndex);
                                movesLeft--;
                                render();
                                usleep(enemyWaitTimeAfterMove);
                                continue;
                            }
                            
                            // approach if possible
                            int dx=0;
                            if (v->x>closestPlayerPos.x){dx = -1;}else if (v->x<closestPlayerPos.x){dx = 1;}
                            if (dx!=0&&inDungeon(v->x+dx,v->y)){
                                if (dungeon.scene[v->x+dx][v->y]->id == empty){ // if have space
                                    v2di targetPos = *v+v2di(dx,0);
                                    dungeon.swapEntity(targetPos,*v);
                                    *v = targetPos;
                                    movesLeft--;
                                    render();
                                    usleep(enemyWaitTimeAfterMove);
                                    continue;
                                }
                            }
                            int dy=0;
                            if (v->y>closestPlayerPos.y){dy = -1;}else if (v->y<closestPlayerPos.y){dy = 1;}
                            if (dy!=0&&inDungeon(v->x,v->y+dy)){
                                if(dungeon.scene[v->x][v->y+dy]->id == empty){
                                    v2di targetPos = *v+v2di(0,dy);
                                    dungeon.swapEntity(targetPos,*v);
                                    *v = targetPos;
                                    movesLeft--;
                                    render();
                                    usleep(enemyWaitTimeAfterMove);
                                    continue;
                                }
                            }
                        }
                        
                        // not alerted or stuck, do nothing then
                        movesLeft--;
                    }
                    render();
                }
                startNewPlayerTurn();
                return;
            }
            int gameover(){
                if (dungeon.playerPos.size()==0){ //lose
                    return 2;
                }else if (dungeon.enemyPos.size()==0){ //win
                    return 1;
                }else{
                    return 0;
                }
            }
            int getscore(){
                return dungeon.score;
            }
    };
};

#endif
