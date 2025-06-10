FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = "file://dnsmasq.conf"

do_install_append() {
	install -d ${D}${sysconfdir}/
	install -m 0644 ${WORKDIR}/dnsmasq.conf ${D}${sysconfdir}/
}
