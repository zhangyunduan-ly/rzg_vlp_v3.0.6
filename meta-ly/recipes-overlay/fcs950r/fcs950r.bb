SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "Recipe created by bitbake-layers"
LICENSE = "CLOSED"
SRC_URI = "file://PHY_REG_PG.txt \
           file://TXPWR_LMT.txt \
           file://rtl8821c_config \
           file://rtl8821c_fw"

S = "${WORKDIR}"

do_install_append() {
	install -d ${D}/lib/firmware
	install -m 0644 ${WORKDIR}/PHY_REG_PG.txt ${D}/lib/firmware/PHY_REG_PG.txt
	install -m 0644 ${WORKDIR}/TXPWR_LMT.txt ${D}/lib/firmware/TXPWR_LMT.txt
	install -d ${D}/lib/firmware/rtlbt
	install -m 0644 ${WORKDIR}/rtl8821c_config ${D}/lib/firmware/rtlbt/rtl8821c_config
	install -m 0644 ${WORKDIR}/rtl8821c_fw ${D}/lib/firmware/rtlbt/rtl8821c_fw
}

FILES_${PN} += "/lib/firmware/PHY_REG_PG.txt"
FILES_${PN} += "/lib/firmware/TXPWR_LMT.txt"
FILES_${PN} += "/lib/firmware/rtlbt/rtl8821c_config"
FILES_${PN} += "/lib/firmware/rtlbt/rtl8821c_fw"
