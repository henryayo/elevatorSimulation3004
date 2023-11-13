#ifndef ECS_H
#define ECS_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QList>
#include <QMap>
#include "elevator.h"
#include "floor.h"

class Elevator;
class Floor;

class ECS : public QObject
{
    Q_OBJECT
public:
    ECS(QObject *parent = nullptr);
    ~ECS();
    ECS(ECS &other) = delete;
    static const int numFloor = 4;
    static const int numElevator = 2;
    static const int safeFloor = 1;

    static ECS* getECS();
    void addElevator(Elevator*);
    void addFloor(Floor*);
    void elevatorReqeust(int, int);
    void floorRequest(int, QString);
    Elevator* getElevator(int);
    Floor* getFloor(int);
    void initMove(int);
    void fireAlarm(bool);
    void powerOutage(bool);

private:
    QList<Elevator*> elevators;
    QList<Floor*> floors;
    QMap<int, QList<int>> elevatorOrder;
    QMap<int, QList<QString>> floorOrder;
    bool isFireAlarm;
    bool isPowerOutage;
    bool checkIfIdle();
    void moveIdle(int);
    void addElevatorRequest(int, int);
    void addFloorRequest(int, QString);
    bool isElevatorRequest(int, int);
    bool isFloorRequest(int, int, QString);
    QString elevatorDirection(int, int);

};

#endif // ECS_H
