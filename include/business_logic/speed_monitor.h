#pragma once

#include <QObject>

class SpeedMonitor : public QObject {
    Q_OBJECT
public:
    explicit SpeedMonitor(QObject* parent = nullptr) : QObject(parent) {}
    // TODO: Implement speed monitoring logic
};
