#pragma once

#include <QObject>
#include <QQueue>

/**
 * @brief Monitors and processes vehicle speed data with noise filtering
 * 
 * Applies moving average filter to smooth out noisy speed readings
 * and detects abnormal values.
 */
class SpeedMonitor : public QObject {
    Q_OBJECT

public:
    explicit SpeedMonitor(int window_size = 5, QObject* parent = nullptr);
    ~SpeedMonitor();

    /**
     * @brief Set the moving average window size
     * @param size Number of samples to average (1-20)
     */
    void setWindowSize(int size);

    /**
     * @brief Get current window size
     * @return Window size
     */
    int windowSize() const { return window_size_; }

    /**
     * @brief Get the last smoothed speed value
     * @return Smoothed speed in km/h
     */
    double smoothedSpeed() const { return smoothed_speed_; }

    /**
     * @brief Reset the filter (clear all samples)
     */
    void reset();

public slots:
    /**
     * @brief Process new raw speed reading
     * @param raw_speed Speed in km/h
     */
    void onRawSpeedUpdate(double raw_speed);

signals:
    /**
     * @brief Emitted when smoothed speed is calculated
     * @param speed_kmh Smoothed speed in km/h
     */
    void smoothedSpeedUpdated(double speed_kmh);

    /**
     * @brief Emitted when abnormal speed value is detected
     * @param raw_speed The abnormal speed value
     */
    void abnormalSpeedDetected(double raw_speed);

private:
    /**
     * @brief Validate if speed is within acceptable range
     * @param speed Speed value to validate
     * @return true if valid, false otherwise
     */
    bool isValidSpeed(double speed) const;

    /**
     * @brief Calculate moving average from current samples
     * @return Average speed
     */
    double calculateAverage() const;

    int window_size_;                    ///< Moving average window size
    QQueue<double> speed_samples_;       ///< Ring buffer of speed samples
    double smoothed_speed_;              ///< Last calculated smoothed speed
    
    static constexpr double MIN_SPEED = 0.0;     ///< Minimum valid speed (km/h)
    static constexpr double MAX_SPEED = 300.0;   ///< Maximum valid speed (km/h)
};
