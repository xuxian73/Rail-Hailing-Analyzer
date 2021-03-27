#ifndef JSCOMMUNICATOR_H
#define JSCOMMUNICATOR_H

#include <QtWebChannel>
class jsCommunicator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString type MEMBER type )
    Q_PROPERTY(QString lat MEMBER lat )
    Q_PROPERTY(QString lng MEMBER lng )
    Q_PROPERTY(QJsonObject jsonData MEMBER jsonData NOTIFY dataChange)
signals:
    void jsChangeMarker(int type, double lat, double lng);

    void dataChange(const QJsonObject& jsonData);
public:
    QString type;
    QString lat;
    QString lng;
    QJsonObject jsonData;
public:
    jsCommunicator();

public slots:
    void jschangemarker();
};

#endif // JSCOMMUNICATOR_H
