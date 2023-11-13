#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QSignalMapper>
#include <QList>
#include <QTimer>
#include <QRegExpValidator>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    initECS();
    setupButtonGroups();
    connectSignals();
    setupValidators();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initECS() {
    ecs = ECS::getECS();
    for (int i = 0; i < ECS::numElevator; i++) {
        ecs->addElevator(new Elevator(i));
    }
    for (int i = 0; i < ECS::numFloor; i++) {
        ecs->addFloor(new Floor(i));
    }
}

void MainWindow::setupButtonGroups() {
    // Setup for Up Buttons
    upButtonGroup = new QButtonGroup(this);
    QList<QPushButton*> upButtons = {ui->floor1up, ui->floor2up, ui->floor3up};
    setupButtonGroup(upButtonGroup, upButtons, SLOT(requestElevatorUp(int)));

    // Setup for Down Buttons
    downButtonGroup = new QButtonGroup(this);
    QList<QPushButton*> downButtons = {ui->floor2down, ui->floor3down, ui->floor4down,};
    setupButtonGroup(downButtonGroup, downButtons, SLOT(requestElevatorDown(int)));

    // Setup for Elevator 1 Floor Buttons
    elevator1Buttons = new QButtonGroup(this);
    QList<QPushButton*> e1Buttons = {ui->elevator1f1, ui->elevator1f2, ui->elevator1f3, ui->elevator1f4};
    setupButtonGroup(elevator1Buttons, e1Buttons, SLOT(sendFirstElevator(int)));

    // Setup for Elevator 2 Floor Buttons
    elevator2Buttons = new QButtonGroup(this);
    QList<QPushButton*> e2Buttons = {ui->elevator2f1, ui->elevator2f2, ui->elevator2f3, ui->elevator2f4};
    setupButtonGroup(elevator2Buttons, e2Buttons, SLOT(sendSecondElevator(int)));
}

void MainWindow::setupButtonGroup(QButtonGroup* buttonGroup, const QList<QPushButton*>& buttons, const char* slot) {
    QSignalMapper* signalMapper = new QSignalMapper(this);
    for (int i = 0; i < buttons.size(); i++) {
        buttonGroup->addButton(buttons[i], i + 1);
        connect(buttons[i], SIGNAL(pressed()), signalMapper, SLOT(map()));
        signalMapper->setMapping(buttons[i], i + 1);
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, slot);
}


void MainWindow::connectSignals() {
    for (int i = 0; i < ECS::numElevator; i++) {
        Elevator* elevator = ecs->getElevator(i);
        connect(elevator, SIGNAL(displayFloor(int, int)), this, SLOT(displayFloor(int, int)));
        connect(elevator, SIGNAL(openDoor(int)), this, SLOT(openDoor(int)));
        connect(elevator, SIGNAL(closeDoor(int)), this, SLOT(closeDoor(int)));
        connect(elevator, SIGNAL(deactivateFloorButton(int, int)), this, SLOT(deactivateFloorButton(int, int)));
        connect(elevator, SIGNAL(showObstacleMessage(int)), this, SLOT(showObstacleMessage(int)));
        connect(elevator, SIGNAL(fireEmergencyHandling(int)), this, SLOT(fireEmergencyHandling(int)));
        connect(elevator, SIGNAL(powerOutageHandling(int)), this, SLOT(powerOutageHandling(int)));
    }
    for (int i = 0; i < ECS::numFloor; i++) {
        Floor* floor = ecs->getFloor(i);
        connect(floor, SIGNAL(deactivateFloorButton(int, QString)), this, SLOT(deactivateFloorButton(int, QString)));
    }

    setupHelpButtonConnections();
    setupObstacleButtonConnections();
    setupDoorButtonConnections();
    setupWeightEditConnections();
    connect(ui->fire_button, SIGNAL(pressed()), this, SLOT(activateFireEmergency()));
    connect(ui->powerout_button, SIGNAL(pressed()), this, SLOT(activatePowerOutage()));
}

