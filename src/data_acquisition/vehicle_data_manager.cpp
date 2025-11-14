#include "data_acquisition/vehicle_data_manager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QTimer>

VehicleDataManager::VehicleDataManager(QObject* parent)
    : QObject(parent)
    , current_speed_(0.0)
    , is_connected_(false)
    , retry_count_(0)
{
    connect(&websocket_, &QWebSocket::connected,
            this, &VehicleDataManager::onConnected);
    connect(&websocket_, &QWebSocket::disconnected,
            this, &VehicleDataManager::onDisconnected);
    connect(&websocket_, &QWebSocket::textMessageReceived,
            this, &VehicleDataManager::onTextMessageReceived);
    connect(&websocket_, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this, &VehicleDataManager::onError);
}

VehicleDataManager::~VehicleDataManager() {
    shutdown();
}

bool VehicleDataManager::initialize(const QString& url, const QString& token) {
    afb_url_ = url;
    auth_token_ = token;
    
    QString ws_url = afb_url_;
    if (!token.isEmpty()) {
        ws_url += QString("?token=%1").arg(token);
    }
    
    qInfo() << "Connecting to AFB at:" << ws_url;
    websocket_.open(QUrl(ws_url));
    
    // Wait for connection (with timeout)
    // In production, this would be asynchronous
    return true;
}

void VehicleDataManager::subscribeToSpeed() {
    if (!is_connected_) {
        qWarning() << "Not connected, cannot subscribe";
        return;
    }
    
    QJsonObject request;
    request["api"] = "vss";
    request["verb"] = "subscribe";
    
    QJsonObject args;
    args["path"] = "Vehicle.Speed";
    request["args"] = args;
    
    QString message = QJsonDocument(request).toJson(QJsonDocument::Compact);
    websocket_.sendTextMessage(message);
    
    qInfo() << "Subscribed to Vehicle.Speed";
}

bool VehicleDataManager::isDataValid() const {
    if (!is_connected_) {
        return false;
    }
    
    auto now = std::chrono::system_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - last_update_
    );
    
    return age.count() < DATA_TIMEOUT_MS;
}

void VehicleDataManager::shutdown() {
    if (is_connected_) {
        websocket_.close();
        is_connected_ = false;
    }
}

void VehicleDataManager::onConnected() {
    qInfo() << "Connected to AFB";
    is_connected_ = true;
    retry_count_ = 0;
    emit connectionEstablished();
    
    // Auto-subscribe to speed
    subscribeToSpeed();
}

void VehicleDataManager::onDisconnected() {
    qWarning() << "Disconnected from AFB";
    is_connected_ = false;
    emit connectionLost();
    
    // Schedule reconnect
    reconnect();
}

void VehicleDataManager::onTextMessageReceived(const QString& message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON received";
        return;
    }
    
    QJsonObject obj = doc.object();
    
    // Check if this is a VSS event
    if (obj.contains("event") && obj["event"].toString() == "vss/Vehicle.Speed") {
        QJsonObject data = obj["data"].toObject();
        handleSpeedUpdate(data);
    }
}

void VehicleDataManager::onError(QAbstractSocket::SocketError error) {
    qWarning() << "WebSocket error:" << websocket_.errorString();
    emit errorOccurred(websocket_.errorString());
}

void VehicleDataManager::handleSpeedUpdate(const QJsonObject& data) {
    if (!data.contains("value")) {
        qWarning() << "Speed data missing 'value' field";
        return;
    }
    
    double speed = data["value"].toDouble();
    QString unit = data["unit"].toString("km/h");
    
    if (unit != "km/h") {
        qWarning() << "Unexpected unit:" << unit;
        return;
    }
    
    // Validate speed
    if (speed < 0 || speed > 300) {
        qWarning() << "Invalid speed value:" << speed;
        return;
    }
    
    current_speed_ = speed;
    last_update_ = std::chrono::system_clock::now();
    
    emit speedUpdated(speed);
}

void VehicleDataManager::reconnect() {
    if (retry_count_ >= MAX_RETRIES) {
        qCritical() << "Max retries reached. Giving up.";
        emit errorOccurred("Cannot reconnect to AFB");
        return;
    }
    
    int delay_ms = 1000 * (1 << retry_count_); // Exponential backoff
    retry_count_++;
    
    qInfo() << "Reconnecting in" << delay_ms << "ms (attempt" << retry_count_ << ")";
    
    QTimer::singleShot(delay_ms, this, [this]() {
        initialize(afb_url_, auth_token_);
    });
}
