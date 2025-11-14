#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>

/**
 * @brief Manages application configuration
 * 
 * Loads and saves CarSpeedBoy specific settings (not vehicle protocol settings)
 */
class ConfigurationManager : public QObject {
    Q_OBJECT

public:
    struct SpeedThresholds {
        double relaxed_max = 20.0;
        double normal_max = 60.0;
        double alert_max = 100.0;
        double warning_max = 120.0;
    };

    struct DisplaySettings {
        QString units = "km/h";
        QString theme = "dark";
        QString language = "en";
        bool show_speed_number = true;
        bool fullscreen = false;
    };

    struct CharacterSettings {
        QString selected = "default_boy";
        double animation_speed = 1.0;
        bool enable_transitions = true;
    };

    struct AFBConnectionConfig {
        QString url = "ws://localhost:1234/api";
        QString token = "";
        int reconnect_interval_ms = 1000;
        int max_retries = 5;
    };

    struct LoggingConfig {
        bool enabled = true;
        QString level = "info";
        QString log_dir = "/var/log/carspeedboy";
        int max_file_size_mb = 10;
        int max_files = 5;
    };

    explicit ConfigurationManager(QObject* parent = nullptr);

    bool loadFromFile(const QString& config_path);
    bool saveToFile(const QString& config_path);

    SpeedThresholds getSpeedThresholds() const { return speed_thresholds_; }
    void setSpeedThresholds(const SpeedThresholds& thresholds);

    DisplaySettings getDisplaySettings() const { return display_settings_; }
    void setDisplaySettings(const DisplaySettings& settings);

    CharacterSettings getCharacterSettings() const { return character_settings_; }
    void setCharacterSettings(const CharacterSettings& settings);

    AFBConnectionConfig getAFBConfig() const { return afb_config_; }
    LoggingConfig getLoggingConfig() const { return logging_config_; }

    void resetToDefaults();

signals:
    void configurationChanged();

private:
    void loadDefaults();
    void parseJSON(const QJsonObject& config);
    QJsonObject toJSON() const;

    SpeedThresholds speed_thresholds_;
    DisplaySettings display_settings_;
    CharacterSettings character_settings_;
    AFBConnectionConfig afb_config_;
    LoggingConfig logging_config_;
    QString config_file_path_;
};
