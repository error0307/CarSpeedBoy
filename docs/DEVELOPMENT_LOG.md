# CarSpeedBoy 開発ログ

## 2025年11月14日 - プロジェクト初期セットアップ

### 実施した作業

#### 1. アーキテクチャ設計
- **PlantUMLアーキテクチャ図の作成** (`docs/architecture.puml`)
  - ハードウェア抽象化層 → AGLフレームワークサービス → CarSpeedBoyアプリケーションの3層構造
  - Data Acquisition Layer (VSS WebSocket通信)
  - Business Logic Layer (速度監視、状態管理、ログ記録)
  - Presentation Layer (Qt/QMLベースUI)

#### 2. 設計ドキュメント作成
以下のドキュメントを作成し、`docs/`ディレクトリに配置:

- **`AGL_vs_Implementation.md`** - AGL標準機能とアプリケーション実装の責任分界
- **`DataAcquisitionLayer_Detail.md`** - データ取得層の詳細設計
- **`AGL_Protocol_Configuration.md`** - プロトコル設定の管理方法
- **`AGL_AppStore_and_Flutter.md`** - アプリ配布とFlutter実装の検討結果
- **`README.md`** - プロジェクト概要

**重要な設計決定:**
- アプリケーション層ではCAN/OBD-IIプロトコルを直接扱わない（VSSが抽象化）
- プロトコル設定は`/etc/afb/`でAGLプラットフォーム側が管理
- アプリケーション設定は`config/config.json`で管理
- 配布形式は`.wgt` Widget Package形式
- 実装言語はQt/QML（C++17）を採用（Flutterは非推奨）

#### 3. プロジェクト構造の構築

**ディレクトリ構成:**
```
CarSpeedBoy/
├── .github/
│   ├── workflows/
│   │   └── ci.yml                    # GitHub Actions CI/CD
│   └── copilot-instructions.md        # AIコーディングガイドライン
├── docs/                              # ドキュメント類
├── config/
│   └── config.json                    # アプリケーション設定
├── include/                           # ヘッダファイル
│   ├── application_controller.h
│   ├── data_acquisition/
│   │   ├── vehicle_data_manager.h
│   │   └── configuration_manager.h
│   ├── business_logic/
│   │   ├── expression_state_machine.h
│   │   ├── speed_monitor.h
│   │   ├── data_logger.h
│   │   └── alert_manager.h
│   └── presentation/
│       └── character_animation_engine.h
├── src/                               # 実装ファイル
│   ├── main.cpp
│   ├── application_controller.cpp
│   ├── data_acquisition/
│   ├── business_logic/
│   └── presentation/
├── qml/                               # QML UIファイル
│   ├── main.qml
│   ├── SpeedDisplay.qml
│   ├── CharacterView.qml
│   └── SettingsDialog.qml
├── resources/
│   └── resources.qrc                  # Qtリソースファイル
├── CMakeLists.txt                     # ビルド設定
├── .clang-format                      # コードフォーマット設定
├── .gitignore
└── LICENSE
```

#### 4. 実装済みコンポーネント

**完全実装:**
- **ApplicationController** - アプリケーション全体の制御、コンポーネント初期化
- **VehicleDataManager** - VSS WebSocket接続、`Vehicle.Speed`サブスクリプション、再接続ロジック
- **ExpressionStateMachine** - 速度→表情状態マッピング（5状態: RELAXED/NORMAL/ALERT/WARNING/SCARED）、ヒステリシス制御
- **ConfigurationManager** - 設定ファイル読み込み（保存は未実装）

**スタブ実装（ヘッダのみ/最小限の実装）:**
- SpeedMonitor
- DataLogger
- AlertManager
- CharacterAnimationEngine
- QMLファイル（main.qml, SpeedDisplay.qml, CharacterView.qml, SettingsDialog.qml）

#### 5. ビルドシステム
- **CMake 3.16+** - Qt5統合、リソースコンパイル、インストールターゲット
- **依存関係:** Qt5 (Core, Gui, Qml, Quick, WebSockets), nlohmann/json

#### 6. CI/CD設定
- **GitHub Actions** (`.github/workflows/ci.yml`)
  - ビルドジョブ（CMake + Ninja）
  - テストジョブ（ctest）
  - Lintジョブ（clang-format, clang-tidy）
  - パッケージジョブ（.wgt作成 - スタブ）

#### 7. 開発ツール設定
- **Copilot Instructions** (`.github/copilot-instructions.md`)
  - C++17/Qt5コーディング規約
  - プロジェクト固有ルール（VSS専用、プロトコルハンドラ禁止）
  - 速度閾値定義（0-20/21-60/61-100/101-120/121+ km/h）
  - セキュリティ/パフォーマンスガイドライン
  - ファイルテンプレート

