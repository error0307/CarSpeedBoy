#include "application_controller.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    
    QCoreApplication::setOrganizationName("CarSpeedBoy");
    QCoreApplication::setApplicationName("CarSpeedBoy");
    QCoreApplication::setApplicationVersion("1.0.0");
    
    qInfo() << "CarSpeedBoy starting...";
    
    ApplicationController controller;
    
    if (!controller.initialize()) {
        qCritical() << "Failed to initialize application";
        return 1;
    }
    
    return controller.run();
}
