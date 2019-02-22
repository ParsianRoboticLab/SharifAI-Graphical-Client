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
        qDebug() << QDir::currentPath();
        QFile serverLog("../../../log.json"); serverLog.open(QIODevice::ReadOnly);
        QJsonDocument sl = QJsonDocument::fromJson(serverLog.readAll());
        bool done = false;
        if (sl.isEmpty() || sl.isNull()) qDebug() << "HEY2";
        while(runApp) {
            int turn = 0;
            QJsonObject arg = sl.object()["init"].toObject();
            InitMSG msg;
            msg.map.col = arg["map"].toObject()["columnNum"].toInt();
            msg.map.row = arg["map"].toObject()["rowNum"].toInt();
            msg.map.cells = new Cell*[msg.map.col];
            for (int i = 0; i < msg.map.col; i++) msg.map.cells[i] = new Cell[msg.map.row];
            for (auto cell : arg["map"].toObject()["cells"].toArray()) {
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

            MoveMSG mmsg;

            mmsg.currentAP[0] = arg["our"].toObject()["AP"].toInt();
            mmsg.currentAP[1] = arg["opp"].toObject()["AP"].toInt();
            for (const auto& h : arg["opp"].toObject()["heroes"].toArray().at(0).toArray()) {
                mmsg.heroes[0][h.toObject()["id"].toInt()].id = h.toObject()["id"].toInt();
                mmsg.heroes[0][h.toObject()["id"].toInt()].col = h.toObject()["lastVisiblePos-col"].toInt();
                mmsg.heroes[0][h.toObject()["id"].toInt()].row = h.toObject()["lastVisiblePos-row"].toInt();
            }
            for (const auto& h : arg["our"].toObject()["heroes"].toArray().at(0).toArray()) {
                mmsg.heroes[1][h.toObject()["id"].toInt()].id = h.toObject()["id"].toInt();
                mmsg.heroes[1][h.toObject()["id"].toInt()].col = h.toObject()["pos-col"].toInt();
                mmsg.heroes[1][h.toObject()["id"].toInt()].row = h.toObject()["pos-row"].toInt();
            }
            view->updateMove(mmsg);

            QJsonArray data = sl.object()["msg"].toArray();
            for (const auto& datum : data) {
                MoveMSG mmsg;

                mmsg.currentAP[0] = datum.toObject()["our"].toObject()["AP"].toInt();
                mmsg.currentAP[1] = datum.toObject()["opp"].toObject()["AP"].toInt();
//                for (const auto& h : datum.toObject()["opp"].toArray().at(0).toObject()["heroes"].toArray().at(0).toArray()) {
//                    mmsg.heroes[0][h.toObject()["id"].toInt()].id = h.toObject()["id"].toInt();
//                    mmsg.heroes[0][h.toObject()["id"].toInt()].col = h.toObject()["lastVisiblePos-col"].toInt();
//                    mmsg.heroes[0][h.toObject()["id"].toInt()].row = h.toObject()["lastVisiblePos-row"].toInt();
//                }
                for (const auto& h : datum.toObject()["our"].toArray().at(0).toObject()["heroes"].toArray().at(0).toArray()) {
                    mmsg.heroes[1][h.toObject()["id"].toInt()].id = h.toObject()["id"].toInt();
                    mmsg.heroes[1][h.toObject()["id"].toInt()].col = h.toObject()["pos-col"].toInt();
                    mmsg.heroes[1][h.toObject()["id"].toInt()].row = h.toObject()["pos-row"].toInt();
                }
                view->updateMove(mmsg);
                msleep(100);
            }
            //            for (int i = 0; i < sl.array().size() ; i++) {
            //                QJsonObject obj = sl.array().at(i).toObject();
            //                QJsonObject arg = obj["args"].toArray().at(0).toObject();
            //                if (obj["name"].toString() == "init") {
            ////                    if (done) continue;
            ////                    done = true;
            //                    InitMSG msg;
            //                    msg.map.col = arg["map"].toObject()["columnNum"].toInt();
            //                    msg.map.row = arg["map"].toObject()["rowNum"].toInt();
            //                    msg.map.cells = new Cell*[msg.map.col];
            //                    for (int i = 0; i < msg.map.col; i++) msg.map.cells[i] = new Cell[msg.map.row];
            //                    for (auto row : arg["map"].toObject()["cells"].toArray()) {
            //                        for (auto cell : row.toArray()) {
            //                            Cell& c = msg.map.cells
            //                                    [cell.toObject()["column"].toInt()]
            //                                    [cell.toObject()["row"].toInt()];
            //                            c.col = cell.toObject()["column"].toInt();
            //                            c.row = cell.toObject()["row"].toInt();
            //                            c.type = NONE;
            //                            if (cell.toObject()["isWall"].toBool()) {
            //                                c.type = WALL;
            //                            } else if (cell.toObject()["isInFirstRespawnZone"].toBool()) {
            //                                c.type = RES1;
            //                            } else if (cell.toObject()["isInSecondRespawnZone"].toBool()) {
            //                                c.type = RES2;
            //                            } else if (cell.toObject()["isInObjectiveZone"].toBool()) {
            //                                c.type = ZONE;
            //                            } else {
            //                                c.type = NONE;
            //                            }
            //                        }
            //                    }
            //                    view->initStuff(msg);
            //                } else if (obj["name"].toString() == "move") {
            //                    MoveMSG msg;
            //                    QString str = arg["movements"].toString();
            //                    int cnt = 0;
            //                    for(auto& s : str) {
            //                        switch (s.toLatin1()) {
            //                        case 'u':
            //                            msg.movement[cnt] = D_up;
            //                            break;
            //                        case 'd':
            //                            msg.movement[cnt] = D_down;
            //                            break;
            //                        case 'l':
            //                            msg.movement[cnt] = D_left;
            //                            break;
            //                        case 'r':
            //                            msg.movement[cnt] = D_right;
            //                            break;
            //                        case 'n':
            //                            msg.movement[cnt] = D_none;
            //                            break;
            //                        default:
            //                            msg.movement[cnt] = D_none;
            //                            break;
            //                        }
            //                        cnt += 1;
            //                    }
            //                    msg.currentAP[0] = arg["currentAP"].toArray().at(0).toInt();
            //                    msg.currentAP[1] = arg["currentAP"].toArray().at(1).toInt();

            //                    view->updateMove(msg);
            //                    msleep(100);

            //                } else if (obj["name"].toString() == "action") {
            //                    ActionMSG msg;

            //                } else if (obj["name"].toString() == "status") {
            //                    turn++;
            //                    StatusMSG msg;
            //                    msg.turn = turn;
            //                    msg.score[0] = arg["scores"].toArray().at(0).toInt();
            //                    msg.score[1] = arg["scores"].toArray().at(1).toInt();
            //                    for (int j = 0; j < 8; j++) {
            //                        QJsonObject o = arg["heroes"].toArray().at(j).toObject();
            //                        msg.heroes[j].id = o["id"].toInt();
            //                        msg.heroes[j].currentHP = o["currentHP"].toInt();
            //                        msg.heroes[j].remRespawnTime = o["remRespawnTime"].toInt();
            //                        for (int k = 0; k < 4; k++) {
            //                            QJsonObject oo = o["remainingCooldowns"].toObject();
            //                            msg.heroes[j].remCool[oo["ability"].toString()] = oo["num"].toInt();
            //                        }
            //                    }
            //                    view->updateStatus(msg);

            //                } else if (obj["name"].toString() == "pick") {
            //                    PickMSG msg;
            //                    for (int i = 0; i < arg["heroes"].toArray().size(); i++) {
            //                        for (int j = 0; j < arg["heroes"].toArray().at(i).toArray().size(); j++) {
            //                            QJsonObject obj = arg["heroes"].toArray().at(i).toArray().at(j).toObject();
            //                            msg.heroes[i][j].id = obj["id"].toInt();
            //                            msg.heroes[i][j].name = obj["name"].toString();
            //                            msg.heroes[i][j].col = obj["column"].toInt();
            //                            msg.heroes[i][j].row = obj["row"].toInt();
            //                        }
            //                    }
            //                    view->initHero(msg);
            //                } else if (obj["name"].toString() == "end") {
            //                    runApp = false;
            //                } else {
            //                    qDebug() << obj["name"] << "IS UNKNWON";
            //                    continue;
            //                }

            //            }
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

