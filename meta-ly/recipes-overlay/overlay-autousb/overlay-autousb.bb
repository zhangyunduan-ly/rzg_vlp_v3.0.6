SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "Recipe created by bitbake-layers"
LICENSE = "CLOSED"
SRC_URI = "file://auto-usb.service \
           file://auto_usb.sh"

inherit systemd

S = "${WORKDIR}"

do_install() {
	install -d ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/auto-usb.service ${D}${systemd_unitdir}/system/auto-usb.service
	install -d ${D}${sysconfdir}
	install -m 0755 ${WORKDIR}/auto_usb.sh ${D}${sysconfdir}/auto_usb.sh
}

FILES_${PN} += "${systemd_unitdir}/system/auto-usb.service"

SYSTEMD_SERVICE_${PN} = "auto-usb.service"
