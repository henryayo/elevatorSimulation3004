#include "ecs.h"

Q_GLOBAL_STATIC(ECS, ECSInstance);

ECS::ECS(QObject *parent) : QObject(parent), isFireAlarm(false), isPowerOutage(false)
{}

ECS::~ECS(){
    for(int i = 0; i < elevators.size(); i++){
        delete elevators[i];
    }
    for(int i = 0; i < floors.size(); i++){
        delete floors[i];
    }
}

ECS* ECS::getECS(){
    return ECSInstance();
}


void ECS::addElevator(Elevator* elevator){
    elevators += elevator;
}

void ECS::addFloor(Floor* floor){
    floors += floor;
}

Elevator* ECS::getElevator(int elevatorId){
    return this->elevators[elevatorId];
}

Floor* ECS::getFloor(int floorId){
    return this->floors[floorId];
}

void ECS::floorRequest(int floorId, QString direction){
    qDebug() << "Floor " << floorId <<" is requesting a elevator" << direction;
    addFloorRequest(floorId, direction);
    if(!checkIfIdle()){
        moveIdle(floorId);
    }
}

void ECS::elevatorReqeust(int elevatorId, int floorId){
    addElevatorRequest(elevatorId, floorId);
    int currFloor = elevators[elevatorId]->getCurrentFloor() - 1;
    if(elevators[elevatorId]->isIdle()){
        elevators[elevatorId]->setIdle(false);
        if(currFloor < floorId){
            qDebug()<<"Elevator is vacant" << elevatorId;
            elevators[elevatorId]->moving("Up");
        }
        elevators[elevatorId]->moving("Down");
    }
}

bool ECS::checkIfIdle(){
    bool inUse = true;
    for(int i  = 0; i < numElevator; i++){
        if(elevators[i]->isIdle() && !elevators[i]->getObstacle() &&
                !elevators[i]->getHoldDoorOpen() && !elevators[i]->getOverload()){
            inUse = false;
        }
    }
    return inUse;
}

void ECS::moveIdle(int floorId){
    for(int i  = 1; i <= numElevator; i++){
        if(elevators[i]->isIdle() && !elevators[i]->getObstacle() &&
                !elevators[i]->getHoldDoorOpen() && !elevators[i]->getOverload()){
            qDebug() << "Elevator" << i << "resuming operations";
            elevators[i]->setIdle(false);
            int currFloor = elevators[i]->getCurrentFloor();
            if(currFloor < floorId){
                elevators[i]->moving("Up");
            }
            else{
                elevators[i]->moving("Down");
            }
            break;
        }
    }
}

void ECS::addFloorRequest(int floorId, QString direction){
    if(floorOrder.contains(floorId) && !floorOrder[floorId].contains(direction)){
        floorOrder[floorId] += direction;
    }
    else{
        QList<QString> directions;
        directions += direction;
        floorOrder[floorId] = directions;
    }
}

void ECS::addElevatorRequest(int elevatorId, int floorId){
    if(elevatorOrder.contains(elevatorId) && !elevatorOrder[elevatorId].contains(floorId)){
        elevatorOrder[elevatorId] += floorId;
    }
    else{
        QList<int> floors;
        floors += floorId;
        elevatorOrder[elevatorId] = floors;
    }
}

bool ECS::isElevatorRequest(int elevatorId, int floorId){
    bool requested = false;
    if(elevatorOrder.contains(elevatorId) && elevatorOrder[elevatorId].contains(floorId)){
        qDebug()<< "Elevator " << elevatorId<<" requested on floor " << floorId;
        requested = true;
        for(int i  = 0; i < elevatorOrder[elevatorId].size(); i++){
            if(elevatorOrder[elevatorId].at(i) == floorId){
                elevatorOrder[elevatorId].removeAt(i);
                break;
            }
        }
    }
    return requested;
}

bool ECS::isFloorRequest(int elevatorId, int floorId, QString direction){
    bool requested = false;
    if(floorOrder.contains(floorId) && floorOrder[floorId].size() > 0){
        qDebug()<< "Elevator " << elevatorId<<" requested on floor " << floorId;
        requested = true;
        QString dir = floorOrder[floorId].at(0);
        floorOrder[floorId].removeAt(0);
    }
    return requested;
}


void ECS::initMove(int elevatorId){
    bool isRequest = false;
    bool isEmergency = isFireAlarm || isPowerOutage;
    int currFloor = elevators[elevatorId]->getCurrentFloor() - 1;

    if(isEmergency){
        elevators[elevatorId]->moving("Going Down");
        return;
    }

    if(elevatorOrder[elevatorId].size() != 0){
        isRequest = true;
        elevators[elevatorId]->moving(elevatorDirection(elevatorId, currFloor));
    }

    if(!isRequest){
        QMapIterator<int, QList<QString>> i(floorOrder);
        while(i.hasNext()){
            i.next();
            if(!i.value().isEmpty()){
                if (i.key() > currFloor){
                    qDebug()<<"Upward floor request at " << i.key()<< " current floor "<< currFloor;
                    elevators[elevatorId]->moving("Going up");
                }
                else{
                    qDebug()<<"Downward floor request at " << i.key()<< "current floor " << currFloor;
                    elevators[elevatorId]->moving("Going down");
                }
                isRequest = true;
                break;
            }
        }
    }
    if(isRequest){
        elevators[elevatorId]->setIdle(true);
    }
}

QString ECS::elevatorDirection(int elevatorId, int currFloor){
    int index;
    int nearest = 1000000;
    for(int i  = 0; i < elevatorOrder[elevatorId].size(); i++){
        int result;
        if(elevatorOrder[elevatorId][i] - currFloor > 0){
            result = elevatorOrder[elevatorId][i] - currFloor;
        }
        else{
            result = currFloor - elevatorOrder[elevatorId][i];
        }
        if(result < nearest){
            nearest = result;
            index = i;
        }
    }
    if(elevatorOrder[elevatorId][index] > currFloor){
        return "Going Up";
    }
    else{
        return "Going Down";
    }
}

void ECS::fireAlarm(bool emergency){
    this->isFireAlarm = emergency;
    if(isFireAlarm){
        for(int i = 0; i < elevators.size(); i++){
            elevators[i]->setEmergencyFire();
            elevators[i]->moving("Going Down");
        }
    }
    else{
        for(int i = 0; i < elevators.size(); i++){
            elevators[i]->setIdle(true);
            elevators[i]->setRegOperations();
        }
    }
}

void ECS::powerOutage(bool emergency){
    this->isPowerOutage = emergency;
    if(isPowerOutage){
        for(int i = 0; i < elevators.size(); i++){
            elevators[i]->setPowerOutage();
            elevators[i]->moving("Going Down");
        }
    }
    else{
        for(int i = 0; i < elevators.size(); i++){
            elevators[i]->setIdle(true);
            elevators[i]->setRegOperations();
        }
    }
}
