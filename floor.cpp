#include "ecs.h"
#include "floor.h"

Floor::Floor(int floorId, QObject *parent) : QObject(parent), floorId(floorId) {
    ecs = ECS::getECS();
}

void Floor::sendRequest(QString direction) {
    qDebug() << "Floor ID: " << floorId;
    ecs->floorRequest(floorId, direction);
}

int Floor::getFloorId() const {
    return floorId;
}

void Floor::serviceCompletion(QString direction, int delaySeconds) {
    int targetFloor = floorId + 1;
    QTimer::singleShot(delaySeconds * 1000, [this, targetFloor, direction]() {
        emit deactivateFloorButton(targetFloor, direction);
    });
}
