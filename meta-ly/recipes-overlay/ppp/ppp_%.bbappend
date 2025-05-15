FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = "file://ppp-off"

do_install_append() {
	install -d ${D}${bindir}
	install -m 0644 ${WORKDIR}/ppp-off ${D}${bindir}/ppp-off
}
