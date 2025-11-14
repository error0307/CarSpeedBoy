#include "business_logic/expression_state_machine.h"
#include <QDebug>

ExpressionStateMachine::ExpressionStateMachine(QObject* parent)
    : QObject(parent)
    , current_state_(ExpressionState::RELAXED)
    , relaxed_max_(20.0)
    , normal_max_(60.0)
    , alert_max_(100.0)
    , warning_max_(120.0)
    , last_speed_(0.0)
{
}

void ExpressionStateMachine::setThresholds(double relaxed, double normal, 
                                           double alert, double warning) {
    relaxed_max_ = relaxed;
    normal_max_ = normal;
    alert_max_ = alert;
    warning_max_ = warning;
    
    qInfo() << "Speed thresholds set:" 
            << "relaxed=" << relaxed
            << "normal=" << normal
            << "alert=" << alert
            << "warning=" << warning;
}

void ExpressionStateMachine::updateSpeed(double speed) {
    ExpressionState new_state = determineState(speed);
    
    if (new_state != current_state_) {
        ExpressionState old_state = current_state_;
        setState(new_state);
        emit stateChanged(old_state, new_state);
        emit stateStringChanged(getStateString());
    }
    
    last_speed_ = speed;
}

ExpressionState ExpressionStateMachine::determineState(double speed) const {
    // Apply hysteresis to prevent rapid state changes
    double margin = (speed > last_speed_) ? 0 : hysteresis_margin_;
    
    if (speed <= relaxed_max_ + margin) {
        return ExpressionState::RELAXED;
    } else if (speed <= normal_max_ + margin) {
        return ExpressionState::NORMAL;
    } else if (speed <= alert_max_ + margin) {
        return ExpressionState::ALERT;
    } else if (speed <= warning_max_ + margin) {
        return ExpressionState::WARNING;
    } else {
        return ExpressionState::SCARED;
    }
}

void ExpressionStateMachine::setState(ExpressionState new_state) {
    current_state_ = new_state;
    qDebug() << "State changed to:" << getStateString();
}

QString ExpressionStateMachine::getStateString() const {
    switch (current_state_) {
        case ExpressionState::RELAXED: return "relaxed";
        case ExpressionState::NORMAL: return "normal";
        case ExpressionState::ALERT: return "alert";
        case ExpressionState::WARNING: return "warning";
        case ExpressionState::SCARED: return "scared";
        default: return "unknown";
    }
}
