#include<iostream>
#include<string>
#include<limits.h>
#include<fstream>
#include<cstdlib>
#include<map>
#include "gamemanager.h"
#include "debug.h"

int main(){
    std::cin.sync_with_stdio(false); // otherwise in_avail() is always 0
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());  // who knows, it just makes wcout work
    char input;
    bool gameon = true;
    // 20x20 dungeon
    game::GameManager gm(20,20);
    gm.render();
    while (gameon){
        gameon = gm.handleInput(input) && !gm.gameover();
        if (!gm.playerTurn){
            gm.moveAllEnemy();
        }
        input = '\0';
        system("/bin/stty raw"); // input without pressing enter, platform specific
        if (std::cin.rdbuf() and std::cin.rdbuf()->in_avail() > 0) {
            input = getchar();
        }
        system("/bin/stty cooked"); // output sensibly
    }
    std::wcout<< L"Game over!" << std::endl;
    if (gm.gameover()==1){
        std::wcout << "Congratulations! You defeated all enemies!"<<std::endl;
    }else if (gm.gameover()==2){
        std::wcout << "You lost all you heroes."<<std::endl;
    }
    std::wcout<< L"Your score: " << std::to_wstring(gm.getscore()) << std::endl;
    if (gm.gameover()!=0){
        std::wcout << "Leave your name: ";
        std::wstring name;
        std::wcin >> name;
        std::wofstream out("scoreboard.txt", std::ios::app);
        out << name << "    " << gm.getscore() << std::endl;
        out.close();
    }
    std::wcout<< L"Thanks for playing >_<" << std::endl;
    return 0;
}