# Data Acquisition Layer 詳細説明

## 概要

Data Acquisition Layer（データ取得層）は、CarSpeedBoyアプリケーションが車両の速度情報を取得するための層です。

## なぜプロトコルハンドラが不要なのか？

### 従来のアプローチ（プロトコル依存）

```
┌─────────────────────────────────────────────────────────────┐
│ アプリケーション                                              │
│  ┌─────────────────────────────────────────────────────────┐│
│  │ Vehicle Data Manager                                    ││
│  │  - CANプロトコルを理解する必要がある                      ││
│  │  - 車両固有のCAN ID（例: 0x123）を知る必要がある         ││
│  │  - ビットフィールドを手動でパースする                     ││
│  └─────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────┘
                            ▼
        ❌ 問題点:
        - 車両メーカーごとに実装が異なる
        - CANメッセージ仕様書が必要
        - コードの保守性が低い
        - 車両変更時に大幅な改修が必要
```

### VSSを使うアプローチ（プロトコル非依存）✅

```
┌─────────────────────────────────────────────────────────────┐
│ CarSpeedBoy Application                                      │
│  ┌─────────────────────────────────────────────────────────┐│
│  │ Vehicle Data Manager                                    ││
│  │  - "Vehicle.Speed" というパスを購読するだけ              ││
│  │  - プロトコルは完全に意識不要                            ││
│  │  - 単位は常にkm/h（標準化済み）                          ││
│  └─────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────┘
                            ▼
                   AFB WebSocket API
                            ▼
┌─────────────────────────────────────────────────────────────┐
│ AGL VSS Binding (プラットフォーム側で提供)                    │
│  - Vehicle.Speed → CAN ID 0x123 のマッピング                │
│  - ビットフィールドの解析                                     │
│  - 単位変換（生データ → km/h）                               │
└─────────────────────────────────────────────────────────────┘
                            ▼
                    low-can binding
                            ▼
                      Linux SocketCAN
```

## Data Acquisition Layerの責務

### ✅ やるべきこと

1. **AFB/VSS APIへの接続**
   ```cpp
   void VehicleDataManager::connectToVSS() {
       // WebSocket接続
       afb_client_.connect("ws://localhost:1234/api?token=xxx");
       
       // VSSサービスに接続確認
       afb_client_.call("vss", "ping", {}, [](const json& response) {
           std::cout << "VSS service connected" << std::endl;
       });
   }
   ```

2. **Vehicle.Speedの購読**
   ```cpp
   void VehicleDataManager::subscribeToSpeed() {
       afb_client_.call("vss", "subscribe", {
           {"path", "Vehicle.Speed"}
       }, [this](const json& response) {
           // 購読成功
           std::cout << "Subscribed to Vehicle.Speed" << std::endl;
       });
       
       // イベントハンドラ登録
       afb_client_.on_event("vss/Vehicle.Speed", 
           [this](const json& event_data) {
               handleSpeedUpdate(event_data);
           });
   }
   ```

3. **データ受信とバリデーション**
   ```cpp
   void VehicleDataManager::handleSpeedUpdate(const json& data) {
       try {
           // VSSからのデータ形式:
           // {
           //   "path": "Vehicle.Speed",
           //   "value": 65.5,
           //   "unit": "km/h",
           //   "timestamp": 1699964123456
           // }
           
           double speed = data["value"];
           std::string unit = data["unit"];
           
           // バリデーション
           if (unit != "km/h") {
               throw std::runtime_error("Unexpected unit: " + unit);
           }
           
           if (speed < 0 || speed > 300) {
               std::cerr << "Invalid speed value: " << speed << std::endl;
               return;
           }
           
           // キャッシュ更新
           current_speed_ = speed;
           last_update_ = std::chrono::system_clock::now();
           
           // アプリケーションコントローラに通知
           notifySpeedChange(speed);
           
       } catch (const std::exception& e) {
           std::cerr << "Error parsing speed data: " << e.what() << std::endl;
       }
   }
   ```

