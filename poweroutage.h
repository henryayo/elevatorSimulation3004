#ifndef POWEROUTAGE_H
#define POWEROUTAGE_H
#include "elevatorstate.h"
#include <QTimer>
#include <QObject>

class PowerOutage : public ElevatorState
{
public:
    PowerOutage(Elevator*);
    void moving(QString) override;
private:
    void powerOutageHandling(int);//floor
};

#endif // POWEROUTAGE_H
