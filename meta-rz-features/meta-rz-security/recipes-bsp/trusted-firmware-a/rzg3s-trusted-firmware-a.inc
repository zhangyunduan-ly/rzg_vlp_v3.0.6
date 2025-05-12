DEPENDS_append_rzg3s = " \
		${@oe.utils.conditional("TRUSTED_BOARD_BOOT", "1", " python3-pycryptodome-native \
		python3-pycryptodomex-native secprv-native", "", d)} \
"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SEC_FLAGS_append_rzg3s = " \
	${@oe.utils.conditional("TRUSTED_BOARD_BOOT", "1", " TRUSTED_BOARD_BOOT=1 COT=tbbr", "", d)} \
	${@oe.utils.conditional("ENABLE_SPD_OPTEE", "1", " SPD=opteed", "",d)} \
"

SRC_URI_append_rzg3s = " \
	file://0001-g3s-sram-size-fixed.patch \	
	file://0002-g3s-fip-size-fixed.patch \
"

EXTRA_FLAGS_append += "${SEC_FLAGS}"

do_compile_rzg3s () {
	oe_runmake PLAT=${PLATFORM} ${EXTRA_FLAGS} bl2 bl31

	if [ "${TRUSTED_BOARD_BOOT}" = "1" ]; then
		python3 ${MANIFEST_GENERATION_KCERT} -info ${DIRPATH_MANIFEST_GENTOOL}/info/g3s/bl2_${IMG_AUTH_MODE}_info.xml \
			-iskey ${SYMLINK_NATIVE_BOOT_KEY_DIR}/bl2_key.pem -certout ${S}/build/${PLATFORM}/release/bl2-kcert.bin

		python3 ${MANIFEST_GENERATION_CCERT} -info ${DIRPATH_MANIFEST_GENTOOL}/info/g3s/bl2_${IMG_AUTH_MODE}_info.xml \
			-iskey ${SYMLINK_NATIVE_BOOT_KEY_DIR}/bl2_key.pem -imgin ${S}/build/${PLATFORM}/release/bl2.bin \
			-certout ${S}/build/${PLATFORM}/release/bl2-ccert.bin -imgout ${S}/build/${PLATFORM}/release/bl2_tbb.bin

		python3 ${MANIFEST_GENERATION_KCERT} -info ${DIRPATH_MANIFEST_GENTOOL}/info/g3s/bl31_${IMG_AUTH_MODE}_info.xml \
			-iskey ${SYMLINK_NATIVE_BOOT_KEY_DIR}/bl31_key.pem -certout ${S}/build/${PLATFORM}/release/bl31-kcert.bin

		python3 ${MANIFEST_GENERATION_CCERT} -info ${DIRPATH_MANIFEST_GENTOOL}/info/g3s/bl31_${IMG_AUTH_MODE}_info.xml \
			-iskey ${SYMLINK_NATIVE_BOOT_KEY_DIR}/bl31_key.pem -imgin ${S}/build/${PLATFORM}/release/bl31.bin \
			-certout ${S}/build/${PLATFORM}/release/bl31-ccert.bin -imgout ${S}/build/${PLATFORM}/release/bl31_tbb.bin
	fi
}

do_install_append_rzg3s () {
	if [ "${TRUSTED_BOARD_BOOT}" = "1" ]; then
		# install firmware images
		install -m 0644 ${S}/build/${PLATFORM}/release/bl2-kcert.bin  ${D}/boot/bl2-kcert-${MACHINE}.bin
		install -m 0644 ${S}/build/${PLATFORM}/release/bl2-ccert.bin  ${D}/boot/bl2-ccert-${MACHINE}.bin
		install -m 0644 ${S}/build/${PLATFORM}/release/bl2_tbb.bin    ${D}/boot/bl2-${MACHINE}_tbb.bin
		install -m 0644 ${S}/build/${PLATFORM}/release/bl31-kcert.bin ${D}/boot/bl31-kcert-${MACHINE}.bin
		install -m 0644 ${S}/build/${PLATFORM}/release/bl31-ccert.bin ${D}/boot/bl31-ccert-${MACHINE}.bin
		install -m 0644 ${S}/build/${PLATFORM}/release/bl31_tbb.bin   ${D}/boot/bl31-${MACHINE}_tbb.bin

		install -m 0644 ${S}/build/${PLATFORM}/release/root_of_trust_key_pk.hash ${D}/boot/root_of_trust_key_pk.hash
	fi
}
