#include "manualcontrolhandler.h"
#include "dearbuttons.h"

ManualControlHandler::ManualControlHandler(QQuickItem* parent):
    QQuickItem(parent),system_id(0),component_id(0),
    target_system(1),target_component(0),
    x(0), y(0), z(0), r(0),
    buttons(BUTTON_MANUAL),
    m_hostAddress(""), m_status("run with server"),
    m_port(0), m_voltage(0),
    m_latitude(22.3341732), m_longitude(114.2644281)
{
    tcpSocket = new QTcpSocket(this);
    //status label has not been used yet.
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(read()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));


    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(send_manual_control()));

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
    setStatus("Connected to server");
    timer->start(500);//20Hz

}

void ManualControlHandler::disconnectServer(){
    timer->stop();
    tcpSocket->abort();
    setStatus("Disconnected");
}

void ManualControlHandler::startTimer(){
    timer->stop();
    timer->start(500);
    //the correct procedure should be
}

void ManualControlHandler::stopTimer(){
    timer->stop();
}




void ManualControlHandler::read(){
    QByteArray ba = tcpSocket->readAll();
    unsigned char *buffer;
    buffer = (unsigned char*)ba.data();
    qDebug() << ba;



    //decode
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

void ManualControlHandler::setX(int x){
    x = (int16_t)x;
}

void ManualControlHandler::setY(int y){
    y = (int16_t)y;
}

void ManualControlHandler::setZ(int z){
    z = (int16_t)z;
}

void ManualControlHandler::setR(int r){
    r = (int16_t)r;
}

void ManualControlHandler::setButtons(uint16_t b){
    buttons = b;
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

void ManualControlHandler::setLongitude(int32_t l){
    m_longitude = l / 1000000.0;
    emit longitudeChanged();
}

void ManualControlHandler::setLatitude(int32_t l){
    m_latitude = l / 1000000.0;
    emit latitudeChanged();
}

void ManualControlHandler::set_mode_disarm(){
    setButtons(BUTTON_ALLZERO);
    send_command_long(MAV_CMD_DO_SET_MODE,0,MAV_MODE_MANUAL_DISARMED,0,0,0,0,0,0);
    qDebug() << "MODE_MODE_MANUAL_DISARMED";
}

void ManualControlHandler::set_mode_arm(){
    setButtons(BUTTON_ALLZERO);
    send_command_long(MAV_CMD_DO_SET_MODE,0,MAV_MODE_MANUAL_ARMED,0,0,0,0,0,0);
    qDebug() << "MODE_MODE_MANUAL_ARMED";
}

void ManualControlHandler::set_mode_return(){
    setButtons(BUTTON_RETURN);
    qDebug() << "BUTTON_RETURN";
}

void ManualControlHandler::set_mode_auto_delivery(){
    setButtons(BUTTON_AUTO_DELIVERY);
    qDebug() << "BUTTON_RETURN";
}

void ManualControlHandler::set_mode_manual(){
    setButtons(BUTTON_MANUAL);
   // qDebug() << "BUTTON_MANUAL";
}

void ManualControlHandler::set_mode_assist_altctl(){
    setButtons(BUTTON_ASSIST_ALTCTL);
    // qDebug() << "BUTTON_ASSIST_ALTCTL";
}

void ManualControlHandler::set_mode_assist_posctl(){
    setButtons(BUTTON_ASSIST_POSCTL);
   // qDebug() << "BUTTON_ASSIST_POSCTL";
}

void ManualControlHandler::set_mode_auto_mission(){
    setButtons(BUTTON_AUTO_MISSION);
   // qDebug() << "BUTTON_AUTO_MISSION";
}

void ManualControlHandler::set_mode_auto_loiter(){
    setButtons(BUTTON_AUTO_LOITER);
    qDebug() << "BUTTON_AUTO_LOITER";
}

//69
void ManualControlHandler::send_manual_control(){
    mavlink_message_t msg;
    uint8_t buffer[2048];
     mavlink_msg_manual_control_pack(0, 0, &msg, target_system, x, y, z, r, buttons);
    int size = mavlink_msg_to_send_buffer(buffer, &msg);
    QByteArray ba((char*)buffer,size);
    tcpSocket->write(ba);
}

//76
void ManualControlHandler::send_command_long(uint16_t CMD_ID, uint8_t confirmation, float f1, float f2, float f3, float f4, float f5, float f6, float f7){
    mavlink_message_t msg;
    uint8_t buffer[2048];

    mavlink_msg_command_long_pack(system_id, component_id, &msg, target_system, target_component,
                                  CMD_ID, confirmation, f1, f2, f3, f4, f5, f6, f7);
    int size = mavlink_msg_to_send_buffer(buffer, &msg);
    QByteArray ba((char*)buffer,size);
    tcpSocket->write(ba);
}