4. **エラーハンドリング**
   ```cpp
   void VehicleDataManager::handleConnectionError() {
       std::cerr << "VSS connection lost. Retrying..." << std::endl;
       
       // リトライロジック（エクスポネンシャルバックオフ）
       int retry_count = 0;
       while (retry_count < MAX_RETRIES) {
           std::this_thread::sleep_for(
               std::chrono::seconds(std::pow(2, retry_count))
           );
           
           if (connectToVSS()) {
               subscribeToSpeed();
               return;
           }
           retry_count++;
       }
       
       // 最大リトライ回数超過
       notifyFatalError("Cannot connect to VSS service");
   }
   ```

5. **データキャッシング**
   ```cpp
   double VehicleDataManager::getCurrentSpeed() {
       // 最新データが古すぎる場合の処理
       auto now = std::chrono::system_clock::now();
       auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
           now - last_update_
       );
       
       if (age > std::chrono::seconds(5)) {
           std::cerr << "Warning: Speed data is stale ("
                     << age.count() << "ms old)" << std::endl;
           // オプション: 0を返す、またはエラーを投げる
       }
       
       return current_speed_;
   }
   ```

### ❌ やらなくてよいこと（VSSが担当）

1. **CANプロトコルの解析**
   - CAN IDの識別
   - DBC（CAN Database）ファイルの解析
   - ビッグエンディアン/リトルエンディアン変換
   - ビットフィールドの抽出

2. **OBD-IIプロトコルの処理**
   - PID（Parameter ID）リクエスト
   - OBD-IIレスポンスのパース
   - プロトコルネゴシエーション

3. **車両固有の実装**
   - トヨタ、ホンダ、日産などメーカー別の違いの吸収
   - 車種別のCAN仕様への対応

4. **プロトコルの切り替え**
   - CAN/OBD-IIの選択ロジック
   - フォールバック処理

## 実装例: Vehicle Data Manager

```cpp
// vehicle_data_manager.h
#pragma once

#include <string>
#include <functional>
#include <chrono>
#include <nlohmann/json.hpp>
#include "afb_client.h"

using json = nlohmann::json;

class VehicleDataManager {
public:
    using SpeedCallback = std::function<void(double)>;
    using ErrorCallback = std::function<void(const std::string&)>;
    
    VehicleDataManager();
    ~VehicleDataManager();
    
    // 初期化とクリーンアップ
    bool initialize(const std::string& afb_url);
    void shutdown();
    
    // データアクセス
    double getCurrentSpeed() const;
    bool isDataValid() const;
    
    // コールバック登録
    void setSpeedCallback(SpeedCallback callback);
    void setErrorCallback(ErrorCallback callback);
    
private:
    // VSS接続
    bool connectToVSS();
    void subscribeToSpeed();
    
    // イベントハンドラ
    void handleSpeedUpdate(const json& data);
    void handleConnectionError();
    
    // リトライロジック
    void scheduleReconnect();
    
    // メンバー変数
    AFBClient afb_client_;
    double current_speed_;
    std::chrono::system_clock::time_point last_update_;
    
    SpeedCallback speed_callback_;
    ErrorCallback error_callback_;
    
    bool is_connected_;
    int retry_count_;
    
    static constexpr int MAX_RETRIES = 5;
    static constexpr int DATA_TIMEOUT_MS = 5000;
};
```

