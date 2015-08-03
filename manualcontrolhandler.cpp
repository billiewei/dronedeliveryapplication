#include "manualcontrolhandler.h"

ManualControlHandler::ManualControlHandler(QQuickItem* parent):
    QQuickItem(parent),
    m_hostAddress(""), m_status("run with server"),
    m_port(0), m_voltage(0),
    m_latitude(22.3341732), m_longitude(114.2644281)
{
    tcpSocket = new QTcpSocket(this);
    //status label has not been used yet.
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(read()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));


    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendControlCommand()));

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        setStatus("Opening network session.");
        networkSession->open();
    }

}

void ManualControlHandler::connectServer(){

    tcpSocket->abort();
    tcpSocket->connectToHost(hostAddress(),port());
    timer->start(250);//40Hz

}

void ManualControlHandler::disconnectServer(){
    timer->stop();
    tcpSocket->abort();
    setStatus("Disconnected");
}

void ManualControlHandler::sendModeCommand(int m){
    //M m M
    char buffer[4];
    sprintf(buffer,"M%dM",m);

    //stop timer and restart it again
    timer->stop();
    tcpSocket->write(buffer);
    timer->start(250); //40Hz

    //we can repeat to improve the quality
    //QTimer::singleShot(1000, tcpSocket, SLOT(write(buffer)));

}

void ManualControlHandler::sendArmCommand(bool armed){
    //A 0,1 A
    char buffer[4];
    sprintf(buffer,"A%dA",armed);
    tcpSocket->write(buffer);
}

void ManualControlHandler::sendControlCommand(){
    // X x Y y Z z R r
    char buffer[9];
    sprintf(buffer,"X%dY%dZ%dR%d", m_x, m_y, m_z, m_r);
    tcpSocket->write(buffer);
}

void ManualControlHandler::read(){
    QByteArray ba = tcpSocket->readAll();
    unsigned char *buffer;
    buffer = (unsigned char*)ba.data();
}

void ManualControlHandler::displayError(QAbstractSocket::SocketError socketError){
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        setStatus("The host was not found. ");
                  //Please check the host name and port settings
        break;
    case QAbstractSocket::ConnectionRefusedError:
        setStatus("The connection was refused by the peer");
                               // Make sure the fortune server is running
                               // and check that the host name and port settings are correct
        break;
    default:
        setStatus(tcpSocket->errorString());
    }
}

void ManualControlHandler::sessionOpened(){
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

    setStatus("running with UAV GCS server");
}

QString ManualControlHandler::hostAddress() const{
    return m_hostAddress;
}

QString ManualControlHandler::status() const{
    return m_status;
}

int ManualControlHandler::port() const{
    return m_port;
}

int ManualControlHandler::voltage() const{
    return m_voltage;
}

double ManualControlHandler::longitude() const{
    return m_longitude;
}

double ManualControlHandler::latitude() const{
    return m_latitude;
}

void ManualControlHandler::setX(double x){
    m_x = (int)x;
}

void ManualControlHandler::setY(double y){
    m_y = (int)y;
}

void ManualControlHandler::setZ(double z){
    m_z = (int)z;
}

void ManualControlHandler::setR(double r){
    m_r = (int)r;
}

void ManualControlHandler::setHostAddress(QString a){
    if(m_hostAddress.compare(a)){
        m_hostAddress = a;
        emit hostAddressChanged();
    }
}

void ManualControlHandler::setStatus(QString s){
    if(m_status.compare(s)){
        m_status = s;
        emit statusChanged();
    }
}

void ManualControlHandler::setPort(int p){
    if(m_port - p){
        m_port = p;
        emit portChanged();
    }
}

void ManualControlHandler::setVoltage(int v){
    if(m_voltage - v){
        m_voltage = v;
        emit voltageChanged();
    }
}

void ManualControlHandler::setLongitude(int l){
    m_longitude = l;
    emit longitudeChanged();
}

void ManualControlHandler::setLatitude(int l){
    m_latitude = l;
    emit latitudeChanged();
}
