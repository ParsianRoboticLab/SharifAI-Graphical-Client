#include "mainwindow.h"
#include <QApplication>
#include <stdio.h>
#include <QtGui>
#include <QApplication>
#include "soccerview.h"
#include "timer.h"
#include "msg.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
GLSoccerView *view;

bool runApp = true;

class MyThread : public QThread
{
protected:
    void run()
    {
        QFile serverLog("../../../../graphic.log"); serverLog.open(QIODevice::ReadOnly);
        QJsonDocument sl = QJsonDocument::fromJson(serverLog.readAll());

        if (sl.isEmpty() || sl.isNull()) qDebug() << "HEY2";
        while(runApp) {
            for (int i = 0; i < sl.array().size() ; i++) {
                QJsonObject obj = sl.array().at(i).toObject();
                QJsonObject arg = obj["args"].toArray().at(0).toObject();
                if (obj["name"].toString() == "init") {
                    InitMSG msg;
                    msg.map.col = arg["map"].toObject()["columnNum"].toInt();
                    msg.map.row = arg["map"].toObject()["rowNum"].toInt();
                    msg.map.cells = new Cell*[msg.map.col];
                    for (int i = 0; i < msg.map.col; i++) msg.map.cells[i] = new Cell[msg.map.row];
                    for (auto cell : arg["map"].toObject()["cells"].toArray().at(0).toArray()) {
                        Cell& c = msg.map.cells
                                [cell.toObject()["column"].toInt()]
                                [cell.toObject()["row"].toInt()];
                        c.col = cell.toObject()["column"].toInt();
                        c.row = cell.toObject()["row"].toInt();
                        c.type = NONE;
                        if (cell.toObject()["isWall"].toBool()) {
                            c.type = WALL;
                        } else if (cell.toObject()["isInFirstRespawnZone"].toBool()) {
                            c.type = RES1;
                        } else if (cell.toObject()["isInSecondRespawnZone"].toBool()) {
                            c.type = RES2;
                        } else if (cell.toObject()["isInObjectiveZone"].toBool()) {
                            c.type = ZONE;
                        } else {
                            c.type = NONE;
                        }

                    }
                    view->initStuff(msg);
                } else if (obj["name"].toString() == "move") {
                    MoveMSG msg;
                    msg.movement = arg["movements"].toString();
                    msg.currentAP[0] = arg["currentAP"].toArray().at(0).toInt();
                    msg.currentAP[1] = arg["currentAP"].toArray().at(1).toInt();

                    view->updateMove(msg);

                } else if (obj["name"].toString() == "action") {
                    ActionMSG msg;

                } else if (obj["name"].toString() == "status") {
                    StatusMSG msg;
                    msg.score[0] = arg["scores"].toArray().at(0).toInt();
                    msg.score[1] = arg["scores"].toArray().at(1).toInt();
                    for (int j = 0; j < 8; j++) {
                        QJsonObject o = arg["heroes"].toArray().at(j).toObject();
                        msg.heroes[j].id = o["id"].toInt();
                        msg.heroes[j].currentHP = o["currentHP"].toInt();
                        msg.heroes[j].remRespawnTime = o["remRespawnTime"].toInt();
                        for (int k = 0; k < 4; k++) {
                            QJsonObject oo = o["remainingCooldowns"].toObject();
                            msg.heroes[j].remCool[oo["ability"].toString()] = oo["num"].toInt();
                        }
                    }
                    view->updateStatus(msg);

                } else if (obj["name"].toString() == "pick") {
                } else if (obj["name"].toString() == "end") {
                } else {
                    qDebug() << obj["name"] << "IS UNKNWON";
                    continue;
                }

            }
        }
        serverLog.close();
    }

public:
    MyThread(QObject* parent = 0){}
    ~MyThread(){}
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    view = new GLSoccerView();
    view->show();
    MyThread thread;
    thread.start();
    int retVal = app.exec();
    runApp = false;
    thread.wait();
    return retVal;
}

