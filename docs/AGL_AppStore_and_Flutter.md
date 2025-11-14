# AGLアプリケーション配布とFlutter実装

## AGLにおけるアプリ配布の仕組み

### 現状のAGLアプリ配布方法

AGLには**App Storeのような統一されたアプリストアは現時点では存在しません**。ただし、いくつかの配布・インストール方法があります。

---

## 1. AGLアプリケーションの配布方法

### 方法A: Widget Package（.wgt形式）【推奨】

AGLの標準的なアプリケーションパッケージ形式です。

```
┌─────────────────────────────────────────────────────────────┐
│ CarSpeedBoy.wgt (Widgetパッケージ)                          │
├─────────────────────────────────────────────────────────────┤
│ ├── config.xml              (アプリメタデータ)               │
│ ├── bin/                                                     │
│ │   └── carspeedboy         (実行ファイル)                  │
│ ├── lib/                    (共有ライブラリ)                │
│ ├── data/                                                    │
│ │   ├── characters/         (キャラクター画像)              │
│ │   └── config.json         (デフォルト設定)                │
│ ├── icon.png                (アプリアイコン)                │
│ └── manifest.json           (パーミッション定義)            │
└─────────────────────────────────────────────────────────────┘
```

#### Widget Package作成例

**config.xml:**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<widget xmlns="http://www.w3.org/ns/widgets" 
        id="jp.example.carspeedboy" 
        version="1.0.0">
    <name>CarSpeedBoy</name>
    <description>Speed monitoring IVI app with animated character</description>
    <author>Your Name</author>
    <license>MIT</license>
    <icon src="icon.png"/>
    <content src="bin/carspeedboy" type="application/vnd.agl.native"/>
    <feature name="urn:AGL:widget:required-api">
        <param name="vss" value="ws"/>
        <param name="afb-helpers" value="link"/>
    </feature>
    <feature name="urn:AGL:widget:required-permission">
        <param name="urn:AGL:permission:vehicle:speed" value="required"/>
    </feature>
</widget>
```

**manifest.json:**
```json
{
  "permissions": [
    "urn:AGL:permission:vehicle:speed",
    "urn:AGL:permission:display:window"
  ],
  "required-api": [
    "vss",
    "afb-helpers"
  ]
}
```

#### パッケージのビルド

```bash
# wgtpkgツールでパッケージ化
wgtpkg-pack -o CarSpeedBoy.wgt carspeedboy/

# または
cd carspeedboy/
zip -r ../CarSpeedBoy.wgt .
```

---

### 方法B: App Framework Manager経由でのインストール

```bash
# .wgtファイルのインストール
afm-util install /path/to/CarSpeedBoy.wgt

# インストール済みアプリ一覧
afm-util list

# アプリ起動
afm-util start jp.example.carspeedboy@1.0

# アプリ停止
afm-util stop jp.example.carspeedboy@1.0

# アンインストール
afm-util remove jp.example.carspeedboy@1.0
```

---

### 方法C: OTA (Over-The-Air) アップデート

車両に統合された後のアップデート用。

```
┌──────────────────┐
│  Cloud Server    │
│  (OTA Backend)   │
└────────┬─────────┘
         │ HTTPS
         │ CarSpeedBoy_v1.1.0.wgt
         ▼
┌──────────────────┐
│  Vehicle ECU     │
│  ┌─────────────┐ │
│  │ SOTA Client │ │
│  │ (libaktualizr)│ │
│  └──────┬──────┘ │
│         │         │
│         ▼         │
│  AFM (App        │
│   Framework      │
│   Manager)       │
└──────────────────┘
```

**libaktualizr** を使ったOTAアップデート:
```bash
# OTA設定ファイル
cat > /etc/sota/sota.toml << EOF
[uptane]
director_server = "https://ota.example.com/director"
repo_server = "https://ota.example.com/repo"

[storage]
type = "sqlite"
path = "/var/sota/sql.db"
EOF

# OTAクライアント起動
aktualizr-lite
```

---

### 方法D: Yoctoイメージに統合（プリインストール）

車両出荷時にプリインストールする場合。

**meta-carspeedboy/recipes-app/carspeedboy/carspeedboy.bb:**
```bitbake
SUMMARY = "CarSpeedBoy IVI Application"
LICENSE = "MIT"

SRC_URI = "git://github.com/yourrepo/carspeedboy.git;protocol=https;branch=main"
SRCREV = "${AUTOREV}"

inherit cmake_qt5

DEPENDS = "qtbase qtwayland afb-binding nlohmann-json"
RDEPENDS_${PN} = "agl-service-can-low-level agl-vss"

