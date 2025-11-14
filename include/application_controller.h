#pragma once

#include <QObject>
#include <memory>

class VehicleDataManager;
class SpeedMonitor;
class ExpressionStateMachine;
class DataLogger;
class AlertManager;
class ConfigurationManager;

/**
 * @brief Main application controller
 * 
 * Coordinates all components and manages the application lifecycle
 */
class ApplicationController : public QObject {
    Q_OBJECT

public:
    explicit ApplicationController(QObject* parent = nullptr);
    ~ApplicationController();

    /**
     * @brief Initialize the application
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Start the application
     * @return Exit code
     */
    int run();

    /**
     * @brief Shutdown the application
     */
    void shutdown();

signals:
    void speedChanged(double speed);
    void expressionStateChanged(const QString& state);
    void errorOccurred(const QString& message);

private slots:
    void onSpeedUpdate(double speed);
    void onVehicleDataError(const QString& error);

private:
    std::unique_ptr<ConfigurationManager> config_manager_;
    std::unique_ptr<VehicleDataManager> vehicle_data_manager_;
    std::unique_ptr<SpeedMonitor> speed_monitor_;
    std::unique_ptr<ExpressionStateMachine> state_machine_;
    std::unique_ptr<DataLogger> data_logger_;
    std::unique_ptr<AlertManager> alert_manager_;
};
