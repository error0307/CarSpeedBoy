#include "data_acquisition/configuration_manager.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

ConfigurationManager::ConfigurationManager(QObject* parent)
    : QObject(parent)
{
    loadDefaults();
}

bool ConfigurationManager::loadFromFile(const QString& config_path) {
    config_file_path_ = config_path;
    
    QFile file(config_path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Config file not found:" << config_path << "Using defaults.";
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (!doc.isObject()) {
        qWarning() << "Invalid config file format";
        return false;
    }
    
    parseJSON(doc.object());
    qInfo() << "Configuration loaded from:" << config_path;
    return true;
}

void ConfigurationManager::loadDefaults() {
    speed_thresholds_ = SpeedThresholds();
    display_settings_ = DisplaySettings();
    character_settings_ = CharacterSettings();
    afb_config_ = AFBConnectionConfig();
    logging_config_ = LoggingConfig();
}

void ConfigurationManager::parseJSON(const QJsonObject& config) {
    // TODO: Implement full JSON parsing
    // This is a stub implementation
    
    if (config.contains("speed_thresholds")) {
        auto thresholds = config["speed_thresholds"].toObject();
        speed_thresholds_.relaxed_max = thresholds["relaxed_max"].toDouble(20.0);
        speed_thresholds_.normal_max = thresholds["normal_max"].toDouble(60.0);
        speed_thresholds_.alert_max = thresholds["alert_max"].toDouble(100.0);
        speed_thresholds_.warning_max = thresholds["warning_max"].toDouble(120.0);
    }
    
    if (config.contains("afb")) {
        auto afb = config["afb"].toObject();
        afb_config_.url = afb["url"].toString("ws://localhost:1234/api");
        afb_config_.token = afb["token"].toString("");
    }
}

bool ConfigurationManager::saveToFile(const QString& config_path) {
    // TODO: Implement save functionality
    qInfo() << "Configuration save requested to:" << config_path;
    return true;
}

QJsonObject ConfigurationManager::toJSON() const {
    // TODO: Implement JSON serialization
    return QJsonObject();
}

void ConfigurationManager::setSpeedThresholds(const SpeedThresholds& thresholds) {
    speed_thresholds_ = thresholds;
    emit configurationChanged();
}

void ConfigurationManager::setDisplaySettings(const DisplaySettings& settings) {
    display_settings_ = settings;
    emit configurationChanged();
}

void ConfigurationManager::setCharacterSettings(const CharacterSettings& settings) {
    character_settings_ = settings;
    emit configurationChanged();
}

void ConfigurationManager::resetToDefaults() {
    loadDefaults();
    emit configurationChanged();
}