do_install_append() {
    # Widget packageとしてインストール
    install -d ${D}${afm_appdir}/carspeedboy
    install -m 0755 ${B}/bin/carspeedboy ${D}${afm_appdir}/carspeedboy/
    install -m 0644 ${S}/config.xml ${D}${afm_appdir}/carspeedboy/
}

FILES_${PN} += "${afm_appdir}/carspeedboy/*"
```

**イメージレシピに追加:**
```bitbake
# meta-carspeedboy/recipes-platform/images/agl-demo-platform.bbappend
IMAGE_INSTALL_append = " carspeedboy"
```

---

## 2. 将来的なApp Store構想

### AGL App Store（開発中）

AGL Community は**App Storeのような仕組み**を検討中ですが、まだ標準化されていません。

**想定される仕組み:**
```
┌─────────────────────────────────────────────────────────────┐
│ AGL App Store (Web Portal)                                   │
│  https://apps.automotivelinux.org                            │
├─────────────────────────────────────────────────────────────┤
│ - アプリ一覧                                                  │
│ - レビュー・評価                                              │
│ - セキュリティ検証済みバッジ                                  │
│ - カテゴリ分類 (Navigation, Entertainment, Utility...)      │
└─────────────────────────────────────────────────────────────┘
                            │
                            │ Download .wgt
                            ▼
┌─────────────────────────────────────────────────────────────┐
│ Vehicle IVI System                                           │
│  ┌────────────────────────────────────────────────────────┐ │
│  │ AGL App Installer (GUI)                                │ │
│  │  - ダウンロード                                         │ │
│  │  - 署名検証                                             │ │
│  │  - インストール                                         │ │
│  │  - パーミッション確認                                   │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

**現実的な代替案:**
- 車両メーカー独自のApp Store（例: Tesla, Rivian）
- OEM提携のマーケットプレイス
- エンタープライズ向けプライベートストア

---

## 3. 実装言語の選択肢

### AGLで推奨される言語・フレームワーク

| 言語/フレームワーク | サポート状況 | 推奨度 | 備考 |
|-------------------|------------|-------|------|
| **C++** | ✅ 完全サポート | ⭐⭐⭐⭐⭐ | AGLのネイティブ言語 |
| **Qt (C++/QML)** | ✅ 完全サポート | ⭐⭐⭐⭐⭐ | IVI開発の標準 |
| **C** | ✅ 完全サポート | ⭐⭐⭐⭐ | システムレベル開発 |
| **HTML5/JavaScript** | ✅ サポート | ⭐⭐⭐⭐ | WebアプリとしてElectron風 |
| **Python** | ⚠️ 限定サポート | ⭐⭐⭐ | プロトタイピング用 |
| **Rust** | ⚠️ コミュニティサポート | ⭐⭐⭐ | 将来性あり |
| **Flutter** | ⚠️ 実験的 | ⭐⭐ | 可能だが非公式 |

---

## 4. Flutter実装の可能性

### ✅ Flutter on AGLは技術的に可能

Flutterは**Wayland対応**があるため、AGLで動作させることは可能です。

```
┌─────────────────────────────────────────────────────────────┐
│ Flutter Application (Dart)                                   │
│  ├── lib/main.dart                                           │
│  ├── lib/vehicle_data_service.dart                          │
│  └── pubspec.yaml                                            │
└──────────────────┬──────────────────────────────────────────┘
                   │
                   │ Flutter Engine
                   ▼
┌─────────────────────────────────────────────────────────────┐
│ Flutter Embedder (Linux/Wayland)                             │
│  - flutter-wayland embedder                                  │
│  - Skia rendering                                            │
└──────────────────┬──────────────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────────────┐
│ AGL Wayland Compositor (Weston)                              │
└─────────────────────────────────────────────────────────────┘
```

### Flutter実装の手順

#### 1. Flutter Embedder for Waylandのセットアップ

```bash
# Flutter Engineのビルド（Wayland対応）
git clone https://github.com/flutter/engine.git
cd engine
./flutter/tools/gn --linux-cpu x64 --embedder-for-target --runtime-mode=release
ninja -C out/linux_release_x64
```

#### 2. Yoctoレシピの作成

**meta-carspeedboy/recipes-devtools/flutter/flutter-engine_git.bb:**
```bitbake
SUMMARY = "Flutter Engine for AGL"
LICENSE = "BSD-3-Clause"

DEPENDS = "wayland libdrm libgbm"

SRC_URI = "git://github.com/flutter/engine.git;protocol=https;branch=main"

inherit cmake

do_install() {
    install -d ${D}${libdir}
    install -m 0755 ${B}/libflutter_engine.so ${D}${libdir}/
    
    install -d ${D}${includedir}/flutter
    install -m 0644 ${S}/shell/platform/embedder/embedder.h \
        ${D}${includedir}/flutter/
}
```

