#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include "expression_state_machine.h"

/**
 * @brief Manages character animations based on expression state
 * 
 * Maps expression states to animation file paths and provides
 * properties for QML integration.
 */
class CharacterAnimationEngine : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentAnimationPath READ currentAnimationPath NOTIFY animationChanged)

public:
    explicit CharacterAnimationEngine(const QString& resource_dir = "/usr/share/carspeedboy/resources",
                                     QObject* parent = nullptr);
    ~CharacterAnimationEngine();

    /**
     * @brief Get current animation file path
     * @return Path to current animation file
     */
    QString currentAnimationPath() const { return current_animation_path_; }

    /**
     * @brief Set resource directory
     * @param dir Directory containing animation files
     */
    void setResourceDirectory(const QString& dir);

    /**
     * @brief Get resource directory
     * @return Resource directory path
     */
    QString resourceDirectory() const { return resource_dir_; }

public slots:
    /**
     * @brief Handle expression state change
     * @param old_state Previous state
     * @param new_state New state
     */
    void onStateChanged(ExpressionState old_state, ExpressionState new_state);

signals:
    /**
     * @brief Emitted when animation changes
     * @param animation_path Path to new animation file
     */
    void animationChanged(const QString& animation_path);

    /**
     * @brief Emitted when animation file is missing
     * @param expected_path Expected file path
     */
    void animationMissing(const QString& expected_path);

private:
    /**
     * @brief Load animation mappings
     */
    void loadAnimationMappings();

    /**
     * @brief Get animation path for state
     * @param state Expression state
     * @return Animation file path
     */
    QString getAnimationPath(ExpressionState state) const;

    /**
     * @brief Convert state to animation filename
     * @param state Expression state
     * @return Animation filename
     */
    QString stateToFilename(ExpressionState state) const;

    QString resource_dir_;                           ///< Resource directory
    QString current_animation_path_;                 ///< Current animation path
    QMap<ExpressionState, QString> animation_map_;  ///< State to animation mapping
};