void MainWindow::setupHelpButtonConnections() {
    QSignalMapper* helpButtonMapper = new QSignalMapper(this);
    connect(ui->elevator1help, SIGNAL(pressed()), helpButtonMapper, SLOT(map()));
    connect(ui->elevator2help, SIGNAL(pressed()), helpButtonMapper, SLOT(map()));
    helpButtonMapper->setMapping(ui->elevator1help, 0);
    helpButtonMapper->setMapping(ui->elevator2help, 1);
    connect(helpButtonMapper, SIGNAL(mapped(int)), this, SLOT(emergencyCall(int)));
}

void MainWindow::setupObstacleButtonConnections() {
    QSignalMapper* doorObstacleButtonMapper = new QSignalMapper(this);
    connect(ui->elevator1doorObstacle, SIGNAL(pressed()), doorObstacleButtonMapper, SLOT(map()));
    connect(ui->elevator2doorObstacle, SIGNAL(pressed()), doorObstacleButtonMapper, SLOT(map()));
    doorObstacleButtonMapper->setMapping(ui->elevator1doorObstacle, 0);
    doorObstacleButtonMapper->setMapping(ui->elevator2doorObstacle, 1);
    connect(doorObstacleButtonMapper, SIGNAL(mapped(int)), this, SLOT(doorObstacle(int)));
}

void MainWindow::setupDoorButtonConnections() {
    QSignalMapper* openButtonMapper = new QSignalMapper(this);
    connect(ui->elevatorOpen, SIGNAL(pressed()), openButtonMapper, SLOT(map()));
    connect(ui->elevator2Open, SIGNAL(pressed()), openButtonMapper, SLOT(map()));
    openButtonMapper->setMapping(ui->elevatorOpen, 0);
    openButtonMapper->setMapping(ui->elevator2Open, 1);
    connect(openButtonMapper, SIGNAL(mapped(int)), this, SLOT(pressOpenDoorButton(int)));

    QSignalMapper* closeButtonMapper = new QSignalMapper(this);
    connect(ui->elevatorClose, SIGNAL(pressed()), closeButtonMapper, SLOT(map()));
    connect(ui->elevator2Close, SIGNAL(pressed()), closeButtonMapper, SLOT(map()));
    closeButtonMapper->setMapping(ui->elevatorClose, 0);
    closeButtonMapper->setMapping(ui->elevator2Close, 1);
    connect(closeButtonMapper, SIGNAL(mapped(int)), this, SLOT(pressCloseDoorButton(int)));
}

void MainWindow::setupWeightEditConnections() {
    QSignalMapper* weightMapper = new QSignalMapper(this);
    connect(ui->elevator1Weight, SIGNAL(editingFinished()), weightMapper, SLOT(map()));
    connect(ui->elevator2Weight, SIGNAL(editingFinished()), weightMapper, SLOT(map()));
    weightMapper->setMapping(ui->elevator1Weight, 0);
    weightMapper->setMapping(ui->elevator2Weight, 1);
    connect(weightMapper, SIGNAL(mapped(int)), this, SLOT(activateOverload(int)));
    ui->elevator1Weight->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), ui->elevator1Weight));
    ui->elevator2Weight->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), ui->elevator2Weight));
}


void MainWindow::setupValidators() {
    QRegExpValidator* weightValidator = new QRegExpValidator(QRegExp("[0-9]*"), this);
    ui->elevator1Weight->setValidator(weightValidator);
    ui->elevator2Weight->setValidator(weightValidator);

    QSignalMapper* weightMapper = new QSignalMapper(this);
    connect(ui->elevator1Weight, SIGNAL(editingFinished()), weightMapper, SLOT(map()));
    connect(ui->elevator2Weight, SIGNAL(editingFinished()), weightMapper, SLOT(map()));
    weightMapper->setMapping(ui->elevator1Weight, 0);
    weightMapper->setMapping(ui->elevator2Weight, 1);
    connect(weightMapper, SIGNAL(mapped(int)), this, SLOT(activateOverload(int)));
}

void MainWindow::requestElevatorUp(int floorNumber) {
    qDebug("Up to floor %d", floorNumber);
    changeButtonColor(upButtonGroup, floorNumber, "#2cb323");
    ecs->getFloor(floorNumber)->sendRequest("Going Up");
}

