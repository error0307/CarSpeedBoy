# AGLにおけるプロトコル設定

## 概要

車速データの取得プロトコル（CAN, OBD-II等）は、**AGL Framework Services側で設定**され、**CarSpeedBoyアプリケーションは設定不要**です。

## プロトコル設定の階層

```
┌─────────────────────────────────────────────────────────────┐
│ CarSpeedBoy Application                                      │
│                                                               │
│  vehicle_data.subscribe("Vehicle.Speed");                    │
│                                                               │
│  ↑ アプリは "Vehicle.Speed" だけを知っている                 │
│  ↑ プロトコルは意識しない                                     │
└─────────────────────────────────────────────────────────────┘
                            │
                            │ AFB WebSocket API
                            ▼
┌─────────────────────────────────────────────────────────────┐
│ AGL Framework Services (プラットフォーム側)                   │
│                                                               │
│ ┌───────────────────────────────────────────────────────┐   │
│ │ VSS Binding                                           │   │
│ │  - Vehicle.Speed のマッピング定義を読み込む           │   │
│ │  - 設定ファイル: vss_config.json                      │   │
│ │                                                       │   │
│ │  "Vehicle.Speed" -> CAN signal "EngineSpeed.0x123"   │   │
│ └───────────────┬───────────────────────────────────────┘   │
│                 │                                             │
│                 ▼                                             │
│ ┌───────────────────────────────────────────────────────┐   │
│ │ low-can Binding                                       │   │
│ │  - CAN設定ファイル: can_config.json                   │   │
│ │  - どのCANバスを使うか (can0, can1, vcan0)            │   │
│ │  - CAN IDフィルタリング                               │   │
│ │  - DBC (CAN Database) ファイルの読み込み             │   │
│ └───────────────┬───────────────────────────────────────┘   │
└─────────────────┼───────────────────────────────────────────┘
                  │
                  ▼
        ┌──────────────────┐
        │ Linux SocketCAN   │
        │ (can0, can1, etc) │
        └──────────────────┘
                  │
                  ▼
        ┌──────────────────┐
        │ CAN Hardware      │
        └──────────────────┘
```

---

## 設定ファイルの配置

### 1. VSS Binding設定 (`/etc/afb/vss_config.json`)

VSSの標準パスと実際のCAN信号のマッピングを定義します。

```json
{
  "vss_mapping": {
    "Vehicle.Speed": {
      "source": "low-can",
      "signal_name": "vehicle_speed",
      "can_id": "0x123",
      "byte_order": "little_endian",
      "offset": 0,
      "length": 8,
      "unit": "km/h",
      "scale": 1.0,
      "min": 0,
      "max": 250
    },
    "Vehicle.Powertrain.CombustionEngine.Speed": {
      "source": "low-can",
      "signal_name": "engine_rpm",
      "can_id": "0x456",
      "byte_order": "big_endian",
      "offset": 0,
      "length": 16,
      "unit": "rpm",
      "scale": 0.25,
      "min": 0,
      "max": 8000
    }
  }
}
```

**この設定により:**
- `Vehicle.Speed` → CAN ID 0x123から取得
- アプリは`Vehicle.Speed`を購読するだけでデータが得られる

---

### 2. low-can Binding設定 (`/etc/afb/can_config.json`)

CANバスの物理的な設定を定義します。

```json
{
  "can_devices": [
    {
      "name": "hs",
      "device": "can0",
      "baudrate": 500000,
      "enabled": true
    },
    {
      "name": "ls",
      "device": "can1",
      "baudrate": 125000,
      "enabled": false
    }
  ],
  "can_messages": [
    {
      "id": 291,
      "name": "VehicleSpeed",
      "bus": "hs",
      "is_extended": false,
      "is_fd": false,
      "signals": [
        {
          "name": "vehicle_speed",
          "bit_position": 0,
          "bit_size": 8,
          "factor": 1.0,
          "offset": 0,
          "unit": "km/h"
        }
      ]
    },
    {
      "id": 1122,
      "name": "EngineData",
      "bus": "hs",
      "signals": [
        {
          "name": "engine_rpm",
          "bit_position": 0,
          "bit_size": 16,
          "factor": 0.25,
          "offset": 0,
          "unit": "rpm"
        }
      ]
    }
  ]
}
```

---

### 3. DBC (CAN Database) ファイル (`/etc/can/vehicle.dbc`)

より複雑なCAN定義にはDBCファイルを使用することもできます。

