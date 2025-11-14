# 次回作業予定

## 優先タスク

### 1. Yoctoビルドレシピの作成
**ディレクトリ:** `meta-carspeedboy/recipes-carspeedboy/carspeedboy/`

**作成ファイル:**
- `carspeedboy_git.bb` - BitBakeレシピファイル

**含めるべき内容:**
```bitbake
SUMMARY = "CarSpeedBoy IVI Application"
LICENSE = "MIT"
DEPENDS = "qtbase qtwayland qtwebsockets nlohmann-json"
RDEPENDS_${PN} = "agl-service-can-low-level agl-vss qtbase qtwayland qtwebsockets"

inherit cmake_qt5

SRC_URI = "git://github.com/yourusername/CarSpeedBoy.git;protocol=https;branch=main"
SRCREV = "${AUTOREV}"

do_install_append() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/carspeedboy ${D}${bindir}/
    
    install -d ${D}${sysconfdir}/carspeedboy
    install -m 0644 ${S}/config/config.json ${D}${sysconfdir}/carspeedboy/
}

FILES_${PN} += "${bindir}/carspeedboy ${sysconfdir}/carspeedboy/*"
```

**確認事項:**
- Qt5依存関係が正しく設定されているか
- AGL VSS/AFBサービスがランタイム依存に含まれているか
- インストールパスが正しいか（`/usr/bin/`, `/etc/carspeedboy/`）

---

### 2. ユニットテストのセットアップ
**ディレクトリ:** `tests/`

**作成ファイル:**
```
tests/
├── CMakeLists.txt
├── test_expression_state_machine.cpp
├── test_configuration_manager.cpp
├── test_vehicle_data_manager.cpp
└── mocks/
    └── mock_websocket.h
```

**CMakeLists.txt に追加:**
```cmake
enable_testing()

find_package(Qt5 REQUIRED COMPONENTS Test)

add_executable(test_expression_state_machine 
    tests/test_expression_state_machine.cpp
    src/business_logic/expression_state_machine.cpp
)
target_link_libraries(test_expression_state_machine Qt5::Test Qt5::Core)
add_test(NAME ExpressionStateMachine COMMAND test_expression_state_machine)
```

**テストケース例:**
- 速度閾値の境界値テスト
- ヒステリシス動作の検証
- 設定ファイルのパース/バリデーション
- WebSocket再接続ロジック

---

### 3. 残りのスタブ実装

#### SpeedMonitor (`src/business_logic/speed_monitor.cpp`)
**実装機能:**
- 移動平均フィルタ（ウィンドウサイズ: 5-10サンプル）
- 異常値検出（範囲外の速度を除外）
- 平滑化された速度の出力

**シグナル:**
```cpp
signals:
    void smoothedSpeedUpdated(double speed_kmh);
```

#### DataLogger (`src/business_logic/data_logger.cpp`)
**実装機能:**
- CSV形式でのログ出力（タイムスタンプ、生速度、平滑化速度、状態）
- ログファイルのローテーション（サイズベース: 10MB）
- 設定可能なログレベル

**ファイル例:**
```
/var/log/carspeedboy/speed_log_20251114.csv
timestamp,raw_speed,smoothed_speed,expression_state
2025-11-14T10:30:00,45.2,44.8,NORMAL
```

#### AlertManager (`src/business_logic/alert_manager.cpp`)
**実装機能:**
- WARNING/SCARED状態での視覚的フィードバック調整
- アラートの優先度管理
- アラート履歴の記録

#### CharacterAnimationEngine (`src/presentation/character_animation_engine.cpp`)
**実装機能:**
- GIFファイルのロード
- ExpressionState → アニメーションファイルのマッピング
- QMLへのプロパティ公開（`Q_PROPERTY`）

**プロパティ例:**
```cpp
Q_PROPERTY(QString currentAnimationPath READ currentAnimationPath NOTIFY animationChanged)
```

---

### 4. QML UIの実装

#### SpeedDisplay.qml
**表示内容:**
- デジタル速度表示（大きな数字）
- 単位表示（km/h）
- スピードメーターグラフィック（オプション）

#### CharacterView.qml
**実装:**
```qml
AnimatedImage {
    id: characterAnimation
    source: characterAnimationEngine.currentAnimationPath
    playing: true
    fillMode: Image.PreserveAspectFit
}
```

#### SettingsDialog.qml
**設定項目:**
- 速度閾値のカスタマイズ（SpinBox）
- テーマ選択（ComboBox: ライト/ダーク）
- ログ有効/無効（CheckBox）

---

## 中期タスク（2-4週間後）

### 5. キャラクターアセットの準備
**必要ファイル:**
```
resources/animations/
├── relaxed.gif    (穏やかな表情)
├── normal.gif     (通常の表情)
├── alert.gif      (注意深い表情)
├── warning.gif    (警告表情)
└── scared.gif     (恐怖表情)
```

**作成方法:**
- ドット絵/アニメーション作成ツール（Aseprite, Piskel等）
- フリー素材の利用（ライセンス確認必須）
- 外注/デザイナーへの依頼

**仕様:**
- サイズ: 256x256 または 512x512
- フレームレート: 12-24 FPS
- ループアニメーション

---

### 6. Widget Packageの作成