void MainWindow::requestElevatorDown(int floorNumber) {
    qDebug("Down floor #%d", floorNumber);
    changeButtonColor(downButtonGroup, floorNumber, "#2cb323");
    ecs->getFloor(floorNumber - 1)->sendRequest("Going Down");
}

void MainWindow::sendFirstElevator(int floorId) {
    handleElevatorRequest(0, floorId);
}

void MainWindow::sendSecondElevator(int floorId) {
    handleElevatorRequest(1, floorId);
}

void MainWindow::displayFloor(int floor, int elevatorId) {
    QString displayText = "Current floor " + QString::number(floor);
    updateElevatorDisplay(elevatorId, displayText);
}

void MainWindow::deactivateFloorButton(int floor, QString direction) {
    QButtonGroup *group = (direction == "Going Up") ? upButtonGroup : downButtonGroup;
    changeButtonColor(group, floor, "#d6d6d6");
}

void MainWindow::deactivateFloorButton(int floor, int elevatorId){
    QButtonGroup *group = (elevatorId == 0) ? elevator1Buttons : elevator2Buttons;
    changeButtonColor(group, floor, "#d6d6d6");
}

void MainWindow::openDoor(int elevatorId) {
    updateElevatorDisplay(elevatorId, "Door Opening");
}

void MainWindow::closeDoor(int elevatorId) {
    if (!ecs->getElevator(elevatorId)->getHoldDoorOpen()) {
        updateElevatorDisplay(elevatorId, "Door Closing");

        if (!ecs->getElevator(elevatorId)->getOverload()) {
            if (!ecs->getElevator(elevatorId)->getObstacle()) {
                ecs->initMove(elevatorId);
            } else {
                qDebug("Close door, an obstacle has been detected");
                ecs->getElevator(elevatorId)->doorObstacleHandling();
            }
        } else {
            qDebug("Close door, overload detected");
        }
    }
}

void MainWindow::pressOpenDoorButton(int elevatorId) {
    QPushButton *button;
    switch (elevatorId) {
        case 0:
            button = ui->elevatorOpen;
            break;
        case 1:
            button = ui->elevator2Open;
            break;
        default:
            return; // Invalid elevator ID
    }

    if (!button->isChecked()) {
        ecs->getElevator(elevatorId)->setHoldDoorOpen(true);
        if (ecs->getElevator(elevatorId)->isIdle()) {
            openDoor(elevatorId);
        }
    } else {
        ecs->getElevator(elevatorId)->setHoldDoorOpen(false);
        closeDoor(elevatorId);
    }
}

void MainWindow::pressCloseDoorButton(int elevatorId) {
    if (!ecs->getElevator(elevatorId)->isIdle()) {
        ecs->getElevator(elevatorId)->cancelClosingTimer();
    }
    closeDoor(elevatorId);
}

void MainWindow::emergencyCall(int elevatorId) {
    QString displayText;
    switch (elevatorId) {
        case 0:
            displayText = "Calling the Building Safety Service...";
            QTimer::singleShot(5000, [this]() { ui->e1evator1Display->setText("Calling 911..."); });
            break;
        case 1:
            displayText = "Calling the Building Safety Service...";
            QTimer::singleShot(5000, [this]() { ui->e1evator2Display->setText("Calling 911..."); });
            break;
        default:
            displayText = "Invalid Elevator ID";
            break;
    }
    updateElevatorDisplay(elevatorId, displayText);
}

void MainWindow::doorObstacle(int elevatorId) {
    QString displayText;
    switch (elevatorId) {
        case 0:
            if (!ui->elevator1doorObstacle->isChecked()) {
                qDebug() << "Door obstacle detected in Elevator " << elevatorId;
                ecs->getElevator(elevatorId)->setObstacle();
                displayText = "Door Obstacle has been Detected";
            } else {
                ecs->getElevator(elevatorId)->removeObstacle();
                closeDoor(elevatorId);
                displayText = "Obstacle Removed";
            }
            break;
        case 1:
            if (!ui->elevator2doorObstacle->isChecked()) {
                qDebug() << "Door obstacle detected in Elevator " << elevatorId;
                ecs->getElevator(elevatorId)->setObstacle();
                displayText = "Door Obstacle has been Detected";
            } else {
                ecs->getElevator(elevatorId)->removeObstacle();
                closeDoor(elevatorId);
                displayText = "Obstacle Removed";
            }
            break;
        default:
            displayText = "Invalid Elevator ID";
            break;
    }
    updateElevatorDisplay(elevatorId, displayText);
}