```
VERSION ""

NS_ :
    NS_DESC_
    CM_
    BA_DEF_
    BA_
    VAL_
    CAT_DEF_
    CAT_
    FILTER
    BA_DEF_DEF_
    EV_DATA_
    ENVVAR_DATA_
    SGTYPE_
    SGTYPE_VAL_
    BA_DEF_SGTYPE_
    BA_SGTYPE_
    SIG_TYPE_REF_
    VAL_TABLE_
    SIG_GROUP_
    SIG_VALTYPE_
    SIGTYPE_VALTYPE_
    BO_TX_BU_
    BA_DEF_REL_
    BA_REL_
    BA_SGTYPE_REL_
    SG_MUL_VAL_

BS_:

BU_: ECU1 ECU2 Gateway

BO_ 291 VehicleSpeed: 8 ECU1
 SG_ vehicle_speed : 0|8@1+ (1,0) [0|250] "km/h" Gateway

BO_ 1122 EngineData: 8 ECU1
 SG_ engine_rpm : 0|16@1+ (0.25,0) [0|8000] "rpm" Gateway
 SG_ coolant_temp : 16|8@1+ (1,-40) [-40|215] "C" Gateway
```

**low-can bindingでDBCファイルを指定:**
```json
{
  "dbc_file": "/etc/can/vehicle.dbc",
  "can_devices": [...]
}
```

---

## プロトコル切り替え方法

### ケース1: CANからOBD-IIへの切り替え

#### Step 1: OBD-II Binding設定 (`/etc/afb/obd_config.json`)

```json
{
  "obd_device": "/dev/ttyUSB0",
  "protocol": "ISO15765",
  "baudrate": 500000,
  "pids": {
    "0x0D": {
      "name": "vehicle_speed",
      "description": "Vehicle Speed",
      "unit": "km/h",
      "formula": "A"
    },
    "0x0C": {
      "name": "engine_rpm",
      "description": "Engine RPM",
      "unit": "rpm",
      "formula": "((A*256)+B)/4"
    }
  }
}
```

#### Step 2: VSS Binding設定を更新

```json
{
  "vss_mapping": {
    "Vehicle.Speed": {
      "source": "obd-ii",        // ← CANからOBD-IIに変更
      "pid": "0x0D",
      "unit": "km/h",
      "poll_rate_ms": 100
    }
  }
}
```

#### Step 3: サービスの再起動

```bash
# AFBサービスの再起動（設定を再読み込み）
systemctl restart afb-daemon
```

**CarSpeedBoyアプリケーションは何も変更不要！**

---

### ケース2: 複数プロトコルの混在

車両によっては、一部の信号はCANから、他はOBD-IIから取得することもあります。

```json
{
  "vss_mapping": {
    "Vehicle.Speed": {
      "source": "low-can",       // CANから取得
      "signal_name": "vehicle_speed",
      "can_id": "0x123"
    },
    "Vehicle.OBD.FuelLevel": {
      "source": "obd-ii",        // OBD-IIから取得
      "pid": "0x2F",
      "unit": "%"
    },
    "Vehicle.Cabin.HVAC.Temperature": {
      "source": "mqtt",          // 別のプロトコル
      "topic": "vehicle/cabin/temp"
    }
  }
}
```

---

## 実際の設定手順（車両統合時）

### 開発環境（仮想CAN）での設定

```bash
# 1. 仮想CANデバイスの作成
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0

# 2. low-can binding設定
cat > /etc/afb/can_config.json << EOF
{
  "can_devices": [
    {
      "name": "virtual",
      "device": "vcan0",
      "enabled": true
    }
  ]
}
EOF

# 3. VSS mapping設定
cat > /etc/afb/vss_config.json << EOF
{
  "vss_mapping": {
    "Vehicle.Speed": {
      "source": "low-can",
      "signal_name": "vehicle_speed",
      "can_id": "0x123"
    }
  }
}
EOF

# 4. AFBサービス起動
afb-daemon --binding=/usr/lib/afb/low-can.so \
           --binding=/usr/lib/afb/vss.so \
           --port=1234 \
           --token=test-token
```

### 実車両での設定

```bash
# 1. 実CANデバイスの確認
ip link show can0

# 2. CANボーレート設定
sudo ip link set can0 type can bitrate 500000
sudo ip link set up can0

# 3. 車両固有のDBCファイルを配置
sudo cp toyota_camry_2024.dbc /etc/can/vehicle.dbc

# 4. low-can設定を実車に合わせる
cat > /etc/afb/can_config.json << EOF
{
  "dbc_file": "/etc/can/vehicle.dbc",
  "can_devices": [
    {
      "name": "hs",
      "device": "can0",
      "baudrate": 500000,
      "enabled": true
    }
  ]
}
EOF

# 5. AFBサービス再起動
sudo systemctl restart afb-daemon
```

---

## Yoctoレシピでの設定管理

AGLをYoctoでビルドする場合、設定ファイルをレシピに含めます。

### `meta-carspeedboy/recipes-config/agl-config/agl-config.bb`

