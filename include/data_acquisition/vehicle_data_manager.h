#pragma once

#include <QObject>
#include <QWebSocket>
#include <QString>
#include <QJsonObject>
#include <chrono>

/**
 * @brief Manages vehicle data acquisition from AGL VSS
 * 
 * Connects to AFB WebSocket API and subscribes to Vehicle.Speed signal
 */
class VehicleDataManager : public QObject {
    Q_OBJECT

public:
    explicit VehicleDataManager(QObject* parent = nullptr);
    ~VehicleDataManager();

    /**
     * @brief Initialize connection to AFB/VSS
     * @param url AFB WebSocket URL
     * @param token Authentication token
     * @return true if connection successful
     */
    bool initialize(const QString& url, const QString& token);

    /**
     * @brief Subscribe to Vehicle.Speed signal
     */
    void subscribeToSpeed();

    /**
     * @brief Get current cached speed value
     * @return Speed in km/h
     */
    double getCurrentSpeed() const { return current_speed_; }

    /**
     * @brief Check if data is valid (not stale)
     * @return true if data is fresh
     */
    bool isDataValid() const;

    /**
     * @brief Shutdown and disconnect
     */
    void shutdown();

signals:
    void speedUpdated(double speed);
    void connectionEstablished();
    void connectionLost();
    void errorOccurred(const QString& error);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString& message);
    void onError(QAbstractSocket::SocketError error);

private:
    void handleSpeedUpdate(const QJsonObject& data);
    void reconnect();

    QWebSocket websocket_;
    QString afb_url_;
    QString auth_token_;
    double current_speed_;
    std::chrono::system_clock::time_point last_update_;
    bool is_connected_;
    int retry_count_;

    static constexpr int MAX_RETRIES = 5;
    static constexpr int DATA_TIMEOUT_MS = 5000;
};