```cpp
// vehicle_data_manager.cpp
#include "vehicle_data_manager.h"
#include <iostream>
#include <thread>
#include <cmath>

VehicleDataManager::VehicleDataManager()
    : current_speed_(0.0)
    , is_connected_(false)
    , retry_count_(0) {
}

VehicleDataManager::~VehicleDataManager() {
    shutdown();
}

bool VehicleDataManager::initialize(const std::string& afb_url) {
    std::cout << "Initializing Vehicle Data Manager..." << std::endl;
    
    // AFBクライアント初期化
    if (!afb_client_.connect(afb_url)) {
        std::cerr << "Failed to connect to AFB at " << afb_url << std::endl;
        return false;
    }
    
    // VSS接続
    if (!connectToVSS()) {
        std::cerr << "Failed to connect to VSS service" << std::endl;
        scheduleReconnect();
        return false;  // バックグラウンドでリトライ
    }
    
    // Vehicle.Speed購読
    subscribeToSpeed();
    
    is_connected_ = true;
    return true;
}

void VehicleDataManager::shutdown() {
    if (is_connected_) {
        // 購読解除
        afb_client_.call("vss", "unsubscribe", {
            {"path", "Vehicle.Speed"}
        });
        
        afb_client_.disconnect();
        is_connected_ = false;
    }
}

bool VehicleDataManager::connectToVSS() {
    // VSS pingテスト
    bool success = false;
    
    afb_client_.call("vss", "ping", {}, 
        [&success](const json& response) {
            if (response["status"] == "success") {
                success = true;
            }
        });
    
    return success;
}

void VehicleDataManager::subscribeToSpeed() {
    // Vehicle.Speedイベントを購読
    afb_client_.call("vss", "subscribe", {
        {"path", "Vehicle.Speed"}
    }, [this](const json& response) {
        if (response["status"] == "success") {
            std::cout << "Successfully subscribed to Vehicle.Speed" << std::endl;
        } else {
            std::cerr << "Failed to subscribe: " 
                      << response["error"] << std::endl;
        }
    });
    
    // イベントハンドラ登録
    afb_client_.on_event("vss/Vehicle.Speed", 
        [this](const json& event_data) {
            handleSpeedUpdate(event_data);
        });
}

void VehicleDataManager::handleSpeedUpdate(const json& data) {
    try {
        // VSSデータ構造:
        // {
        //   "path": "Vehicle.Speed",
        //   "value": 65.5,
        //   "unit": "km/h",
        //   "timestamp": 1699964123456
        // }
        
        double speed = data.at("value");
        std::string unit = data.at("unit");
        
        // 単位チェック
        if (unit != "km/h") {
            std::cerr << "Unexpected unit: " << unit << std::endl;
            return;
        }
        
        // 値の妥当性チェック
        if (speed < 0 || speed > 300) {
            std::cerr << "Invalid speed: " << speed << " km/h" << std::endl;
            return;
        }
        
        // データ更新
        current_speed_ = speed;
        last_update_ = std::chrono::system_clock::now();
        
        // コールバック呼び出し
        if (speed_callback_) {
            speed_callback_(speed);
        }
        
    } catch (const json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        if (error_callback_) {
            error_callback_("Failed to parse speed data");
        }
    }
}

double VehicleDataManager::getCurrentSpeed() const {
    return current_speed_;
}

bool VehicleDataManager::isDataValid() const {
    if (!is_connected_) {
        return false;
    }
    
    auto now = std::chrono::system_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - last_update_
    );
    
    return age.count() < DATA_TIMEOUT_MS;
}

void VehicleDataManager::setSpeedCallback(SpeedCallback callback) {
    speed_callback_ = callback;
}

void VehicleDataManager::setErrorCallback(ErrorCallback callback) {
    error_callback_ = callback;
}

void VehicleDataManager::handleConnectionError() {
    is_connected_ = false;
    
    if (error_callback_) {
        error_callback_("VSS connection lost");
    }
    
    scheduleReconnect();
}

void VehicleDataManager::scheduleReconnect() {
    if (retry_count_ >= MAX_RETRIES) {
        std::cerr << "Max retries reached. Giving up." << std::endl;
        if (error_callback_) {
            error_callback_("Cannot reconnect to VSS service");
        }
        return;
    }
    
    // エクスポネンシャルバックオフ
    int delay_seconds = std::pow(2, retry_count_);
    std::cout << "Reconnecting in " << delay_seconds << " seconds..." << std::endl;
    
    std::thread([this, delay_seconds]() {
        std::this_thread::sleep_for(std::chrono::seconds(delay_seconds));
        
        if (connectToVSS()) {
            retry_count_ = 0;
            subscribeToSpeed();
            is_connected_ = true;
            std::cout << "Reconnected to VSS" << std::endl;
        } else {
            retry_count_++;
            scheduleReconnect();
        }
    }).detach();
}
```

## 使用例

