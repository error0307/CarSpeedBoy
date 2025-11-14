#include "application_controller.h"
#include "data_acquisition/vehicle_data_manager.h"
#include "data_acquisition/configuration_manager.h"
#include "business_logic/speed_monitor.h"
#include "business_logic/expression_state_machine.h"
#include "business_logic/data_logger.h"
#include "business_logic/alert_manager.h"
#include <QCoreApplication>
#include <QDebug>

ApplicationController::ApplicationController(QObject* parent)
    : QObject(parent)
    , config_manager_(std::make_unique<ConfigurationManager>())
    , vehicle_data_manager_(std::make_unique<VehicleDataManager>())
    , speed_monitor_(std::make_unique<SpeedMonitor>())
    , state_machine_(std::make_unique<ExpressionStateMachine>())
    , data_logger_(std::make_unique<DataLogger>())
    , alert_manager_(std::make_unique<AlertManager>())
{
    qInfo() << "ApplicationController created";
}

ApplicationController::~ApplicationController() {
    shutdown();
}

bool ApplicationController::initialize() {
    qInfo() << "Initializing CarSpeedBoy...";
    
    // Load configuration
    QString config_path = "/etc/carspeedboy/config.json";
    if (!config_manager_->loadFromFile(config_path)) {
        qWarning() << "Failed to load config, using defaults";
    }
    
    // Get AFB connection config
    auto afb_config = config_manager_->getAFBConfig();
    
    // Initialize vehicle data manager
    if (!vehicle_data_manager_->initialize(afb_config.url, afb_config.token)) {
        qCritical() << "Failed to initialize vehicle data manager";
        return false;
    }
    
    // Setup speed thresholds
    auto thresholds = config_manager_->getSpeedThresholds();
    state_machine_->setThresholds(
        thresholds.relaxed_max,
        thresholds.normal_max,
        thresholds.alert_max,
        thresholds.warning_max
    );
    
    // Connect signals
    connect(vehicle_data_manager_.get(), &VehicleDataManager::speedUpdated,
            this, &ApplicationController::onSpeedUpdate);
    
    connect(vehicle_data_manager_.get(), &VehicleDataManager::errorOccurred,
            this, &ApplicationController::onVehicleDataError);
    
    connect(state_machine_.get(), &ExpressionStateMachine::stateStringChanged,
            this, &ApplicationController::expressionStateChanged);
    
    // Subscribe to speed data
    vehicle_data_manager_->subscribeToSpeed();
    
    qInfo() << "Initialization complete";
    return true;
}

int ApplicationController::run() {
    qInfo() << "CarSpeedBoy running";
    return QCoreApplication::exec();
}

void ApplicationController::shutdown() {
    qInfo() << "Shutting down CarSpeedBoy...";
    
    if (vehicle_data_manager_) {
        vehicle_data_manager_->shutdown();
    }
    
    qInfo() << "Shutdown complete";
}

void ApplicationController::onSpeedUpdate(double speed) {
    emit speedChanged(speed);
    
    // Update state machine
    state_machine_->updateSpeed(speed);
    
    // Log data
    // TODO: Implement data logging
    
    qDebug() << "Speed updated:" << speed << "km/h - State:" 
             << state_machine_->getStateString();
}

void ApplicationController::onVehicleDataError(const QString& error) {
    qWarning() << "Vehicle data error:" << error;
    emit errorOccurred(error);
}
