#pragma once

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "expression_state_machine.h"

/**
 * @brief Logs speed and state data to CSV files
 * 
 * Provides structured logging with automatic file rotation
 * based on file size.
 */
class DataLogger : public QObject {
    Q_OBJECT

public:
    explicit DataLogger(const QString& log_dir = "/var/log/carspeedboy", 
                       QObject* parent = nullptr);
    ~DataLogger();

    /**
     * @brief Enable or disable logging
     * @param enabled true to enable, false to disable
     */
    void setEnabled(bool enabled);

    /**
     * @brief Check if logging is enabled
     * @return true if enabled
     */
    bool isEnabled() const { return enabled_; }

    /**
     * @brief Set maximum log file size before rotation
     * @param size_bytes Maximum size in bytes (default: 10MB)
     */
    void setMaxFileSize(qint64 size_bytes);

    /**
     * @brief Get current log file path
     * @return Log file path
     */
    QString currentLogFile() const { return current_log_file_; }

public slots:
    /**
     * @brief Log speed data
     * @param raw_speed Raw speed value
     * @param smoothed_speed Smoothed speed value
     * @param state Current expression state
     */
    void logSpeedData(double raw_speed, double smoothed_speed, ExpressionState state);

signals:
    /**
     * @brief Emitted when log file is rotated
     * @param new_file_path Path to new log file
     */
    void logFileRotated(const QString& new_file_path);

    /**
     * @brief Emitted when logging error occurs
     * @param error_message Error description
     */
    void loggingError(const QString& error_message);

private:
    /**
     * @brief Open new log file
     * @return true if successful
     */
    bool openLogFile();

    /**
     * @brief Close current log file
     */
    void closeLogFile();

    /**
     * @brief Check if rotation is needed and perform it
     */
    void checkAndRotate();

    /**
     * @brief Write CSV header to file
     */
    void writeHeader();

    /**
     * @brief Convert ExpressionState to string
     * @param state State to convert
     * @return State name
     */
    QString stateToString(ExpressionState state) const;

    QString log_dir_;                ///< Directory for log files
    QString current_log_file_;       ///< Current log file path
    QFile* log_file_;                ///< Current log file
    QTextStream* log_stream_;        ///< Text stream for writing
    bool enabled_;                   ///< Logging enabled flag
    qint64 max_file_size_;          ///< Maximum file size before rotation
    
    static constexpr qint64 DEFAULT_MAX_SIZE = 10 * 1024 * 1024;  ///< 10MB
};
