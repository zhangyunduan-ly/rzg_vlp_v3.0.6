require u-boot-common_${PV}.inc
require u-boot.inc

DEPENDS += "bc-native dtc-native"

UBOOT_URL = "git://github.com/zhangyunduan-ly/renesas-u-boot-cip.git"
BRANCH = "develop-ly"

SRC_URI = "${UBOOT_URL};branch=${BRANCH}"
SRCREV = "611d37eee938fb9da4992db8c12808cb47a9c942"
PV = "v2021.10+git${SRCPV}"
