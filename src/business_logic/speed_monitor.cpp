#include "business_logic/speed_monitor.h"
#include <QDebug>
#include <numeric>

SpeedMonitor::SpeedMonitor(int window_size, QObject* parent)
    : QObject(parent)
    , window_size_(window_size)
    , smoothed_speed_(0.0)
{
    if (window_size_ < 1) {
        window_size_ = 1;
        qWarning() << "Window size too small, using 1";
    } else if (window_size_ > 20) {
        window_size_ = 20;
        qWarning() << "Window size too large, using 20";
    }
    
    qInfo() << "SpeedMonitor created with window size:" << window_size_;
}

SpeedMonitor::~SpeedMonitor() {
    qInfo() << "SpeedMonitor destroyed";
}

void SpeedMonitor::setWindowSize(int size) {
    if (size < 1 || size > 20) {
        qWarning() << "Invalid window size:" << size << "(must be 1-20)";
        return;
    }
    
    window_size_ = size;
    
    // Trim samples if new window is smaller
    while (speed_samples_.size() > window_size_) {
        speed_samples_.dequeue();
    }
    
    qInfo() << "Window size changed to:" << window_size_;
}

void SpeedMonitor::reset() {
    speed_samples_.clear();
    smoothed_speed_ = 0.0;
    qInfo() << "SpeedMonitor reset";
}

void SpeedMonitor::onRawSpeedUpdate(double raw_speed) {
    // Validate speed
    if (!isValidSpeed(raw_speed)) {
        qWarning() << "Abnormal speed detected:" << raw_speed << "km/h";
        emit abnormalSpeedDetected(raw_speed);
        return;
    }
    
    // Add to sample queue
    speed_samples_.enqueue(raw_speed);
    
    // Maintain window size
    if (speed_samples_.size() > window_size_) {
        speed_samples_.dequeue();
    }
    
    // Calculate smoothed speed
    smoothed_speed_ = calculateAverage();
    
    qDebug() << "Speed update - Raw:" << raw_speed 
             << "Smoothed:" << smoothed_speed_
             << "Samples:" << speed_samples_.size();
    
    emit smoothedSpeedUpdated(smoothed_speed_);
}

bool SpeedMonitor::isValidSpeed(double speed) const {
    return speed >= MIN_SPEED && speed <= MAX_SPEED;
}

double SpeedMonitor::calculateAverage() const {
    if (speed_samples_.isEmpty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(speed_samples_.begin(), speed_samples_.end(), 0.0);
    return sum / speed_samples_.size();
}
