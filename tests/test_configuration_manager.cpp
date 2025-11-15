#include <QtTest/QtTest>
#include <QTemporaryFile>
#include <QJsonDocument>
#include <QJsonObject>
#include "configuration_manager.h"

/**
 * @brief Unit tests for ConfigurationManager
 */
class TestConfigurationManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases
    void testDefaultConfiguration();
    void testLoadValidConfiguration();
    void testLoadInvalidFile();
    void testLoadMalformedJSON();
    void testSpeedThresholds();
    void testAFBConfiguration();
    void testDisplaySettings();
    void testLoggingConfiguration();
    void testSaveConfiguration();

private:
    ConfigurationManager* config_manager_;
    QTemporaryFile* temp_file_;
    
    QString createTestConfig(const QJsonObject& config);
};

void TestConfigurationManager::initTestCase() {
    qInfo() << "Starting ConfigurationManager tests";
}

void TestConfigurationManager::cleanupTestCase() {
    qInfo() << "ConfigurationManager tests completed";
}

void TestConfigurationManager::init() {
    config_manager_ = new ConfigurationManager();
    temp_file_ = new QTemporaryFile();
}

void TestConfigurationManager::cleanup() {
    delete config_manager_;
    config_manager_ = nullptr;
    delete temp_file_;
    temp_file_ = nullptr;
}

QString TestConfigurationManager::createTestConfig(const QJsonObject& config) {
    if (!temp_file_->open()) {
        return QString();
    }
    
    QJsonDocument doc(config);
    temp_file_->write(doc.toJson());
    temp_file_->flush();
    
    return temp_file_->fileName();
}

void TestConfigurationManager::testDefaultConfiguration() {
    // Test default values
    auto thresholds = config_manager_->speedThresholds();
    QCOMPARE(thresholds.relaxed_max, 20.0);
    QCOMPARE(thresholds.normal_max, 60.0);
    QCOMPARE(thresholds.alert_max, 100.0);
    QCOMPARE(thresholds.warning_max, 120.0);
}

void TestConfigurationManager::testLoadValidConfiguration() {
    QJsonObject config;
    
    QJsonObject thresholds;
    thresholds["relaxed_max"] = 25.0;
    thresholds["normal_max"] = 70.0;
    thresholds["alert_max"] = 110.0;
    thresholds["warning_max"] = 130.0;
    config["speed_thresholds"] = thresholds;
    
    QJsonObject afb;
    afb["url"] = "ws://testhost:5678/api";
    afb["retry_attempts"] = 10;
    afb["retry_interval_ms"] = 3000;
    config["afb"] = afb;
    
    QString filepath = createTestConfig(config);
    QVERIFY(!filepath.isEmpty());
    
    bool result = config_manager_->loadFromFile(filepath);
    QVERIFY(result);
    
    auto loaded_thresholds = config_manager_->speedThresholds();
    QCOMPARE(loaded_thresholds.relaxed_max, 25.0);
    QCOMPARE(loaded_thresholds.normal_max, 70.0);
    
    auto afb_config = config_manager_->afbConfig();
    QCOMPARE(afb_config.url, QString("ws://testhost:5678/api"));
    QCOMPARE(afb_config.retry_attempts, 10);
}

void TestConfigurationManager::testLoadInvalidFile() {
    bool result = config_manager_->loadFromFile("/nonexistent/path/config.json");
    QVERIFY(!result);
    
    // Should keep default values
    auto thresholds = config_manager_->speedThresholds();
    QCOMPARE(thresholds.relaxed_max, 20.0);
}

void TestConfigurationManager::testLoadMalformedJSON() {
    if (!temp_file_->open()) {
        QFAIL("Failed to create temp file");
    }
    
    temp_file_->write("{ invalid json content }");
    temp_file_->flush();
    
    bool result = config_manager_->loadFromFile(temp_file_->fileName());
    QVERIFY(!result);
}

void TestConfigurationManager::testSpeedThresholds() {
    SpeedThresholds thresholds;
    thresholds.relaxed_max = 30.0;
    thresholds.normal_max = 80.0;
    thresholds.alert_max = 120.0;
    thresholds.warning_max = 150.0;
    
    config_manager_->setSpeedThresholds(thresholds);
    
    auto loaded = config_manager_->speedThresholds();
    QCOMPARE(loaded.relaxed_max, 30.0);
    QCOMPARE(loaded.normal_max, 80.0);
    QCOMPARE(loaded.alert_max, 120.0);
    QCOMPARE(loaded.warning_max, 150.0);
}

void TestConfigurationManager::testAFBConfiguration() {
    AFBConnectionConfig afb_config;
    afb_config.url = "ws://custom:9999/api";
    afb_config.token = "test_token";
    afb_config.retry_attempts = 20;
    afb_config.retry_interval_ms = 5000;
    
    config_manager_->setAFBConfig(afb_config);
    
    auto loaded = config_manager_->afbConfig();
    QCOMPARE(loaded.url, QString("ws://custom:9999/api"));
    QCOMPARE(loaded.token, QString("test_token"));
    QCOMPARE(loaded.retry_attempts, 20);
}

void TestConfigurationManager::testDisplaySettings() {
    DisplaySettings display;
    display.units = "mph";
    display.theme = "light";
    
    config_manager_->setDisplaySettings(display);
    
    auto loaded = config_manager_->displaySettings();
    QCOMPARE(loaded.units, QString("mph"));
    QCOMPARE(loaded.theme, QString("light"));
}

void TestConfigurationManager::testLoggingConfiguration() {
    LoggingConfig logging;
    logging.enabled = false;
    logging.log_dir = "/tmp/test_logs";
    logging.level = "error";
    
    config_manager_->setLoggingConfig(logging);
    
    auto loaded = config_manager_->loggingConfig();
    QCOMPARE(loaded.enabled, false);
    QCOMPARE(loaded.log_dir, QString("/tmp/test_logs"));
    QCOMPARE(loaded.level, QString("error"));
}

void TestConfigurationManager::testSaveConfiguration() {
    // Set custom configuration
    SpeedThresholds thresholds;
    thresholds.relaxed_max = 35.0;
    thresholds.normal_max = 75.0;
    thresholds.alert_max = 115.0;
    thresholds.warning_max = 135.0;
    config_manager_->setSpeedThresholds(thresholds);
    
    // Save to temp file
    QTemporaryFile save_file;
    QVERIFY(save_file.open());
    QString save_path = save_file.fileName();
    save_file.close();
    
    bool save_result = config_manager_->saveToFile(save_path);
    QVERIFY(save_result);
    
    // Load into new instance
    ConfigurationManager* new_manager = new ConfigurationManager();
    bool load_result = new_manager->loadFromFile(save_path);
    QVERIFY(load_result);
    
    auto loaded_thresholds = new_manager->speedThresholds();
    QCOMPARE(loaded_thresholds.relaxed_max, 35.0);
    QCOMPARE(loaded_thresholds.normal_max, 75.0);
    
    delete new_manager;
}

QTEST_MAIN(TestConfigurationManager)
#include "test_configuration_manager.moc"
