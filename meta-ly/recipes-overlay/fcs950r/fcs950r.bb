SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "Recipe created by bitbake-layers"
LICENSE = "CLOSED"

SRC_URI = "file://PHY_REG_PG.txt \
           file://TXPWR_LMT.txt \
           file://rtl8821c_config \
           file://rtl8821c_fw \
		   file://hci_uart.ko \
		   file://hci_uart.conf \
		   file://rtk_hciattach"

S = "${WORKDIR}"
KERNEL_VERSION = "5.10.201-cip41-yocto-standard"

do_install_append() {
	install -d ${D}/lib/firmware/
	install -m 0644 ${S}/PHY_REG_PG.txt ${D}/lib/firmware/
	install -m 0644 ${S}/TXPWR_LMT.txt ${D}/lib/firmware/

	install -d ${D}/lib/firmware/rtlbt/
	install -m 0644 ${S}/rtl8821c_config ${D}/lib/firmware/rtlbt/
	install -m 0644 ${S}/rtl8821c_fw ${D}/lib/firmware/rtlbt/

	install -d ${D}/lib/modules/${KERNEL_VERSION}/kernel/drivers/bluetooth/
	install -m 0644 ${S}/hci_uart.ko ${D}/lib/modules/${KERNEL_VERSION}/kernel/drivers/bluetooth/

	install -d ${D}/etc/modules-load.d/
	install -m 0644 ${S}/hci_uart.conf ${D}/etc/modules-load.d/

	install -d ${D}/usr/bin/
	install -m 0755 ${S}/rtk_hciattach ${D}/usr/bin/
}

FILES_${PN} += "/lib/firmware/PHY_REG_PG.txt"
FILES_${PN} += "/lib/firmware/TXPWR_LMT.txt"
FILES_${PN} += "/lib/firmware/rtlbt/rtl8821c_config"
FILES_${PN} += "/lib/firmware/rtlbt/rtl8821c_fw"
FILES_${PN} += "/lib/modules/${KERNEL_VERSION}/kernel/drivers/bluetooth/hci_uart.ko"
FILES_${PN} += "/etc/modules-load.d/hci_uart.conf"
FILES_${PN} += "/usr/bin/rtk_hciattach"

pkg_postinst:${PN}() {
    if [ -z "$D" ]; then
        depmod -a
    fi
}