```cpp
// main.cpp
#include "vehicle_data_manager.h"
#include "application_controller.h"

int main() {
    VehicleDataManager vehicle_data;
    ApplicationController app_controller;
    
    // Vehicle Data Manager初期化
    if (!vehicle_data.initialize("ws://localhost:1234/api?token=xxx")) {
        std::cerr << "Failed to initialize vehicle data" << std::endl;
        // バックグラウンドでリトライするので続行可能
    }
    
    // 速度変化のコールバック登録
    vehicle_data.setSpeedCallback([&app_controller](double speed) {
        std::cout << "Current speed: " << speed << " km/h" << std::endl;
        app_controller.updateSpeed(speed);
    });
    
    // エラーコールバック登録
    vehicle_data.setErrorCallback([](const std::string& error) {
        std::cerr << "Vehicle data error: " << error << std::endl;
    });
    
    // メインループ
    app_controller.run();
    
    return 0;
}
```

## Configuration Manager

設定ファイル例（`config.json`）:

```json
{
  "afb": {
    "url": "ws://localhost:1234/api",
    "token": "your-auth-token",
    "reconnect_interval_ms": 1000,
    "max_retries": 5
  },
  "vss": {
    "signals": [
      "Vehicle.Speed"
    ],
    "update_rate_hz": 10
  },
  "speed_thresholds": {
    "relaxed_max": 20,
    "normal_max": 60,
    "alert_max": 100,
    "warning_max": 120
  },
  "display": {
    "units": "km/h",
    "theme": "dark",
    "language": "ja",
    "show_speed_number": true,
    "fullscreen": true
  },
  "character": {
    "selected": "default_boy",
    "animation_speed": 1.0,
    "enable_transitions": true
  },
  "logging": {
    "enabled": true,
    "level": "info",
    "log_dir": "/var/log/carspeedboy",
    "max_file_size_mb": 10,
    "max_files": 5
  }
}
```

### Configuration Manager実装例

```cpp
// configuration_manager.h
#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct SpeedThresholds {
    double relaxed_max = 20.0;
    double normal_max = 60.0;
    double alert_max = 100.0;
    double warning_max = 120.0;
};

struct DisplaySettings {
    std::string units = "km/h";
    std::string theme = "dark";
    std::string language = "en";
    bool show_speed_number = true;
    bool fullscreen = false;
};

struct CharacterSettings {
    std::string selected = "default_boy";
    double animation_speed = 1.0;
    bool enable_transitions = true;
};

struct AFBConnectionConfig {
    std::string url = "ws://localhost:1234/api";
    std::string token = "";
    int reconnect_interval_ms = 1000;
    int max_retries = 5;
};

struct LoggingConfig {
    bool enabled = true;
    std::string level = "info";
    std::string log_dir = "/var/log/carspeedboy";
    int max_file_size_mb = 10;
    int max_files = 5;
};

class ConfigurationManager {
public:
    ConfigurationManager();
    
    // 設定の読み込み・保存
    bool loadFromFile(const std::string& config_path);
    bool saveToFile(const std::string& config_path);
    
    // 速度閾値
    SpeedThresholds getSpeedThresholds() const { return speed_thresholds_; }
    void setSpeedThresholds(const SpeedThresholds& thresholds);
    
    // 表示設定
    DisplaySettings getDisplaySettings() const { return display_settings_; }
    void setDisplaySettings(const DisplaySettings& settings);
    
    // キャラクター設定
    CharacterSettings getCharacterSettings() const { return character_settings_; }
    void setCharacterSettings(const CharacterSettings& settings);
    
    // AFB接続設定
    AFBConnectionConfig getAFBConfig() const { return afb_config_; }
    
    // ログ設定
    LoggingConfig getLoggingConfig() const { return logging_config_; }
    
    // デフォルト値にリセット
    void resetToDefaults();
    
private:
    void loadDefaults();
    void parseJSON(const json& config);
    json toJSON() const;
    
    SpeedThresholds speed_thresholds_;
    DisplaySettings display_settings_;
    CharacterSettings character_settings_;
    AFBConnectionConfig afb_config_;
    LoggingConfig logging_config_;
    
    std::string config_file_path_;
};
```

