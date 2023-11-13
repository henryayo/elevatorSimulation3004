#ifndef ELEVATORSTATE_H
#define ELEVATORSTATE_H

#include <QObject>
#include "elevator.h"
#include "ecs.h"

class ElevatorState
{
public:
    ElevatorState(Elevator*);
    virtual void moving(QString) = 0;
protected:
    Elevator* elevator;
    ECS* ecs;
};

#endif // ELEVATORSTATE_H
