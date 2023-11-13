#ifndef FLOOR_H
#define FLOOR_H

#include <QString>
#include <QDebug>
#include <QObject>
#include <QTimer>

class ECS;

class Floor: public QObject
{
    Q_OBJECT
public:
    Floor(int fid, QObject *parent=nullptr);
    void sendRequest(QString);
    int getFloorId() const;
    void serviceCompletion(QString, int);

private:
    ECS* ecs;
    int floorId;

signals:
    void deactivateFloorButton(int, QString);
};

#endif // FLOOR_H
