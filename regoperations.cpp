#include "ecs.h"
#include <QDebug>
#include <QTimer>
#include "regoperations.h"

RegOperations::RegOperations(Elevator* elevator) : ElevatorState(elevator) {}

void RegOperations::moving(QString direction) {
    int floor = elevator->getCurrentFloor();
    int delay = 1;
    int elevatorId = elevator->getElevatorId();

    elevator->setCurrentDirection(direction);
    elevator->setStop(false);
    qDebug() << "Moving elevator" << elevatorId;

    while (true) {
        updateDirectionBasedOnFloor(floor);

        QTimer::singleShot(delay * 1000, [this, floor]() { elevator->updateFloor(floor); });
        delay++;

        if (elevator->getStop()) {
            handleElevatorStop(elevatorId, floor, delay);
            break;
        }
        floor = getNextFloor(floor, elevator->getCurrentDirection());
    }
}

void RegOperations::updateDirectionBasedOnFloor(int floor) {
    if (floor == 1) {
        elevator->setCurrentDirection("Up");
    } else if (floor == ECS::numFloor) {
        elevator->setCurrentDirection("Down");
    }
}

void RegOperations::handleElevatorStop(int elevatorId, int floor, int delay) {
    QTimer::singleShot(delay * 1000, [this, elevatorId, floor]() { emit elevator->deactivateFloorButton(elevatorId, floor); });
    elevator->setOpeningTimer(delay * 2000, elevatorId);
    elevator->setClosingTimer(delay * 10000, elevatorId);
    qDebug() << "Current floor" << floor << "STOPPING";
}

int RegOperations::getNextFloor(int currentFloor, const QString& direction) {
    if(direction == "Going Up") {
        return currentFloor + 1;
    }
    return currentFloor - 1;
}
