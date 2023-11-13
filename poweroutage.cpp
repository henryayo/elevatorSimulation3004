#include "poweroutage.h"
#include "ecs.h"
#include <QTimer>
#include <QDebug>

PowerOutage::PowerOutage(Elevator* elevator) : ElevatorState(elevator) {}

void PowerOutage::moving(QString direction) {
    int elevatorId = elevator->getElevatorId();
    int floor = elevator->getCurrentFloor();

    qDebug() << "Moving elevator" << elevatorId << "current floor:" << floor;

    elevator->setHoldDoorOpen(false);
    elevator->setStop(false);

    if (floor == ECS::safeFloor) {
        QTimer::singleShot(1000, [this, elevatorId]() { emit elevator->powerOutageHandling(elevatorId); });
        return;
    }

    int delay = 1;
    while (floor >= ECS::safeFloor) {
        QTimer::singleShot(delay * 1000, [this, floor]() { elevator->updateFloor(floor); powerOutageHandling(floor); });
        floor--;
        delay++;
    }
}

void PowerOutage::powerOutageHandling(int floor) {
    if (floor == ECS::safeFloor) {
        int elevatorId = elevator->getElevatorId();
        QTimer::singleShot(1000, [this, elevatorId]() { emit elevator->powerOutageHandling(elevatorId); });
    }
}
