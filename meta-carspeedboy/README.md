# meta-carspeedboy

Yocto/OpenEmbedded layer for CarSpeedBoy IVI application.

## Description

This layer provides recipes for building and installing the CarSpeedBoy application on Automotive Grade Linux (AGL) systems.

## Dependencies

This layer depends on:

- **meta-openembedded** (meta-oe)
- **meta-qt5**
- **meta-agl** (for AGL-specific services)

## Quick Start

### 1. Add Layer to Build Configuration

```bash
cd /path/to/agl/build
bitbake-layers add-layer /path/to/meta-carspeedboy
```

### 2. Add to Image

Add the following to your `local.conf` or image recipe:

```bitbake
IMAGE_INSTALL:append = " carspeedboy"
```

### 3. Build

```bash
bitbake agl-demo-platform
```

Or build the package individually:

```bash
bitbake carspeedboy
```

## Layer Structure

```
meta-carspeedboy/
├── conf/
│   └── layer.conf              # Layer configuration
├── recipes-carspeedboy/
│   └── carspeedboy/
│       └── carspeedboy_git.bb  # Main recipe
└── README.md
```

## Recipe Details

### carspeedboy_git.bb

**Version:** Git-based (AUTOREV)  
**License:** MIT  
**Dependencies:**
- Qt5 (qtbase, qtdeclarative, qtwebsockets, qtwayland)
- nlohmann-json
- agl-service-vsomeip (runtime)

**Installed Files:**
- `/usr/bin/carspeedboy` - Main application binary
- `/etc/carspeedboy/config.json` - Configuration file
- `/usr/share/carspeedboy/qml/` - QML UI files

## Configuration

The application configuration is installed to `/etc/carspeedboy/config.json`.

Key configuration items:
- AFB WebSocket URL
- Speed thresholds for expression states
- Display settings (units, theme)
- Logging configuration

## Integration with AGL

### Required AGL Services

- **agl-service-vsomeip** - SOME/IP communication
- **VSS (Vehicle Signal Specification)** - Vehicle data access

### AFB Binding

The application connects to the AGL Application Framework Binder (AFB) via WebSocket to access VSS data.

Default endpoint: `ws://localhost:1234/api`

## Testing

### On AGL System

```bash
# Run the application
carspeedboy

# Check logs
journalctl -u carspeedboy -f

# Verify configuration
cat /etc/carspeedboy/config.json
```

### QEMU Testing

```bash
# Start AGL QEMU image
/path/to/agl/build/tmp/deploy/images/qemux86-64/run.sh

# Inside QEMU
carspeedboy
```

## Customization

### Using a Specific Git Revision

Edit `carspeedboy_git.bb`:

```bitbake
SRCREV = "abc123def456"  # Replace AUTOREV with specific commit
```

### Adding bbappend

Create `carspeedboy_git.bbappend` in your custom layer:

```bitbake
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://custom-config.json"

do_install:append() {
    install -m 0644 ${WORKDIR}/custom-config.json ${D}${sysconfdir}/carspeedboy/config.json
}
```

## Troubleshooting

### Build Errors

**Qt5 not found:**
```bash
bitbake-layers add-layer /path/to/meta-qt5
```

**nlohmann-json not found:**
```bash
bitbake-layers add-layer /path/to/meta-oe
```

### Runtime Issues

**AFB connection failed:**
- Check if `afb-daemon` is running: `systemctl status afb-daemon`
- Verify WebSocket URL in `/etc/carspeedboy/config.json`

**No speed data:**
- Ensure VSS service is running
- Check CAN interface configuration in `/etc/afb/`

## License

MIT License - See LICENSE file in the main repository.

## Maintainer

error0307 - https://github.com/error0307/CarSpeedBoy

## Version Compatibility

| Layer Version | AGL Version | Yocto Version |
|--------------|-------------|---------------|
| 1.0          | Peacock 17+ | Kirkstone+    |

## Contributing

For contributions, please submit pull requests to the main repository:
https://github.com/error0307/CarSpeedBoy
