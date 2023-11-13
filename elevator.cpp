#include "elevator.h"
#include "ecs.h"
#include "elevatorstate.h"
#include "regoperations.h"
#include "emergencyfire.h"
#include "poweroutage.h"

int Elevator::obstacleCount = 0;

Elevator::Elevator(int elevatorId, QObject *parent)
    : QObject(parent), elevatorId(elevatorId), idle(true), stopState(true),
      currFloor(1), isObstacle(false), holdDoorOpen(false), currWeight(0),
      currDirection("Going Up"), ecs(ECS::getECS()),
      closingTimer(new QTimer()), openingTimer(new QTimer()),
      regOperations(new RegOperations(this)),
      emergencyFire(new EmergencyFire(this)),
      powerOutage(new PowerOutage(this)), currState(regOperations) {}

Elevator::~Elevator() {
    delete closingTimer;
    delete openingTimer;
}

bool Elevator::isIdle() const { return idle; }
void Elevator::setIdle(bool idleState) { idle = idleState; }
int Elevator::getElevatorId() const { return elevatorId; }
int Elevator::getCurrentFloor() const { return currFloor; }
void Elevator::setCurrentFloor(int floor) { currFloor = floor; }
bool Elevator::getHoldDoorOpen() const { return holdDoorOpen; }
void Elevator::setHoldDoorOpen(bool open) { holdDoorOpen = open; }
bool Elevator::getStop() const { return stopState; }
void Elevator::setStop(bool stop) { stopState = stop; }
bool Elevator::getObstacle() const { return isObstacle; }
void Elevator::setObstacle() { if (stopState) isObstacle = true; }
void Elevator::removeObstacle() { isObstacle = false; }
QString Elevator::getCurrentDirection() const { return currDirection; }
void Elevator::setCurrentDirection(const QString &direction) { currDirection = direction; }
void Elevator::setCurrentWeight(double weight) { currWeight = weight; }
bool Elevator::getOverload() const { return currWeight > MAX_WEIGHT; }

void Elevator::moving(const QString &direction) {
    qDebug() << "Elevator moving";
    currState->moving(direction);
}

void Elevator::updateFloor(int floor) {
    setCurrentFloor(floor);
    qDebug() << "Elevator" << elevatorId << "updating to floor" << currFloor;
    emit displayFloor(floor, elevatorId);
}

void Elevator::doorObstacleHandling() {
    int elevatorId = this->elevatorId;
    obstacleCount++;
    if (obstacleCount >= 1) {
        QTimer::singleShot(2000, [this, elevatorId]() { emit showObstacleMessage(elevatorId); });
        obstacleCount = 0;
        return;
    }
    QTimer::singleShot(1500, [this, elevatorId]() { emit openDoor(elevatorId); });
    QTimer::singleShot(4000, [this, elevatorId]() { emit closeDoor(elevatorId); });
}

void Elevator::setClosingTimer(int time, int elevatorId) {
    connect(closingTimer, &QTimer::timeout, [this, elevatorId]() { emit closeDoor(elevatorId); });
    closingTimer->setSingleShot(true);
    closingTimer->start(time);
}

void Elevator::setOpeningTimer(int time, int elevatorId) {
    connect(openingTimer, &QTimer::timeout, [this, elevatorId]() { emit openDoor(elevatorId); });
    openingTimer->setSingleShot(true);
    openingTimer->start(time);
}

void Elevator::cancelClosingTimer() { closingTimer->stop(); }
void Elevator::cancelOpeningTimer() { openingTimer->stop(); }
void Elevator::setEmergencyFire() { currState = emergencyFire; }
void Elevator::setPowerOutage() { currState = powerOutage; }
void Elevator::setRegOperations() { currState = regOperations; }
