#include "business_logic/data_logger.h"
#include <QDir>
#include <QDebug>

DataLogger::DataLogger(const QString& log_dir, QObject* parent)
    : QObject(parent)
    , log_dir_(log_dir)
    , log_file_(nullptr)
    , log_stream_(nullptr)
    , enabled_(true)
    , max_file_size_(DEFAULT_MAX_SIZE)
{
    // Create log directory if it doesn't exist
    QDir dir;
    if (!dir.exists(log_dir_)) {
        if (dir.mkpath(log_dir_)) {
            qInfo() << "Created log directory:" << log_dir_;
        } else {
            qWarning() << "Failed to create log directory:" << log_dir_;
        }
    }
    
    openLogFile();
}

DataLogger::~DataLogger() {
    closeLogFile();
    qInfo() << "DataLogger destroyed";
}

void DataLogger::setEnabled(bool enabled) {
    enabled_ = enabled;
    qInfo() << "DataLogger" << (enabled ? "enabled" : "disabled");
}

void DataLogger::setMaxFileSize(qint64 size_bytes) {
    max_file_size_ = size_bytes;
    qInfo() << "Max log file size set to:" << size_bytes << "bytes";
}

void DataLogger::logSpeedData(double raw_speed, double smoothed_speed, ExpressionState state) {
    if (!enabled_ || !log_stream_) {
        return;
    }
    
    // Check if rotation is needed
    checkAndRotate();
    
    // Write log entry
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    *log_stream_ << timestamp << ","
                << raw_speed << ","
                << smoothed_speed << ","
                << stateToString(state) << "\n";
    
    log_stream_->flush();
    
    qDebug() << "Logged:" << timestamp << raw_speed << smoothed_speed << stateToString(state);
}

bool DataLogger::openLogFile() {
    closeLogFile();
    
    // Generate log file name with current date
    QString filename = QString("speed_log_%1.csv")
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    current_log_file_ = log_dir_ + "/" + filename;
    
    log_file_ = new QFile(current_log_file_);
    
    if (!log_file_->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        qCritical() << "Failed to open log file:" << current_log_file_;
        emit loggingError("Failed to open log file: " + current_log_file_);
        delete log_file_;
        log_file_ = nullptr;
        return false;
    }
    
    log_stream_ = new QTextStream(log_file_);
    
    // Write header if file is new
    if (log_file_->size() == 0) {
        writeHeader();
    }
    
    qInfo() << "Opened log file:" << current_log_file_;
    return true;
}

void DataLogger::closeLogFile() {
    if (log_stream_) {
        log_stream_->flush();
        delete log_stream_;
        log_stream_ = nullptr;
    }
    
    if (log_file_) {
        log_file_->close();
        delete log_file_;
        log_file_ = nullptr;
    }
}

void DataLogger::checkAndRotate() {
    if (!log_file_) {
        return;
    }
    
    // Check file size
    if (log_file_->size() >= max_file_size_) {
        qInfo() << "Log file size limit reached, rotating...";
        QString old_file = current_log_file_;
        
        if (openLogFile()) {
            emit logFileRotated(current_log_file_);
            qInfo() << "Rotated from" << old_file << "to" << current_log_file_;
        }
    }
}

void DataLogger::writeHeader() {
    if (log_stream_) {
        *log_stream_ << "timestamp,raw_speed_kmh,smoothed_speed_kmh,expression_state\n";
        log_stream_->flush();
    }
}

QString DataLogger::stateToString(ExpressionState state) const {
    switch (state) {
        case ExpressionState::RELAXED: return "RELAXED";
        case ExpressionState::NORMAL: return "NORMAL";
        case ExpressionState::ALERT: return "ALERT";
        case ExpressionState::WARNING: return "WARNING";
        case ExpressionState::SCARED: return "SCARED";
        default: return "UNKNOWN";
    }
}
