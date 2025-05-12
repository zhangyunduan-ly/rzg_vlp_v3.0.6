SUMMARY = "RZ/G2 OP-TEE TA FWU"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit deploy python3native

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${S}/LICENSE.md;md5=e39af0548166c775f6685c6b69ec94f8"

PV_rzg3s = "1.00.0+git${SRCPV}"

BRANCH_rzg3s = "rzg2l"

SRCREV_rzg3s = "4cda62de17a421b94d982899d7b957c3d97f6658"

SRC_URI_append_rzg3s = " \
	git://github.com/renesas-rz/rzg_optee-ta_fwu.git;branch=${BRANCH};protocol=https \
"

DEPENDS_append_rzg3s = " optee-os optee-client python3-pyelftools-native python3-cryptography-native python3-pycryptodome-native python3-pycryptodomex-native"

OPTEE_CLIENT_EXPORT = "${STAGING_DIR_HOST}${prefix}"
TA_DEV_KIT_DIR = "${STAGING_INCDIR}/optee/export-user_ta/"

CFLAGS_prepend_rzg3s = "--sysroot=${STAGING_DIR_HOST} "

EXTRA_OEMAKE_rzg3s = " \
	TA_DEV_KIT_DIR=${TA_DEV_KIT_DIR} \
	OPTEE_CLIENT_EXPORT=${OPTEE_CLIENT_EXPORT} \
	CROSS_COMPILE=${TARGET_PREFIX} \
"

S = "${WORKDIR}/git"

do_compile_rzg3s () {
	oe_runmake MACHINE="${@d.getVar("MACHINE", False).replace("-", "_")}"
}

do_install_rzg3s () {
	mkdir -p ${WORKDIR}/deploy/${bindir}

	install -p -m 0755 ${S}/out/ca/* ${WORKDIR}/deploy/${bindir}
}

do_deploy_rzg3s () {
	cd ${WORKDIR}/deploy
	install -d ${DEPLOYDIR}

	tar -zcvf ${DEPLOYDIR}/optee-ta-fwu-${MACHINE}.tar.gz ./*
	tar -jcvf ${DEPLOYDIR}/optee-ta-fwu-${MACHINE}.tar.bz2 ./*
}

addtask deploy before do_build after do_install
