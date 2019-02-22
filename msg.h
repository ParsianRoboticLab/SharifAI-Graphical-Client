#ifndef MSG_H
#define MSG_H

#include <QMap>

enum CellType {
    NONE,
    WALL,
    ZONE,
    RES1,
    RES2
};

struct Cell {
    int row, col;
    CellType type;
};

struct Map {
    Cell** cells;
    int row,col;
};

struct InitMSG {
    Map map;
};

enum Direction {
    D_up,D_down,D_left,D_right,D_none
};


struct HeroPickMSG {
    int id;
    QString name;
    int row, col;
};

struct MoveMSG {
    HeroPickMSG heroes[2][4];
    int currentAP[2];
};

struct ActionMSG {

};


struct PickMSG {
    HeroPickMSG heroes[2][4];
};


struct HeroMSG {
    int id;
    int currentHP;
    int remRespawnTime;
    QMap<QString, int> remCool;

};

struct StatusMSG {
    HeroMSG heroes[8];
    int respwanedHeroID[8];
    int score[2];
    int turn;
};


#endif // MSG_H