#### config.xml
```xml
<?xml version="1.0" encoding="UTF-8"?>
<widget xmlns="http://www.w3.org/ns/widgets" id="carspeedboy" version="1.0.0">
  <name>CarSpeedBoy</name>
  <description>Speed-based character expression IVI app</description>
  <author>Your Name</author>
  <license>MIT</license>
  <icon src="icon.png"/>
  <content src="bin/carspeedboy"/>
  <feature name="urn:AGL:widget:required-permission">
    <param name="urn:AGL:permission:vehicle:speed" value="required"/>
  </feature>
</widget>
```

#### manifest.json
```json
{
  "name": "carspeedboy",
  "version": "1.0.0",
  "required-api": [
    "agl-service-vss"
  ]
}
```

#### パッケージ作成コマンド
```bash
wgtpkg-pack -o carspeedboy.wgt /path/to/build/output
```

---

### 7. ConfigurationManagerの完成

**実装すべきメソッド:**

```cpp
// src/data_acquisition/configuration_manager.cpp

QJsonObject ConfigurationManager::toJSON() const {
    QJsonObject root;
    
    // Speed thresholds
    QJsonObject thresholds;
    thresholds["relaxed_max"] = speed_thresholds_.relaxed_max;
    thresholds["normal_max"] = speed_thresholds_.normal_max;
    thresholds["alert_max"] = speed_thresholds_.alert_max;
    thresholds["warning_max"] = speed_thresholds_.warning_max;
    root["speed_thresholds"] = thresholds;
    
    // AFB connection
    QJsonObject afb;
    afb["url"] = afb_config_.url;
    afb["retry_attempts"] = afb_config_.retry_attempts;
    afb["retry_interval_ms"] = afb_config_.retry_interval_ms;
    root["afb"] = afb;
    
    // Display settings
    QJsonObject display;
    display["units"] = display_settings_.units;
    display["theme"] = display_settings_.theme;
    root["display"] = display;
    
    // Logging
    QJsonObject logging;
    logging["enabled"] = logging_config_.enabled;
    logging["log_dir"] = logging_config_.log_dir;
    logging["level"] = logging_config_.level;
    root["logging"] = logging;
    
    return root;
}

bool ConfigurationManager::saveToFile(const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open config file for writing:" << filepath;
        return false;
    }
    
    QJsonDocument doc(toJSON());
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    qInfo() << "Configuration saved to" << filepath;
    return true;
}
```

---

## 長期タスク（1-2ヶ月後）

### 8. AGL実機/エミュレータでのテスト

**環境構築:**
1. AGL SDKのインストール
2. QEMUエミュレータのセットアップ
3. または実機（Raspberry Pi 4等）へのAGLインストール

**テスト項目:**
- AFB WebSocket接続（`ws://localhost:1234/api`）
- VSS `Vehicle.Speed` データの受信
- 速度変化に応じた表情切り替え
- 設定変更の永続化
- ログファイルの出力
- メモリ使用量/CPU使用率の測定

**パフォーマンス目標:**
- 速度更新レイテンシ < 100ms
- メモリ使用量 < 50MB
- CPU使用率 < 10%（アイドル時）

---

### 9. CI/CDパイプラインの完成

**GitHub Actions追加タスク:**

```yaml
# .github/workflows/ci.yml に追加

- name: Create Widget Package
  run: |
    sudo apt-get install -y agl-app-framework-binder-dev
    cd build
    # Icon preparation
    convert ../resources/icon.png -resize 256x256 icon.png
    # Package assembly
    wgtpkg-pack -o ../carspeedboy.wgt .
    
- name: Upload Widget Artifact
  uses: actions/upload-artifact@v3
  with:
    name: carspeedboy-wgt
    path: carspeedboy.wgt
```

---

## 推奨作業順序

1. **Yoctoレシピ作成**（1-2時間）
2. **ユニットテスト構造作成**（2-3時間）
3. **SpeedMonitor実装**（1-2時間）
4. **DataLogger実装**（2-3時間）
5. **CharacterAnimationEngine実装**（2-3時間）
6. **QML UI実装**（3-4時間）
7. **ConfigurationManager完成**（1時間）
8. **AlertManager実装**（1-2時間）
9. **キャラクターアセット準備**（時間は入手方法による）
10. **Widget Package作成**（1-2時間）
11. **AGL環境でのテスト**（4-8時間）

**推定総作業時間:** 20-30時間

---

## 開発時の注意点

### コーディング規約の遵守
- `.github/copilot-instructions.md` に従う
- `.clang-format` でコード整形
- Doxygen形式のコメントを記載

### テスト駆動開発
- 実装前にテストケースを作成
- `ctest` で全テストが通ることを確認
- カバレッジ目標: 80%以上

### Git運用
- 機能ごとにブランチを作成（`feature/speed-monitor`, `feature/qml-ui`等）
- コミットメッセージは簡潔に（例: `feat: Implement SpeedMonitor with moving average filter`）
- Pull Request作成時にCI/CDが通ることを確認

### ドキュメント更新
- 新機能追加時は `README.md` と `DEVELOPMENT_LOG.md` を更新
- API変更時は該当ヘッダファイルのDoxygen コメントを更新

---

**最終更新:** 2025年11月14日  
**次回作業開始予定:** Yoctoビルドレシピの作成
