#pragma once
#include <QObject>

class AlertManager : public QObject {
    Q_OBJECT
public:
    explicit AlertManager(QObject* parent = nullptr) : QObject(parent) {}
    // TODO: Implement alert management
};
