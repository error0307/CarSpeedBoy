SUMMARY = "CarSpeedBoy - Speed-based Character Expression IVI Application"
DESCRIPTION = "An Automotive Grade Linux (AGL) IVI application that displays \
an animated character whose expression changes based on vehicle speed data \
retrieved via VSS (Vehicle Signal Specification)."
HOMEPAGE = "https://github.com/error0307/CarSpeedBoy"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=3e5e3f375f6e8d77f4e8e6c7e6dd6d1e"

# Dependencies
DEPENDS = " \
    qtbase \
    qtdeclarative \
    qtwebsockets \
    nlohmann-json \
"

# Runtime dependencies
RDEPENDS:${PN} = " \
    qtbase \
    qtdeclarative \
    qtwebsockets \
    qtwayland \
    agl-service-vsomeip \
"

# Source repository
SRC_URI = "git://github.com/error0307/CarSpeedBoy.git;protocol=https;branch=main"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

# Inherit CMake and Qt5 classes
inherit cmake_qt5

# CMake configuration
EXTRA_OECMAKE = " \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=${prefix} \
"

# Installation
do_install:append() {
    # Install configuration files
    install -d ${D}${sysconfdir}/carspeedboy
    install -m 0644 ${S}/config/config.json ${D}${sysconfdir}/carspeedboy/
    
    # Install QML files
    install -d ${D}${datadir}/carspeedboy/qml
    install -m 0644 ${S}/qml/*.qml ${D}${datadir}/carspeedboy/qml/
    
    # Install resources (when available)
    # install -d ${D}${datadir}/carspeedboy/resources
    # install -m 0644 ${S}/resources/animations/*.gif ${D}${datadir}/carspeedboy/resources/
}

# Package files
FILES:${PN} += " \
    ${bindir}/carspeedboy \
    ${sysconfdir}/carspeedboy/* \
    ${datadir}/carspeedboy/* \
"

# Package configuration
INSANE_SKIP:${PN} += "dev-so"