```cpp
// configuration_manager.cpp
#include "configuration_manager.h"
#include <fstream>
#include <iostream>

ConfigurationManager::ConfigurationManager() {
    loadDefaults();
}

void ConfigurationManager::loadDefaults() {
    speed_thresholds_ = SpeedThresholds{};
    display_settings_ = DisplaySettings{};
    character_settings_ = CharacterSettings{};
    afb_config_ = AFBConnectionConfig{};
    logging_config_ = LoggingConfig{};
}

bool ConfigurationManager::loadFromFile(const std::string& config_path) {
    config_file_path_ = config_path;
    
    try {
        std::ifstream file(config_path);
        if (!file.is_open()) {
            std::cerr << "Config file not found: " << config_path 
                      << ". Using defaults." << std::endl;
            return false;
        }
        
        json config;
        file >> config;
        parseJSON(config);
        
        std::cout << "Configuration loaded from: " << config_path << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return false;
    }
}

void ConfigurationManager::parseJSON(const json& config) {
    // AFB設定
    if (config.contains("afb")) {
        auto afb = config["afb"];
        afb_config_.url = afb.value("url", afb_config_.url);
        afb_config_.token = afb.value("token", afb_config_.token);
        afb_config_.reconnect_interval_ms = 
            afb.value("reconnect_interval_ms", afb_config_.reconnect_interval_ms);
        afb_config_.max_retries = afb.value("max_retries", afb_config_.max_retries);
    }
    
    // 速度閾値
    if (config.contains("speed_thresholds")) {
        auto thresholds = config["speed_thresholds"];
        speed_thresholds_.relaxed_max = 
            thresholds.value("relaxed_max", speed_thresholds_.relaxed_max);
        speed_thresholds_.normal_max = 
            thresholds.value("normal_max", speed_thresholds_.normal_max);
        speed_thresholds_.alert_max = 
            thresholds.value("alert_max", speed_thresholds_.alert_max);
        speed_thresholds_.warning_max = 
            thresholds.value("warning_max", speed_thresholds_.warning_max);
    }
    
    // 表示設定
    if (config.contains("display")) {
        auto display = config["display"];
        display_settings_.units = display.value("units", display_settings_.units);
        display_settings_.theme = display.value("theme", display_settings_.theme);
        display_settings_.language = display.value("language", display_settings_.language);
        display_settings_.show_speed_number = 
            display.value("show_speed_number", display_settings_.show_speed_number);
        display_settings_.fullscreen = 
            display.value("fullscreen", display_settings_.fullscreen);
    }
    
    // キャラクター設定
    if (config.contains("character")) {
        auto character = config["character"];
        character_settings_.selected = 
            character.value("selected", character_settings_.selected);
        character_settings_.animation_speed = 
            character.value("animation_speed", character_settings_.animation_speed);
        character_settings_.enable_transitions = 
            character.value("enable_transitions", character_settings_.enable_transitions);
    }
    
    // ログ設定
    if (config.contains("logging")) {
        auto logging = config["logging"];
        logging_config_.enabled = logging.value("enabled", logging_config_.enabled);
        logging_config_.level = logging.value("level", logging_config_.level);
        logging_config_.log_dir = logging.value("log_dir", logging_config_.log_dir);
        logging_config_.max_file_size_mb = 
            logging.value("max_file_size_mb", logging_config_.max_file_size_mb);
        logging_config_.max_files = 
            logging.value("max_files", logging_config_.max_files);
    }
}

bool ConfigurationManager::saveToFile(const std::string& config_path) {
    try {
        std::ofstream file(config_path);
        if (!file.is_open()) {
            std::cerr << "Cannot open config file for writing: " 
                      << config_path << std::endl;
            return false;
        }
        
        json config = toJSON();
        file << config.dump(2);  // Pretty print with 2-space indent
        
        std::cout << "Configuration saved to: " << config_path << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving config: " << e.what() << std::endl;
        return false;
    }
}

json ConfigurationManager::toJSON() const {
    json config;
    
    // AFB設定
    config["afb"] = {
        {"url", afb_config_.url},
        {"token", afb_config_.token},
        {"reconnect_interval_ms", afb_config_.reconnect_interval_ms},
        {"max_retries", afb_config_.max_retries}
    };
    
    // 速度閾値
    config["speed_thresholds"] = {
        {"relaxed_max", speed_thresholds_.relaxed_max},
        {"normal_max", speed_thresholds_.normal_max},
        {"alert_max", speed_thresholds_.alert_max},
        {"warning_max", speed_thresholds_.warning_max}
    };
    
    // 表示設定
    config["display"] = {
        {"units", display_settings_.units},
        {"theme", display_settings_.theme},
        {"language", display_settings_.language},
        {"show_speed_number", display_settings_.show_speed_number},
        {"fullscreen", display_settings_.fullscreen}
    };
    
    // キャラクター設定
    config["character"] = {
        {"selected", character_settings_.selected},
        {"animation_speed", character_settings_.animation_speed},
        {"enable_transitions", character_settings_.enable_transitions}
    };
    
    // ログ設定
    config["logging"] = {
        {"enabled", logging_config_.enabled},
        {"level", logging_config_.level},
        {"log_dir", logging_config_.log_dir},
        {"max_file_size_mb", logging_config_.max_file_size_mb},
        {"max_files", logging_config_.max_files}
    };
    
    return config;
}

void ConfigurationManager::setSpeedThresholds(const SpeedThresholds& thresholds) {
    speed_thresholds_ = thresholds;
    
    // 自動保存
    if (!config_file_path_.empty()) {
        saveToFile(config_file_path_);
    }
}

void ConfigurationManager::setDisplaySettings(const DisplaySettings& settings) {
    display_settings_ = settings;
    
    if (!config_file_path_.empty()) {
        saveToFile(config_file_path_);
    }
}

void ConfigurationManager::setCharacterSettings(const CharacterSettings& settings) {
    character_settings_ = settings;
    
    if (!config_file_path_.empty()) {
        saveToFile(config_file_path_);
    }
}

void ConfigurationManager::resetToDefaults() {
    loadDefaults();
    
    if (!config_file_path_.empty()) {
        saveToFile(config_file_path_);
    }
}
```

