#pragma once

#include <QObject>
#include <QQueue>
#include "expression_state_machine.h"

/**
 * @brief Manages visual alerts based on expression state
 * 
 * Provides priority-based alert management and history tracking.
 */
class AlertManager : public QObject {
    Q_OBJECT

public:
    enum class AlertLevel {
        NONE,
        INFO,
        WARNING,
        CRITICAL
    };
    Q_ENUM(AlertLevel)

    explicit AlertManager(QObject* parent = nullptr);
    ~AlertManager();

    /**
     * @brief Get current alert level
     * @return Current alert level
     */
    AlertLevel currentAlertLevel() const { return current_alert_level_; }

    /**
     * @brief Get alert history size
     * @return Number of alerts in history
     */
    int alertHistorySize() const { return alert_history_.size(); }

public slots:
    /**
     * @brief Handle expression state change
     * @param old_state Previous state
     * @param new_state New state
     */
    void onStateChanged(ExpressionState old_state, ExpressionState new_state);

signals:
    /**
     * @brief Emitted when alert level changes
     * @param level New alert level
     * @param message Alert message
     */
    void alertTriggered(AlertLevel level, const QString& message);

    /**
     * @brief Emitted when alert is cleared
     */
    void alertCleared();

private:
    /**
     * @brief Determine alert level from expression state
     * @param state Expression state
     * @return Corresponding alert level
     */
    AlertLevel determineAlertLevel(ExpressionState state) const;

    /**
     * @brief Generate alert message
     * @param state Expression state
     * @return Alert message
     */
    QString generateAlertMessage(ExpressionState state) const;

    /**
     * @brief Add alert to history
     * @param level Alert level
     * @param message Alert message
     */
    void addToHistory(AlertLevel level, const QString& message);

    AlertLevel current_alert_level_;           ///< Current alert level
    QQueue<QString> alert_history_;            ///< Alert history
    static constexpr int MAX_HISTORY_SIZE = 100;  ///< Maximum history entries
};
