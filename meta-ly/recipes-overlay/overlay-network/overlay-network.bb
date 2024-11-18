SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "Recipe created by bitbake-layers"
LICENSE = "CLOSED"
SRC_URI = "file://10-eth0.network \
           file://20-eth1.network"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${sysconfdir}/systemd/network/
	install -m 0644 ${WORKDIR}/10-eth0.network ${D}${sysconfdir}/systemd/network/10-eth0.network
	install -m 0644 ${WORKDIR}/20-eth1.network ${D}${sysconfdir}/systemd/network/20-eth1.network
}