- **Clang-Format設定** (`.clang-format`)
  - Google C++スタイルベース
  - インデント4スペース、100カラム制限
  - Qt/C++ヘッダの自動ソート

#### 8. バージョン管理
- **Git設定** (`.gitignore`)
  - ビルド成果物、IDEファイル、認証情報を除外

---

### 技術スタック

| カテゴリ | 技術 |
|---------|------|
| プラットフォーム | Automotive Grade Linux (AGL) |
| 言語 | C++17 |
| フレームワーク | Qt5 (Core, Gui, Qml, Quick, WebSockets) |
| ビルドシステム | CMake 3.16+ |
| データプロトコル | VSS (Vehicle Signal Specification) |
| 通信 | AFB WebSocket API |
| パッケージ形式 | .wgt (Widget Package) |
| CI/CD | GitHub Actions |
| JSONライブラリ | nlohmann/json |

---

### 次のステップ

#### 優先度: 高

1. **Yoctoビルドレシピの作成** (`meta-carspeedboy/`)
   - BitBakeレシピファイル作成
   - Qt5依存関係の定義
   - ランタイム依存関係（agl-service-can-low-level, agl-vss）
   - インストールルール（`/usr/bin/carspeedboy`, `/etc/carspeedboy/`）

2. **ユニットテストの実装**
   - `tests/` ディレクトリ構造作成
   - Qt Testフレームワークセットアップ
   - VehicleDataManagerのモック作成
   - ExpressionStateMachineのテストケース
   - ConfigurationManagerのテストケース

3. **残りのビジネスロジック実装**
   - **SpeedMonitor** - 移動平均フィルタでノイズ除去
   - **DataLogger** - CSV/JSON形式でのログ出力、ローテーション機能
   - **AlertManager** - 視覚的警告の調整

#### 優先度: 中

4. **プレゼンテーション層の実装**
   - **CharacterAnimationEngine** - スプライト/GIF管理、状態ベースのアニメーション切り替え
   - **QML UI実装**
     - SpeedDisplay.qml - スピードメーター表示
     - CharacterView.qml - キャラクターアニメーション（AnimatedImage）
     - SettingsDialog.qml - 速度閾値のカスタマイズUI

5. **ConfigurationManagerの完成**
   - `toJSON()` メソッド実装
   - `saveToFile()` メソッド実装
   - ユーザー設定の永続化

6. **キャラクターアセットの作成/取得**
   - 5つの表情状態用GIFファイル
     - relaxed.gif (0-20 km/h)
     - normal.gif (21-60 km/h)
     - alert.gif (61-100 km/h)
     - warning.gif (101-120 km/h)
     - scared.gif (121+ km/h)

#### 優先度: 低

7. **Widget Packageファイルの作成**
   - `config.xml` - アプリメタデータ、VSS権限
   - `manifest.json` - AFB API要求仕様
   - `icon.png` - アプリケーションアイコン
   - `.wgt`パッケージのアセンブル（wgtpkg-pack）

8. **AGL環境でのテスト**
   - 実機またはQEMUエミュレータへのデプロイ
   - AFB/VSS接続の検証
   - 速度データフローの検証
   - パフォーマンステスト（更新レイテンシ < 100ms）

---

### 既知の課題

1. **ConfigurationManager** - 保存機能が未実装（TODOコメント有り）
2. **VehicleDataManager** - 実際のAFBエンドポイントでのテストが必要
3. **ユニットテスト** - テストディレクトリがCMakeLists.txtに未追加
4. **Widget Package** - GitHub Actionsのパッケージジョブがスタブ状態
5. **キャラクターアセット** - GIFファイルが未作成

---

### 設計上の重要な制約

1. **アプリケーション層ではプロトコルを扱わない**
   - CAN/OBD-II の実装は禁止
   - VSS標準パス（`Vehicle.Speed`）のみ使用

2. **設定の責任分界**
   - アプリ設定: `config/config.json` （速度閾値、テーマ、ログ設定）
   - プラットフォーム設定: `/etc/afb/` （CAN ID、DBCファイル、通信プロトコル）

3. **パフォーマンス要件**
   - 速度更新レイテンシ < 100ms
   - 組み込み環境向けリソース最適化

4. **セキュリティ**
   - 認証情報のハードコーディング禁止
   - AFB認証トークンの使用
   - 全入力データのバリデーション

---

### 参考リンク

- [AGL Documentation](https://docs.automotivelinux.org/)
- [VSS Specification](https://covesa.github.io/vehicle_signal_specification/)
- [Qt5 Documentation](https://doc.qt.io/qt-5/)
- [CMake Documentation](https://cmake.org/documentation/)

---

**最終更新:** 2025年11月14日  
**次回作業予定:** Yoctoビルドレシピの作成、ユニットテストのセットアップ
