require u-boot-common_${PV}.inc
require u-boot.inc

DEPENDS += "bc-native dtc-native"

UBOOT_URL = "git://github.com/zhangyunduan-ly/renesas-u-boot-cip.git"
BRANCH = "develop-ly"

SRC_URI = "${UBOOT_URL};branch=${BRANCH}"
SRCREV = "6692e85af9ad3a53190404e2f3a8f0756599e0c5"
PV = "v2021.10+git${SRCPV}"
