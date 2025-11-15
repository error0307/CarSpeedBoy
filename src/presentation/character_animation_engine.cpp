#include "presentation/character_animation_engine.h"
#include <QFile>
#include <QDebug>

CharacterAnimationEngine::CharacterAnimationEngine(const QString& resource_dir, QObject* parent)
    : QObject(parent)
    , resource_dir_(resource_dir)
{
    loadAnimationMappings();
    
    // Set initial animation (RELAXED state)
    current_animation_path_ = getAnimationPath(ExpressionState::RELAXED);
    
    qInfo() << "CharacterAnimationEngine created with resource dir:" << resource_dir_;
}

CharacterAnimationEngine::~CharacterAnimationEngine() {
    qInfo() << "CharacterAnimationEngine destroyed";
}

void CharacterAnimationEngine::setResourceDirectory(const QString& dir) {
    resource_dir_ = dir;
    loadAnimationMappings();
    qInfo() << "Resource directory changed to:" << dir;
}

void CharacterAnimationEngine::onStateChanged(ExpressionState old_state, ExpressionState new_state) {
    Q_UNUSED(old_state);
    
    QString new_animation = getAnimationPath(new_state);
    
    if (new_animation != current_animation_path_) {
        current_animation_path_ = new_animation;
        
        // Check if animation file exists
        if (!QFile::exists(current_animation_path_)) {
            qWarning() << "Animation file not found:" << current_animation_path_;
            emit animationMissing(current_animation_path_);
        }
        
        emit animationChanged(current_animation_path_);
        qInfo() << "Animation changed to:" << current_animation_path_;
    }
}

void CharacterAnimationEngine::loadAnimationMappings() {
    animation_map_.clear();
    
    animation_map_[ExpressionState::RELAXED] = 
        resource_dir_ + "/" + stateToFilename(ExpressionState::RELAXED);
    animation_map_[ExpressionState::NORMAL] = 
        resource_dir_ + "/" + stateToFilename(ExpressionState::NORMAL);
    animation_map_[ExpressionState::ALERT] = 
        resource_dir_ + "/" + stateToFilename(ExpressionState::ALERT);
    animation_map_[ExpressionState::WARNING] = 
        resource_dir_ + "/" + stateToFilename(ExpressionState::WARNING);
    animation_map_[ExpressionState::SCARED] = 
        resource_dir_ + "/" + stateToFilename(ExpressionState::SCARED);
    
    qDebug() << "Loaded" << animation_map_.size() << "animation mappings";
}

QString CharacterAnimationEngine::getAnimationPath(ExpressionState state) const {
    return animation_map_.value(state, resource_dir_ + "/default.gif");
}

QString CharacterAnimationEngine::stateToFilename(ExpressionState state) const {
    switch (state) {
        case ExpressionState::RELAXED:
            return "relaxed.gif";
        case ExpressionState::NORMAL:
            return "normal.gif";
        case ExpressionState::ALERT:
            return "alert.gif";
        case ExpressionState::WARNING:
            return "warning.gif";
        case ExpressionState::SCARED:
            return "scared.gif";
        default:
            return "default.gif";
    }
}
