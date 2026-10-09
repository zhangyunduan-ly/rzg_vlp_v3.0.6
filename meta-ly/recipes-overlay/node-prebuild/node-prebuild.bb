SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "Recipe created by bitbake-layers"
LICENSE = "CLOSED"

PV = "16.20.2"

SRC_URI = "file://node"

S = "${WORKDIR}"

do_install() {
	install -d ${D}/usr/bin/
	install -m 0755 ${S}/node ${D}/usr/bin/
}

FILES_${PN} += "/usr/bin/node"
