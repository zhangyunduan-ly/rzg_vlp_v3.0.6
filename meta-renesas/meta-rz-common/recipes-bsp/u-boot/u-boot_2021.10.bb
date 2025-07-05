require u-boot-common_${PV}.inc
require u-boot.inc

DEPENDS += "bc-native dtc-native"

UBOOT_URL = "git://github.com/zhangyunduan-ly/renesas-u-boot-cip-ly.git"
BRANCH = "dev-ly-ph"

SRC_URI = "${UBOOT_URL};branch=${BRANCH}"
SRCREV = "269796d90c2b11e55b1b0c9780c8b79877ebf58f"
PV = "v2021.10+git${SRCPV}"
