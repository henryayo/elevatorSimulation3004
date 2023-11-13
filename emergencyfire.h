#ifndef EMERGENCYFIRE_H
#define EMERGENCYFIRE_H

#include <QObject>
#include <QTimer>
#include "elevatorstate.h"

class EmergencyFire : public ElevatorState
{
public:
    EmergencyFire(Elevator*);
    void moving(QString) override;

private:
    void fireHandling(int);
};

#endif // EMERGENCYFIRE_H
