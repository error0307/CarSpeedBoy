#pragma once
#include <QObject>

class CharacterAnimationEngine : public QObject {
    Q_OBJECT
public:
    explicit CharacterAnimationEngine(QObject* parent = nullptr) : QObject(parent) {}
    // TODO: Implement character animation
};
