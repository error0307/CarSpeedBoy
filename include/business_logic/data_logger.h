#pragma once
#include <QObject>

class DataLogger : public QObject {
    Q_OBJECT
public:
    explicit DataLogger(QObject* parent = nullptr) : QObject(parent) {}
    // TODO: Implement data logging
};
