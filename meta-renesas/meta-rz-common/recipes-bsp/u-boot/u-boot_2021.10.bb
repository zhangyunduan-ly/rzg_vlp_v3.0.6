require u-boot-common_${PV}.inc
require u-boot.inc

DEPENDS += "bc-native dtc-native"

UBOOT_URL = "git://github.com/zhangyunduan-ly/renesas-u-boot-cip-ly.git"
BRANCH = "dev-ly-ph"

SRC_URI = "${UBOOT_URL};branch=${BRANCH}"
SRCREV = "8d14b8c843d65cde6bace93d7b81c0bc9e6029c9"
PV = "v2021.10+git${SRCPV}"
