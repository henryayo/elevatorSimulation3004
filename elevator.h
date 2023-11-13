#ifndef ELEVATOR_H
#define ELEVATOR_H
#include <QObject>
#include <QTimer>

class ECS;
class ElevatorState;

class Elevator : public QObject
{
    Q_OBJECT
public:
    Elevator(int, QObject *parent=nullptr);
    ~Elevator();
    QTimer* openingTimer;
    QTimer* closingTimer;
    static const int MAX_WEIGHT = 245;

    int getElevatorId() const;
    int getCurrentFloor() const;
    bool getStop() const;
    void setStop(bool);
    bool isIdle() const;
    void setIdle(bool);
    void moving(const QString &);
    void setObstacle();
    bool getObstacle() const;
    void setCurrentWeight(double);
    bool getOverload() const;
    void doorObstacleHandling();
    void removeObstacle();
    bool getHoldDoorOpen() const;
    void setHoldDoorOpen(bool);
    void setOpeningTimer(int, int);
    void setClosingTimer(int, int);
    void cancelOpeningTimer();
    void cancelClosingTimer();
    QString getCurrentDirection() const;
    void setCurrentDirection(const QString &direction);
    void updateFloor(int);
    void setEmergencyFire();
    void setRegOperations();
    void setPowerOutage();
    void setCurrentFloor(int);


private:
    int elevatorId;
    bool idle;
    bool stopState;
    bool isObstacle;
    bool holdDoorOpen;
    int currFloor;
    double currWeight;
    static int obstacleCount;
    ECS* ecs;
    QString currDirection;
    ElevatorState *currState;
    ElevatorState *regOperations;
    ElevatorState *emergencyFire;
    ElevatorState *powerOutage;

signals:
    void displayFloor(int, int);
    void deactivateFloorButton(int, int);
    void openDoor(int);
    void closeDoor(int);
    void showObstacleMessage(int);
    void fireEmergencyHandling(int);
    void powerOutageHandling(int);
};

#endif // ELEVATOR_H
