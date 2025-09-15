SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "Recipe created by bitbake-layers"
LICENSE = "CLOSED"

PV = "0.0.2"

SRC_URI = "file://system-monitor.service \
           file://system-monitor.sh"

inherit systemd

S = "${WORKDIR}"

do_install() {
	install -d ${D}${systemd_unitdir}/system
	install -m 0644 ${S}/system-monitor.service ${D}${systemd_unitdir}/system/
	install -d ${D}${sysconfdir}
	install -m 0755 ${S}/system-monitor.sh ${D}${sysconfdir}/
}

FILES_${PN} += "${systemd_unitdir}/system/system-monitor.service"

SYSTEMD_SERVICE_${PN} = "system-monitor.service"
