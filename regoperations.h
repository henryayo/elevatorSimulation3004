#ifndef REGOPERATIONS_H
#define REGOPERATIONS_H

#include <QObject>
#include "elevatorstate.h"

class RegOperations : public ElevatorState {
public:
    RegOperations(Elevator*);
    void moving(QString) override;

private:
    void updateDirectionBasedOnFloor(int);
    void handleElevatorStop(int, int, int);
    int getNextFloor(int, const QString&);
};

#endif // REGOPERATIONS_H
