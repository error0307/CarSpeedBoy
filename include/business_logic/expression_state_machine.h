#pragma once

#include <QObject>
#include <QString>

/**
 * @brief Expression states based on vehicle speed
 */
enum class ExpressionState {
    RELAXED,    // 0-20 km/h
    NORMAL,     // 21-60 km/h
    ALERT,      // 61-100 km/h
    WARNING,    // 101-120 km/h
    SCARED      // 121+ km/h
};

/**
 * @brief Manages expression state transitions based on speed
 */
class ExpressionStateMachine : public QObject {
    Q_OBJECT

public:
    explicit ExpressionStateMachine(QObject* parent = nullptr);

    void setThresholds(double relaxed, double normal, double alert, double warning);
    void updateSpeed(double speed);
    
    ExpressionState getCurrentState() const { return current_state_; }
    QString getStateString() const;

signals:
    void stateChanged(ExpressionState oldState, ExpressionState newState);
    void stateStringChanged(const QString& state);

private:
    ExpressionState determineState(double speed) const;
    void setState(ExpressionState new_state);

    ExpressionState current_state_;
    double relaxed_max_;
    double normal_max_;
    double alert_max_;
    double warning_max_;
    
    // Hysteresis to prevent rapid state changes
    double hysteresis_margin_ = 2.0;
    double last_speed_;
};

// Declare metatype for use in signals/slots
Q_DECLARE_METATYPE(ExpressionState)