#### 3. FlutterアプリからAFB APIを呼び出す

**pubspec.yaml:**
```yaml
dependencies:
  flutter:
    sdk: flutter
  web_socket_channel: ^2.4.0
  json_annotation: ^4.8.0

dev_dependencies:
  build_runner: ^2.4.0
  json_serializable: ^6.6.0
```

**lib/vehicle_data_service.dart:**
```dart
import 'package:web_socket_channel/web_socket_channel.dart';
import 'dart:convert';

class VehicleDataService {
  late WebSocketChannel _channel;
  Function(double)? onSpeedUpdate;
  
  Future<void> connect(String url, String token) async {
    _channel = WebSocketChannel.connect(
      Uri.parse('$url?token=$token')
    );
    
    // VSS subscribeリクエスト
    _channel.sink.add(jsonEncode({
      'api': 'vss',
      'verb': 'subscribe',
      'args': {
        'path': 'Vehicle.Speed'
      }
    }));
    
    // イベント受信
    _channel.stream.listen((message) {
      final data = jsonDecode(message);
      
      if (data['event'] == 'vss/Vehicle.Speed') {
        final speed = data['data']['value'] as double;
        onSpeedUpdate?.call(speed);
      }
    });
  }
  
  void dispose() {
    _channel.sink.close();
  }
}
```

**lib/main.dart:**
```dart
import 'package:flutter/material.dart';
import 'vehicle_data_service.dart';

void main() {
  runApp(const CarSpeedBoyApp());
}

class CarSpeedBoyApp extends StatelessWidget {
  const CarSpeedBoyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'CarSpeedBoy',
      theme: ThemeData.dark(),
      home: const SpeedScreen(),
    );
  }
}

class SpeedScreen extends StatefulWidget {
  const SpeedScreen({Key? key}) : super(key: key);

  @override
  State<SpeedScreen> createState() => _SpeedScreenState();
}

class _SpeedScreenState extends State<SpeedScreen> {
  final _vehicleDataService = VehicleDataService();
  double _currentSpeed = 0.0;
  String _expression = 'relaxed';
  
  @override
  void initState() {
    super.initState();
    
    // AFB接続
    _vehicleDataService.connect(
      'ws://localhost:1234/api',
      'your-token'
    );
    
    // 速度更新コールバック
    _vehicleDataService.onSpeedUpdate = (speed) {
      setState(() {
        _currentSpeed = speed;
        _expression = _getExpression(speed);
      });
    };
  }
  
  String _getExpression(double speed) {
    if (speed <= 20) return 'relaxed';
    if (speed <= 60) return 'normal';
    if (speed <= 100) return 'alert';
    if (speed <= 120) return 'warning';
    return 'scared';
  }
  
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.black,
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            // キャラクターアニメーション
            Image.asset(
              'assets/characters/$_expression.gif',
              width: 300,
              height: 300,
            ),
            const SizedBox(height: 40),
            // 速度表示
            Text(
              '${_currentSpeed.toStringAsFixed(1)} km/h',
              style: const TextStyle(
                fontSize: 48,
                fontWeight: FontWeight.bold,
                color: Colors.white,
              ),
            ),
          ],
        ),
      ),
    );
  }
  
  @override
  void dispose() {
    _vehicleDataService.dispose();
    super.dispose();
  }
}
```

---

### ⚠️ Flutterの課題と制約

#### 1. パフォーマンス
- **ネイティブC++/Qtと比較して重い**
  - Flutter Engineのオーバーヘッド
  - Dartランタイムのメモリ使用量
  - 組み込み環境での最適化が不十分

#### 2. AGLとの統合
- **非公式サポート**
  - AGLのドキュメントにFlutterの記載なし
  - トラブルシューティングがコミュニティ頼み
  - Yoctoレシピを自前で作成する必要

#### 3. ハードウェアアクセス
- **Platform Channelが必要**
  ```dart
  // Dart側
  static const platform = MethodChannel('jp.example.carspeedboy/vehicle');
  
  Future<double> getSpeed() async {
    try {
      final double speed = await platform.invokeMethod('getSpeed');
      return speed;
    } catch (e) {
      return 0.0;
    }
  }
  ```
  
  ```cpp
  // C++側 (Platform Channel実装)
  class VehiclePlugin {
  public:
    void HandleMethodCall(const MethodCall& call, 
                         std::unique_ptr<MethodResult> result) {
      if (call.method_name() == "getSpeed") {
        double speed = vehicle_data_manager_->getCurrentSpeed();
        result->Success(speed);
      }
    }
  };
  ```

