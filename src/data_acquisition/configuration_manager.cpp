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
    // Speed thresholds
    if (config.contains("speed_thresholds")) {
        auto thresholds = config["speed_thresholds"].toObject();
        speed_thresholds_.relaxed_max = thresholds["relaxed_max"].toDouble(20.0);
        speed_thresholds_.normal_max = thresholds["normal_max"].toDouble(60.0);
        speed_thresholds_.alert_max = thresholds["alert_max"].toDouble(100.0);
        speed_thresholds_.warning_max = thresholds["warning_max"].toDouble(120.0);
    }
    
    // AFB configuration
    if (config.contains("afb")) {
        auto afb = config["afb"].toObject();
        afb_config_.url = afb["url"].toString("ws://localhost:1234/api");
        afb_config_.token = afb["token"].toString("");
        afb_config_.reconnect_interval_ms = afb["reconnect_interval_ms"].toInt(1000);
        afb_config_.max_retries = afb["max_retries"].toInt(5);
    }
    
    // Display settings
    if (config.contains("display")) {
        auto display = config["display"].toObject();
        display_settings_.units = display["units"].toString("km/h");
        display_settings_.theme = display["theme"].toString("dark");
        display_settings_.language = display["language"].toString("en");
        display_settings_.show_speed_number = display["show_speed_number"].toBool(true);
        display_settings_.fullscreen = display["fullscreen"].toBool(false);
    }
    
    // Character settings
    if (config.contains("character")) {
        auto character = config["character"].toObject();
        character_settings_.selected = character["selected"].toString("default_boy");
        character_settings_.animation_speed = character["animation_speed"].toDouble(1.0);
        character_settings_.enable_transitions = character["enable_transitions"].toBool(true);
    }
    
    // Logging configuration
    if (config.contains("logging")) {
        auto logging = config["logging"].toObject();
        logging_config_.enabled = logging["enabled"].toBool(true);
        logging_config_.level = logging["level"].toString("info");
        logging_config_.log_dir = logging["log_dir"].toString("/var/log/carspeedboy");
        logging_config_.max_file_size_mb = logging["max_file_size_mb"].toInt(10);
        logging_config_.max_files = logging["max_files"].toInt(5);
    }
}

bool ConfigurationManager::saveToFile(const QString& config_path) {
    QFile file(config_path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open config file for writing:" << config_path;
        return false;
    }
    
    QJsonObject config = toJSON();
    QJsonDocument doc(config);
    
    qint64 bytes_written = file.write(doc.toJson(QJsonDocument::Indented));
    if (bytes_written == -1) {
        qWarning() << "Failed to write config file:" << config_path;
        return false;
    }
    
    qInfo() << "Configuration saved to:" << config_path;
    return true;
}

QJsonObject ConfigurationManager::toJSON() const {
    QJsonObject config;
    
    // Speed thresholds
    QJsonObject thresholds;
    thresholds["relaxed_max"] = speed_thresholds_.relaxed_max;
    thresholds["normal_max"] = speed_thresholds_.normal_max;
    thresholds["alert_max"] = speed_thresholds_.alert_max;
    thresholds["warning_max"] = speed_thresholds_.warning_max;
    config["speed_thresholds"] = thresholds;
    
    // AFB configuration
    QJsonObject afb;
    afb["url"] = afb_config_.url;
    afb["token"] = afb_config_.token;
    afb["reconnect_interval_ms"] = afb_config_.reconnect_interval_ms;
    afb["max_retries"] = afb_config_.max_retries;
    config["afb"] = afb;
    
    // Display settings
    QJsonObject display;
    display["units"] = display_settings_.units;
    display["theme"] = display_settings_.theme;
    display["language"] = display_settings_.language;
    display["show_speed_number"] = display_settings_.show_speed_number;
    display["fullscreen"] = display_settings_.fullscreen;
    config["display"] = display;
    
    // Character settings
    QJsonObject character;
    character["selected"] = character_settings_.selected;
    character["animation_speed"] = character_settings_.animation_speed;
    character["enable_transitions"] = character_settings_.enable_transitions;
    config["character"] = character;
    
    // Logging configuration
    QJsonObject logging;
    logging["enabled"] = logging_config_.enabled;
    logging["level"] = logging_config_.level;
    logging["log_dir"] = logging_config_.log_dir;
    logging["max_file_size_mb"] = logging_config_.max_file_size_mb;
    logging["max_files"] = logging_config_.max_files;
    config["logging"] = logging;
    
    return config;
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

void ConfigurationManager::setAFBConfig(const AFBConnectionConfig& config) {
    afb_config_ = config;
    emit configurationChanged();
}

void ConfigurationManager::setLoggingConfig(const LoggingConfig& config) {
    logging_config_ = config;
    emit configurationChanged();
}

void ConfigurationManager::resetToDefaults() {
    loadDefaults();
    emit configurationChanged();
}
