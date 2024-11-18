SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "Recipe created by bitbake-layers"
LICENSE = "CLOSED"
SRC_URI = "file://ppp-off"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${bindir}
	install -m 0644 ${WORKDIR}/ppp-off ${D}${bindir}/ppp-off
}
