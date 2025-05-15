FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = "file://10-eth0.network \
           		  file://20-eth1.network"

do_install_append() {
	install -d ${D}${sysconfdir}/systemd/network/
	install -m 0644 ${WORKDIR}/10-eth0.network ${D}${sysconfdir}/systemd/network/10-eth0.network
	install -m 0644 ${WORKDIR}/20-eth1.network ${D}${sysconfdir}/systemd/network/20-eth1.network
}
