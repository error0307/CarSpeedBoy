#include "business_logic/alert_manager.h"
#include <QDebug>
#include <QDateTime>

AlertManager::AlertManager(QObject* parent)
    : QObject(parent)
    , current_alert_level_(AlertLevel::NONE)
{
    qInfo() << "AlertManager created";
}

AlertManager::~AlertManager() {
    qInfo() << "AlertManager destroyed";
}

void AlertManager::onStateChanged(ExpressionState old_state, ExpressionState new_state) {
    Q_UNUSED(old_state);
    
    AlertLevel new_level = determineAlertLevel(new_state);
    
    if (new_level != current_alert_level_) {
        current_alert_level_ = new_level;
        
        if (new_level == AlertLevel::NONE) {
            emit alertCleared();
            qInfo() << "Alert cleared";
        } else {
            QString message = generateAlertMessage(new_state);
            addToHistory(new_level, message);
            emit alertTriggered(new_level, message);
            qInfo() << "Alert triggered:" << static_cast<int>(new_level) << message;
        }
    }
}

AlertManager::AlertLevel AlertManager::determineAlertLevel(ExpressionState state) const {
    switch (state) {
        case ExpressionState::RELAXED:
        case ExpressionState::NORMAL:
            return AlertLevel::NONE;
        
        case ExpressionState::ALERT:
            return AlertLevel::INFO;
        
        case ExpressionState::WARNING:
            return AlertLevel::WARNING;
        
        case ExpressionState::SCARED:
            return AlertLevel::CRITICAL;
        
        default:
            return AlertLevel::NONE;
    }
}

QString AlertManager::generateAlertMessage(ExpressionState state) const {
    switch (state) {
        case ExpressionState::ALERT:
            return "Speed is getting high. Please be cautious.";
        
        case ExpressionState::WARNING:
            return "Speed is very high! Slow down.";
        
        case ExpressionState::SCARED:
            return "CRITICAL: Speed is dangerously high! Reduce speed immediately!";
        
        default:
            return "Normal operation";
    }
}

void AlertManager::addToHistory(AlertLevel level, const QString& message) {
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    QString entry = QString("[%1] Level %2: %3")
        .arg(timestamp)
        .arg(static_cast<int>(level))
        .arg(message);
    
    alert_history_.enqueue(entry);
    
    // Maintain maximum history size
    while (alert_history_.size() > MAX_HISTORY_SIZE) {
        alert_history_.dequeue();
    }
    
    qDebug() << "Alert history entry added:" << entry;
}
