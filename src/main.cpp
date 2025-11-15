#include "application_controller.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    
    QCoreApplication::setOrganizationName("CarSpeedBoy");
    QCoreApplication::setApplicationName("CarSpeedBoy");
    QCoreApplication::setApplicationVersion("1.0.0");
    
    qInfo() << "CarSpeedBoy starting...";
    
    // Create application controller
    ApplicationController controller;
    
    if (!controller.initialize()) {
        qCritical() << "Failed to initialize application";
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose controller to QML
    engine.rootContext()->setContextProperty("appController", &controller);
    
    // Load main QML
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    qInfo() << "CarSpeedBoy running";
    
    return app.exec();
}