### 使用例

```cpp
// main.cpp
#include "configuration_manager.h"
#include "vehicle_data_manager.h"
#include "application_controller.h"

int main(int argc, char* argv[]) {
    // Configuration Manager初期化
    ConfigurationManager config;
    
    // 設定ファイルを読み込み（なければデフォルト値使用）
    std::string config_path = "/etc/carspeedboy/config.json";
    if (argc > 1) {
        config_path = argv[1];
    }
    config.loadFromFile(config_path);
    
    // AFB接続設定を取得
    auto afb_config = config.getAFBConfig();
    
    // Vehicle Data Manager初期化
    VehicleDataManager vehicle_data;
    vehicle_data.initialize(afb_config.url);
    
    // Application Controller初期化
    ApplicationController app_controller(config);
    
    // 速度閾値を適用
    auto thresholds = config.getSpeedThresholds();
    app_controller.setSpeedThresholds(thresholds);
    
    // アプリケーション実行
    return app_controller.run();
}
```

**重要:** プロトコル選択（CAN/OBD-II）の設定は不要。それはVSS bindingの設定（AGLプラットフォーム側）で行われます。

## まとめ

### Data Acquisition Layerの役割（修正後）

| コンポーネント | 責務 | プロトコル依存性 |
|--------------|------|-----------------|
| **Vehicle Data Manager** | VSS APIへの接続、Vehicle.Speed購読、データバリデーション | ❌ なし（完全に抽象化） |
| **Configuration Manager** | アプリ設定の管理（閾値、表示設定など） | ❌ なし |

### VSSによる抽象化の利点

✅ **保守性**: 車両が変わってもアプリコードは不変  
✅ **可搬性**: 異なる車両メーカーで動作  
✅ **シンプル**: プロトコルハンドラが不要  
✅ **標準化**: 単位やデータ型が統一  
✅ **テスタビリティ**: VSS mockで簡単にテスト可能  

### プロトコルの違いはどこで吸収されるか？

**AGL側（プラットフォーム）で吸収:**
```
Vehicle.Speed (VSS標準パス)
      ↓
VSS Binding (AGL提供)
      ↓
  [車両Aの場合]           [車両Bの場合]
  CAN ID: 0x123      CAN ID: 0x456
  Byte 0, Bit 0-7    Byte 2-3, Bit 0-15
      ↓                    ↓
    low-can binding
      ↓                    ↓
   Linux SocketCAN
```

**CarSpeedBoyアプリは`Vehicle.Speed`だけを知っていればOK！**
