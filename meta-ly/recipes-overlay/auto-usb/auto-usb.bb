SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "Recipe created by bitbake-layers"
LICENSE = "CLOSED"
SRC_URI = "file://auto-usb.service \
           file://auto_usb.sh"

inherit systemd

S = "${WORKDIR}"

do_install() {
	install -d ${D}${systemd_unitdir}/system
	install -m 0644 ${S}/auto-usb.service ${D}${systemd_unitdir}/system/
	install -d ${D}${sysconfdir}
	install -m 0755 ${S}/auto_usb.sh ${D}${sysconfdir}/
}

FILES_${PN} += "${systemd_unitdir}/system/auto-usb.service"

SYSTEMD_SERVICE_${PN} = "auto-usb.service"