void MainWindow::showObstacleMessage(int elevatorId) {
    QString message = "Remove the obstacle for the door to close.";
    updateElevatorDisplay(elevatorId, message);
}

void MainWindow::activateOverload(int elevatorId) {
    double weight = (elevatorId == 0) ? ui->elevator1Weight->text().toDouble() : ui->elevator2Weight->text().toDouble();
    ecs->getElevator(elevatorId)->setCurrentWeight(weight);

    if (ecs->getElevator(elevatorId)->getOverload()) {
        QString message = "The elevator is overloaded. Please reduce load for operations to resume.";
        updateElevatorDisplay(elevatorId, message);

        if (ecs->getElevator(elevatorId)->getStop() && !ecs->getElevator(elevatorId)->isIdle()) {
            ecs->getElevator(elevatorId)->cancelClosingTimer();
        }
    } else {
        updateElevatorDisplay(elevatorId, "");

        if (ecs->getElevator(elevatorId)->getStop() && !ecs->getElevator(elevatorId)->isIdle()) {
            ecs->getElevator(elevatorId)->setClosingTimer(1000, elevatorId);
        }
    }
}

void MainWindow::activateFireEmergency() {
    bool isActivated = ui->fire_button->isChecked();
    QString message = isActivated ? "There has been a fire outbreak.\n Please remain calm and exit the elevator as soon as it is on the floor" : "";
    updateElevatorDisplay(0, message);
    updateElevatorDisplay(1, message);

    qDebug() << "Fire button " << (isActivated ? "pressed" : "released");
    ecs->fireAlarm(isActivated);
}

void MainWindow::activatePowerOutage() {
    bool isActivated = ui->powerout_button->isChecked();
    QString message = isActivated ? "There has been a power outage.\n Please remain calm and exit the elevator as soon as it is on the floor" : "";
    updateElevatorDisplay(0, message);
    updateElevatorDisplay(1, message);

    qDebug() << "Power Out button " << (isActivated ? "pressed" : "released");
    ecs->powerOutage(isActivated);
}

void MainWindow::updateElevatorDisplay(int elevatorId, const QString& message) {
    switch (elevatorId) {
        case 0:
            ui->e1evator1Display->setText(message);
            break;
        case 1:
            ui->e1evator2Display->setText(message);
            break;
        default:
            break;
    }
}

void MainWindow::fireEmergencyHandling(int elevatorId) {
    //Floor 1 is the safe floor for fire emergency
    displayFloor(1, elevatorId);
    QString emergencyMessage = "There is a fire emergency.\n"
    "Elevator arriving at the safe floor. Please remain calm and exit the elevator on floor 1.";
    updateElevatorDisplay(elevatorId, emergencyMessage);
}

void MainWindow::powerOutageHandling(int elevatorId) {
    //Floor 1 is the safe floor for power outage
    displayFloor(1, elevatorId);
    QString emergencyMessage = "There is a power outage.\n"
    "Elevator arriving at the safe floor. Please remain calm and exit the elevator on floor 1.";
    updateElevatorDisplay(elevatorId, emergencyMessage);
}


void MainWindow::changeButtonColor(QButtonGroup *group, int buttonId, const QString &color) {
    QPalette p = group->button(buttonId)->palette();
    p.setColor(QPalette::Button, color);
    group->button(buttonId)->setPalette(p);
}

void MainWindow::handleElevatorRequest(int ei, int floorId) {
    qDebug("Elevator %d requested for floor #%d", ei, floorId);
    QButtonGroup *group = (ei == 0) ? elevator1Buttons : elevator2Buttons;
    changeButtonColor(group, floorId, "#2cb323");
    ecs->elevatorReqeust(ei, floorId);
}
