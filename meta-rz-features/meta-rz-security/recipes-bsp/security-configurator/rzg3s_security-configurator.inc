DESCRIPTION = "RZ Security Configurator"
LICENSE = "CLOSED"

inherit deploy python3native

PV_rzg3s = "V1.20"

SRC_URI_rzg3s = " file://g3s/security_configurator.tar.gz"

DEPENDS_rzg3s = "\
	${@oe.utils.conditional("TRUSTED_BOARD_BOOT", "1", " \
		python3-pycryptodome-native python3-pycryptodomex-native secprv-native bptool-native ", "", d)} \
	"

COMPATIBLE_MACHINE_rzg3s = "(rzg3s-dev|smarc-rzg3s)"

BOARD_rzg3s-dev        = "RZG3S_DEV"
BOARD_smarc-rzg3s 	   = "RZG3S_SMARC"

EXTRA_OEMAKE_rzg3s = " BOARD=${BOARD} CROSS_COMPILE=${TARGET_PREFIX}"

S = "${WORKDIR}/security_configurator"

BUILD_DIR = "${S}/build/${BOARD}"

BUILD_TBB_DIR = "${S}/build_tbb/${BOARD}"


do_install[noexec] = "1"

do_compile_rzg3s () {
	oe_runmake OUTDIR=${BUILD_DIR}

	if [ "${TRUSTED_BOARD_BOOT}" = "1" ]; then
		oe_runmake OUTDIR=${BUILD_TBB_DIR} TRUSTED_BOARD_BOOT=1

		mkdir -p ${BUILD_TBB_DIR}/tmp

		IMAGE_FILE=$(find "${BUILD_TBB_DIR}" -name "*Security_Configurator*.bin" -maxdepth 1 -printf "%f\n" )

		python3 ${MANIFEST_GENERATION_KCERT} -info ${DIRPATH_MANIFEST_GENTOOL}/info/g3s/bl2_${IMG_AUTH_MODE}_info.xml \
			-iskey ${SYMLINK_NATIVE_BOOT_KEY_DIR}/bl2_key.pem -certout ${BUILD_TBB_DIR}/tmp/sec-cfg-kcert.bin

		python3 ${MANIFEST_GENERATION_CCERT} -info ${DIRPATH_MANIFEST_GENTOOL}/info/g3s/bl2_${IMG_AUTH_MODE}_info.xml \
			-iskey ${SYMLINK_NATIVE_BOOT_KEY_DIR}/bl2_key.pem -imgin ${BUILD_TBB_DIR}/${IMAGE_FILE} \
			-certout ${BUILD_TBB_DIR}/tmp/sec-cfg-ccert.bin -imgout ${BUILD_TBB_DIR}/tmp/sec-cfg-image.bin

		objcopy -I binary --adjust-vma=0x000A2000 --pad-to=0xA2400 ${BUILD_TBB_DIR}/tmp/sec-cfg-kcert.bin
		objcopy -I binary --adjust-vma=0x000A2400 --pad-to=0xA3000 ${BUILD_TBB_DIR}/tmp/sec-cfg-ccert.bin

		cat ${BUILD_TBB_DIR}/tmp/sec-cfg-kcert.bin ${BUILD_TBB_DIR}/tmp/sec-cfg-ccert.bin \
			${BUILD_TBB_DIR}/tmp/sec-cfg-image.bin > ${BUILD_TBB_DIR}/tmp/${IMAGE_FILE}

		bptool ${BUILD_TBB_DIR}/tmp/${IMAGE_FILE} ${BUILD_TBB_DIR}/tmp/${IMAGE_FILE%.*}_TBB.bin 0xA2000
		cat ${BUILD_TBB_DIR}/tmp/${IMAGE_FILE} >> ${BUILD_TBB_DIR}/tmp/${IMAGE_FILE%.*}_TBB.bin

		objcopy -I binary -O srec --adjust-vma=0x000A1E00 --srec-forceS3 ${BUILD_TBB_DIR}/tmp/${IMAGE_FILE%.*}_TBB.bin \
			${BUILD_TBB_DIR}/${IMAGE_FILE%.*}_TBB.srec
	fi
}

do_deploy_rzg3s () {
	install -d ${DEPLOYDIR}
	install -m 644 ${BUILD_DIR}/*.mot ${DEPLOYDIR}

	if [ "${TRUSTED_BOARD_BOOT}" = "1" ]; then
		install -m 644 ${BUILD_TBB_DIR}/*_TBB.srec ${DEPLOYDIR}
	fi
}

addtask deploy after do_compile
