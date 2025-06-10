FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = "file://10-eth0.network \
           		  file://20-eth1.network \
				  file://30-wlan0.network"

do_install_append() {
	install -d ${D}${sysconfdir}/systemd/network/
	install -m 0644 ${WORKDIR}/10-eth0.network ${D}${sysconfdir}/systemd/network/
	install -m 0644 ${WORKDIR}/20-eth1.network ${D}${sysconfdir}/systemd/network/
	install -m 0644 ${WORKDIR}/30-wlan0.network ${D}${sysconfdir}/systemd/network/
}
