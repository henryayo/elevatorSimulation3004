#include "emergencyfire.h"
#include "ecs.h"
#include <QTimer>
#include <QDebug>

EmergencyFire::EmergencyFire(Elevator* elevator) : ElevatorState(elevator) {}

void EmergencyFire::moving(QString direction) {
    int elevatorId = elevator->getElevatorId();
    int floor = elevator->getCurrentFloor();

    qDebug() << "Moving elevator" << elevatorId << "current floor:" << floor;

    elevator->setHoldDoorOpen(false);
    elevator->setStop(false);

    if (floor == ECS::safeFloor) {
        QTimer::singleShot(1000, [this, elevatorId]() { emit elevator->fireEmergencyHandling(elevatorId); });
        return;
    }

    int delay = 1;
    while (floor >= ECS::safeFloor) {
        QTimer::singleShot(delay * 1000, [this, floor]() { elevator->updateFloor(floor); fireHandling(floor); });
        floor--;
        delay++;
    }
}

void EmergencyFire::fireHandling(int floor) {
    if (floor == ECS::safeFloor) {
        int elevatorId = elevator->getElevatorId();
        QTimer::singleShot(1000, [this, elevatorId]() { emit elevator->fireEmergencyHandling(elevatorId); });
    }
}