#### 4. ビルドサイズ
- **Flutter Engineが大きい**
  - libflutter_engine.so: 約20-30MB
  - Qtアプリ（約5-10MB）と比較して大きい
  - 組み込みストレージに影響

---

## 5. 推奨実装方法

### 🏆 推奨: Qt/QML (C++)

**理由:**
- ✅ AGLの標準フレームワーク
- ✅ 豊富なドキュメント・サンプル
- ✅ 最適化されたパフォーマンス
- ✅ Wayland完全対応
- ✅ 車載向けツール充実（Qt Automotive Suite）
- ✅ AFB bindingとの統合が容易

**実装例:**
```qml
// Main.qml
import QtQuick 2.15
import QtQuick.Window 2.15
import jp.example.carspeedboy 1.0

Window {
    visible: true
    width: 1920
    height: 1080
    color: "black"
    
    VehicleDataService {
        id: vehicleData
        onSpeedChanged: {
            speedText.text = speed.toFixed(1) + " km/h"
            character.updateExpression(speed)
        }
    }
    
    Column {
        anchors.centerIn: parent
        spacing: 40
        
        AnimatedImage {
            id: character
            source: "qrc:/characters/" + expression + ".gif"
            width: 300
            height: 300
            
            property string expression: "relaxed"
            
            function updateExpression(speed) {
                if (speed <= 20) expression = "relaxed"
                else if (speed <= 60) expression = "normal"
                else if (speed <= 100) expression = "alert"
                else if (speed <= 120) expression = "warning"
                else expression = "scared"
            }
        }
        
        Text {
            id: speedText
            text: "0.0 km/h"
            color: "white"
            font.pixelSize: 48
            font.bold: true
        }
    }
}
```

---

### 🥈 次点: HTML5/JavaScript (Electron風)

**理由:**
- ✅ Web技術が使える
- ✅ 開発速度が速い
- ✅ クロスプラットフォーム
- ⚠️ パフォーマンスはやや劣る

**AGLでのHTML5アプリ:**
```html
<!-- index.html -->
<!DOCTYPE html>
<html>
<head>
    <title>CarSpeedBoy</title>
    <style>
        body {
            background: black;
            color: white;
            text-align: center;
            font-family: Arial;
        }
        #character {
            width: 300px;
            height: 300px;
        }
        #speed {
            font-size: 48px;
            font-weight: bold;
        }
    </style>
</head>
<body>
    <img id="character" src="characters/relaxed.gif">
    <div id="speed">0.0 km/h</div>
    
    <script>
        const ws = new WebSocket('ws://localhost:1234/api?token=xxx');
        
        ws.onopen = () => {
            ws.send(JSON.stringify({
                api: 'vss',
                verb: 'subscribe',
                args: { path: 'Vehicle.Speed' }
            }));
        };
        
        ws.onmessage = (event) => {
            const data = JSON.parse(event.data);
            if (data.event === 'vss/Vehicle.Speed') {
                const speed = data.data.value;
                updateSpeed(speed);
            }
        };
        
        function updateSpeed(speed) {
            document.getElementById('speed').textContent = 
                speed.toFixed(1) + ' km/h';
            
            let expression = 'relaxed';
            if (speed > 120) expression = 'scared';
            else if (speed > 100) expression = 'warning';
            else if (speed > 60) expression = 'alert';
            else if (speed > 20) expression = 'normal';
            
            document.getElementById('character').src = 
                `characters/${expression}.gif`;
        }
    </script>
</body>
</html>
```

---

## まとめ

### App Store的な配布

| 方法 | 現状 | 将来性 |
|------|------|--------|
| **Widget Package (.wgt)** | ✅ 利用可能 | ⭐⭐⭐⭐⭐ |
| **OEM独自App Store** | ✅ 一部実装済み | ⭐⭐⭐⭐ |
| **統一AGL App Store** | ❌ 未実装 | ⭐⭐⭐ |
| **OTA アップデート** | ✅ 利用可能 | ⭐⭐⭐⭐⭐ |

### 実装言語の推奨順位

1. **Qt/QML (C++)** ⭐⭐⭐⭐⭐ - 最も推奨
2. **HTML5/JavaScript** ⭐⭐⭐⭐ - 開発速度重視
3. **Flutter** ⭐⭐ - 実験的、非推奨（パフォーマンス・サポート面で課題）

### 結論

**CarSpeedBoyには Qt/QML (C++) での実装を強く推奨します。**
- AGLとの完全な互換性
- 最適なパフォーマンス
- 充実したドキュメントとサポート
- Widget Packageとして配布可能

Flutterは技術的に可能ですが、**プロダクション環境では推奨されません**。
