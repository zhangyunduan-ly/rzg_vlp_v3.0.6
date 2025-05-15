SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "Recipe created by bitbake-layers"
LICENSE = "CLOSED"
SRC_URI = "file://mfg-patches.service"

inherit systemd

S = "${WORKDIR}"

do_install() {
	install -d ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/mfg-patches.service ${D}${systemd_unitdir}/system/mfg-patches.service
}

FILES_${PN} += "${systemd_unitdir}/system/mfg-patches.service"

SYSTEMD_SERVICE_${PN} = "mfg-patches.service"
