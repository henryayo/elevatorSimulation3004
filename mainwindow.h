#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QPushButton>

#include "ecs.h"
#include "elevator.h"
#include "emergencyfire.h"
#include "floor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void initECS();
    void setupButtonGroups();
    void connectSignals();
    void setupValidators();
    void setupHelpButtonConnections();
    void setupObstacleButtonConnections();
    void setupDoorButtonConnections();
    void setupWeightEditConnections();
    void requestElevatorUp(int);
    void requestElevatorDown(int);
    void displayFloor(int, int);
    void deactivateFloorButton(int, QString);
    void openDoor(int);
    void closeDoor(int);
    void sendFirstElevator(int);
    void sendSecondElevator(int);
    void deactivateFloorButton(int, int);
    void updateElevatorDisplay(int, const QString &);
    void emergencyCall(int);
    void doorObstacle(int);
    void showObstacleMessage(int);
    void pressOpenDoorButton(int);
    void pressCloseDoorButton(int);
    void activateOverload(int);
    void activateFireEmergency();
    void activatePowerOutage();
    void fireEmergencyHandling(int);
    void powerOutageHandling(int);
    void changeButtonColor(QButtonGroup *, int, const QString &);
    void handleElevatorRequest(int, int);

private:
    Ui::MainWindow *ui;
    ECS* ecs;
    QButtonGroup *elevator1Buttons;
    QButtonGroup *elevator2Buttons;
    QButtonGroup *upButtonGroup;
    QButtonGroup *downButtonGroup;
    void setupButtonGroup(QButtonGroup* buttonGroup, const QList<QPushButton*>& buttons, const char* slot);
};
#endif // MAINWINDOW_H