```bitbake
SUMMARY = "AGL configuration for CarSpeedBoy"
LICENSE = "MIT"

SRC_URI = " \
    file://vss_config.json \
    file://can_config.json \
    file://vehicle.dbc \
"

do_install() {
    install -d ${D}${sysconfdir}/afb
    install -m 0644 ${WORKDIR}/vss_config.json ${D}${sysconfdir}/afb/
    install -m 0644 ${WORKDIR}/can_config.json ${D}${sysconfdir}/afb/
    
    install -d ${D}${sysconfdir}/can
    install -m 0644 ${WORKDIR}/vehicle.dbc ${D}${sysconfdir}/can/
}

FILES_${PN} = " \
    ${sysconfdir}/afb/vss_config.json \
    ${sysconfdir}/afb/can_config.json \
    ${sysconfdir}/can/vehicle.dbc \
"

RDEPENDS_${PN} = "agl-service-can-low-level agl-vss"
```

### 車種別の設定を管理

```
meta-carspeedboy/
├── recipes-config/
│   └── agl-config/
│       ├── agl-config.bb
│       └── files/
│           ├── common/
│           │   └── vss_config.json
│           ├── toyota/
│           │   ├── can_config.json
│           │   └── toyota.dbc
│           ├── honda/
│           │   ├── can_config.json
│           │   └── honda.dbc
│           └── nissan/
│               ├── can_config.json
│               └── nissan.dbc
```

**ビルド時に車種を選択:**
```bash
# Toyotaビルド
VEHICLE_TYPE=toyota bitbake agl-image-carspeedboy

# Hondaビルド
VEHICLE_TYPE=honda bitbake agl-image-carspeedboy
```

---

## 動的プロトコル切り替え（高度な使用例）

実行時にプロトコルを切り替えることも可能です。

### AFB APIで動的に設定変更

```cpp
// CarSpeedBoyアプリから設定を変更（通常は不要）
afb_client.call("vss", "configure", {
    {"path", "Vehicle.Speed"},
    {"source", "obd-ii"},
    {"pid", "0x0D"}
}, [](const json& response) {
    std::cout << "Protocol switched to OBD-II" << std::endl;
});
```

ただし、**通常はこの操作は不要**です。プラットフォーム側で事前に設定されているためです。

---

## デバッグ・確認方法

### 1. 現在の設定を確認

```bash
# AFB bindingの状態確認
afb-client ws://localhost:1234/api?token=xxx

# VSS設定の確認
vss get Vehicle.Speed

# low-can設定の確認
low-can list
```

### 2. CANトラフィックの監視

```bash
# CANメッセージをリアルタイム表示
candump can0

# 特定のCAN IDだけフィルタリング
candump can0,123:7FF

# 16進ダンプ
candump -L can0
```

### 3. VSS経由でのデータ確認

```bash
# afb-clientでテスト
afb-client-demo ws://localhost:1234/api?token=xxx

> vss subscribe '{"path": "Vehicle.Speed"}'
ON-REPLY vss/subscribe: {"response":"subscribed","jtype":"afb-reply","request":{"status":"success"}}

# 速度データが流れる
ON-EVENT vss/Vehicle.Speed: {"path":"Vehicle.Speed","value":65.5,"unit":"km/h","timestamp":1699964123456}
```

---

## まとめ

### プロトコル設定の責任分担

| 設定内容 | 設定場所 | 担当者 | CarSpeedBoy側の対応 |
|---------|---------|--------|-------------------|
| **CANバスデバイス** | `/etc/afb/can_config.json` | プラットフォームエンジニア | 不要 |
| **CAN ID/DBC** | `/etc/can/vehicle.dbc` | 車両統合エンジニア | 不要 |
| **VSS Mapping** | `/etc/afb/vss_config.json` | プラットフォームエンジニア | 不要 |
| **OBD-II設定** | `/etc/afb/obd_config.json` | プラットフォームエンジニア | 不要 |
| **速度閾値** | CarSpeedBoy `config.json` | アプリ開発者 | **必要** |

### CarSpeedBoyアプリケーションの関心事

✅ **関心あり:**
- `Vehicle.Speed`の値
- 速度閾値（20, 60, 100, 120 km/h）
- UIの更新

❌ **関心なし:**
- CANかOBD-IIか
- CAN ID
- ビットフィールド
- プロトコル詳細

### 利点

1. **アプリケーションの可搬性**: 車両が変わってもコード不変
2. **保守性**: プロトコル変更はプラットフォーム側のみ
3. **セキュリティ**: アプリは生のCAN/OBD-IIに直接アクセスしない
4. **開発効率**: アプリ開発者は車両プロトコルを学ばなくてOK
5. **テスト容易性**: VSS mockで簡単にテスト可能

**結論: プロトコル設定は完全にAGL Framework Services側の責任であり、CarSpeedBoyアプリケーションは`Vehicle.Speed`というVSS標準パスのみを使用します。**
