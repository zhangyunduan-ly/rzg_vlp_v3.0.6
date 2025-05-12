require u-boot-common_${PV}.inc
require u-boot.inc

DEPENDS += "bc-native dtc-native"

UBOOT_URL = "git://github.com/zhangyunduan-ly/renesas-u-boot-cip.git"
BRANCH = "dev-ly-ph"

SRC_URI = "${UBOOT_URL};branch=${BRANCH}"
SRCREV = "793ce07969a40ad6686d2ad83ac62307a8cdd0a7"
PV = "v2021.10+git${SRCPV}"
