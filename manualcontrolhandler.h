#ifndef MANUALCONTROLHANDLER_H
#define MANUALCONTROLHANDLER_H

#include <QQuickItem>
#include <QSettings>
#include <QTimer>
#include <QNetworkConfigurationManager>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QNetworkSession>

#include "mavlink/v1.0/common/mavlink.h"
#include "mavlink/v1.0/pixhawk/mavlink.h"
#include "px4_custom_mode.h"


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
    void setLongitude(int32_t l);
    void setLatitude(int32_t l);

    void set_mode_disarm();
    void set_mode_arm();

    void set_mode_return();
    void set_mode_manual();
    void set_mode_assist_altctl();
    void set_mode_assist_posctl();
    void set_mode_auto_mission();
    void set_mode_auto_loiter();
    void set_mode_auto_delivery();

    void setX(int x);
    void setY(int y);
    void setZ(int z);
    void setR(int r);
    void setButtons(uint16_t b);

    void connectServer();
    void disconnectServer();

    void startTimer();
    void stopTimer();

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

 //get this from mav serial port
    uint8_t system_id;//ID of the sending system
    uint8_t component_id;//ID of the sending component
    uint8_t target_system;//ID of receiving system. px4 by default is 1
    uint8_t target_component;//ID of receiving component. 0 for all component

    double lat; //correct latitude
    double lon; //correct longitude

    int16_t x;
    int16_t y;
    int16_t z;
    int16_t r;
    uint16_t buttons;

    //76 arm and disarm
    void send_command_long(uint16_t CMD_ID, uint8_t confirmation, float f1, float f2, float f3, float f4, float f5, float f6, float f7);
    //send x y z r and mode switch
    void send_manual_control();
};

#endif // MANUALCONTROLHANDLER_H
