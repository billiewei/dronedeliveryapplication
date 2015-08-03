#ifndef MANUALCONTROLHANDLER_H
#define MANUALCONTROLHANDLER_H

#include <QQuickItem>
#include <QSettings>
#include <QTimer>
#include <QNetworkConfigurationManager>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QNetworkSession>

class ManualControlHandler : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString hostAddress READ hostAddress WRITE setHostAddress NOTIFY hostAddressChanged)
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(int voltage READ voltage WRITE setVoltage NOTIFY voltageChanged)
    Q_PROPERTY(double longitude READ longitude WRITE setLongitude NOTIFY longitudeChanged)
    Q_PROPERTY(double latitude READ latitude WRITE setLatitude NOTIFY latitudeChanged)
public:
    ManualControlHandler(QQuickItem* parent = 0);

    QString hostAddress() const;
    QString status() const;
    int port() const;
    int voltage() const;
    double longitude() const;
    double latitude() const;

signals:
    void hostAddressChanged();
    void statusChanged();
    void portChanged();
    void voltageChanged();
    void longitudeChanged();
    void latitudeChanged();

public slots:
    void setHostAddress(QString a);
    void setStatus(QString s);
    void setPort(int p);
    void setVoltage(int v);
    void setLongitude(int l);
    void setLatitude(int l);

    void setX(double x);
    void setY(double y);
    void setZ(double z);
    void setR(double r);

    void connectServer();
    void disconnectServer();

    void sendModeCommand(int m);
    void sendArmCommand(bool armed);
    void sendControlCommand();

    void read(); //read GPS and Battery voltage in one function

    void displayError(QAbstractSocket::SocketError socketError);
    void sessionOpened();

private:
    QTcpSocket *tcpSocket;
    QString currentFortune;
    quint16 blockSize;
    QNetworkSession *networkSession;

    QTimer *timer;

    QString m_hostAddress;
    QString m_status;
    int m_port;
    int m_voltage;
    double m_latitude;
    double m_longitude;
    int m_x;
    int m_y;
    int m_z;
    int m_r;

};

#endif // MANUALCONTROLHANDLER_H
