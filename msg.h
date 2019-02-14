#ifndef MSG_H
#define MSG_H

#include <QMap>

struct InitMSG {

};

struct MoveMSG {
    QString movement;
    int currentAP[2];
};

struct ActionMSG {

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
};

#endif // MSG_H
