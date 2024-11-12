# DungeonGame
![286532313-4a82c305-3802-4337-bd82-478b49bc37f4](https://github.com/KLYang1412/COMP2113-Group107-/assets/56218754/22efd8e2-6ff5-4375-9fcd-1aa1ccf62223)

**Demonstration on the game:**
https://www.youtube.com/watch?v=0GJ7XbI6fBE


my setup:

linux 5.15.0-88-generic

g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0

terminal font : Nimbus Mono PS, size 12

please change your font until the grids line up

### build

only works on linux

`make -B main`

### Story

Alex and Jenova entered the cave of monsters by accident. Can they make it out alive?

Turn-based combat, you have to defeat all the monsters to win. If everyone perish you lose the game.

### Tips:
- Each character can have multiple actions in a turn. When every character cannot move anymore, the next turn starts. Our game can hold 3 characters at most.
- Equipments can be used to gain additional damage or get less damage inflicted in combat. If a new equipment is used when you have a better one equipped, the old, better one is lost, so be careful!
- Your character dies when at zero HP (health point)
- To shoot an arrow, the character needs an arrow
- Magic attack costs MP (magic point), so it may fail at low MP. Increase your MP by drinking potions.
- Currently selected target/character have its name **bolded**

You can find the map of the cave in a png file around here. 

### Controls

- move around : <kbd>w</kbd>, <kbd>a</kbd>, <kbd>s</kbd>, <kbd>d</kbd>
- attack : <kbd>j</kbd>/<kbd>k</kbd>/<kbd>l</kbd> to enter selection mode, then press again the confirm action. <kbd>j</kbd>,<kbd>k</kbd>,<kbd>l</kbd> correspond to melee, ranged and magic attack respectively
- exit game : <kbd>Esc</kbd>
- cycle through items : <kbd>q</kbd><kbd>e</kbd>
- use the first (not the third) item in the inventory : <kbd>i</kbd>
- switch to another hero : <kbd>1</kbd><kbd>2</kbd><kbd>3</kbd> (3 heroes at most)

Do not press weird stuff (like <kbd>↑</kbd> <kbd>↓</kbd>) as it breaks the game!

### Features
- many ways of hurting your enemy, randomly
- carefully chosen data structure to maintain game status
- dynamically allocated (and deleted) dungeon
- flexible game difficulty through json files
- leave your name to scoreboard.txt after a completed game session (arcade-style)
- easy-to-read source code (many comments, pertinent variable names)

### Modding
- Make your own level using Dungeon's constructor!
- Extend the game by adding more content in the json files!
