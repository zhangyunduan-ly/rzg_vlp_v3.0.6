SUMMARY = "RZ OP-TEE TA Sample"

PACKAGE_ARCH = "${MACHINE_ARCH}"

LICENSE = "BSD-3-Clause & GPLv2"
LIC_FILES_CHKSUM = "file://${S}/LICENSE.md;md5=e39af0548166c775f6685c6b69ec94f8"

SRC_URI_rzg2l = " \
	file://optee-ta-sample_rzg2l.tar.gz \
"

inherit deploy python3native

DEPENDS_append = " optee-os optee-client python3-pyelftools-native python3-cryptography-native python3-pycryptodome-native python3-pycryptodomex-native"

OPTEE_CLIENT_EXPORT = "${STAGING_DIR_HOST}${prefix}"
TA_DEV_KIT_DIR = "${STAGING_INCDIR}/optee/export-user_ta/"

CFLAGS_prepend = "--sysroot=${STAGING_DIR_HOST} "

EXTRA_OEMAKE = " \
	TA_DEV_KIT_DIR=${TA_DEV_KIT_DIR} \
	OPTEE_CLIENT_EXPORT=${OPTEE_CLIENT_EXPORT} \
	CROSS_COMPILE=${TARGET_PREFIX} \
"

S = "${WORKDIR}/optee-ta-sample"

WRAPPED_KEY_UPDATE_KEY_rzg2l = "0180695A6A60A8B60502ED9321492301..."
KEY_UPDATE_KEY_INI_VEC_rzg2l = "998db909e180ff40fcd7fc1f904ab92b"

EXTRA_OEMAKE_append_rzg2l = " WRAPPED_KEY_UPDATE_KEY=${WRAPPED_KEY_UPDATE_KEY} KEY_UPDATE_KEY_INI_VEC=${KEY_UPDATE_KEY_INI_VEC}"

do_install_append_rzg2l () {
	mkdir -p ${WORKDIR}/deploy/${bindir}

	install -p -m 0755 ${S}/out/ca/* ${WORKDIR}/deploy/${bindir}
}

do_deploy_append_rzg2l () {
	cd ${WORKDIR}/deploy
	install -d ${DEPLOYDIR}

	tar -zcvf ${DEPLOYDIR}/optee-ta-sample-${MACHINE}.tar.gz ./*
	tar -jcvf ${DEPLOYDIR}/optee-ta-sample-${MACHINE}.tar.bz2 ./*
}

addtask deploy before do_build after do_install
