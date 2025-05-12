RZ_SCE = "${@oe.utils.conditional("ENABLE_RZ_SCE", "1", "y", "n",d)}"

SRCREV_rzg2l = "c6e430d7b8140aaff9a59adc2f9a76ed3ef39d42"

PLATFORM_rzg2l = "rz"
PLATFORM_FLAVOR_smarc-rzg2l = "g2l_smarc_2"
PLATFORM_FLAVOR_ly-rzg2l = "g2l_smarc_2"
PLATFORM_FLAVOR_rzg2l-dev = "g2l_dev15_4"
PLATFORM_FLAVOR_smarc-rzg2lc = "g2lc_smarc_1"
PLATFORM_FLAVOR_rzg2lc-dev = "g2l_dev13_1"
PLATFORM_FLAVOR_smarc-rzg2ul = "g2ul_smarc"
PLATFORM_FLAVOR_smarc-rzv2l = "g2l_smarc_4"
PLATFORM_FLAVOR_rzv2l-dev = "g2l_dev15_4"

RZ_SCE ?= "n"

EXTRA_OEMAKE_rzg2l = " \
	PLATFORM=${PLATFORM} PLATFORM_FLAVOR=${PLATFORM_FLAVOR} \
	CFG_ARM64_core=y CFG_REE_FS=y CFG_RPMB_FS=n CFG_CRYPTO_WITH_CE=n \
	CFG_RZ_SCE=${RZ_SCE} CFG_RZ_SCE_LIB_DIR=${SYMLINK_NATIVE_SEC_LIB_DIR} \
	CROSS_COMPILE64=${TARGET_PREFIX} \
"

do_compile_append_rzg2l () {
	if [ "${TRUSTED_BOARD_BOOT}" = "1" ]; then
		python3 ${MANIFEST_GENERATION_KCERT} -info ${DIRPATH_MANIFEST_GENTOOL}/info/g2l/bl32_${IMG_AUTH_MODE}_info.xml \
			-iskey ${SYMLINK_NATIVE_BOOT_KEY_DIR}/bl32_key.pem -certout ${S}/out/arm-plat-${PLATFORM}/core/bl32-kcert.bin
		
		python3 ${MANIFEST_GENERATION_CCERT} -info ${DIRPATH_MANIFEST_GENTOOL}/info/g2l/bl32_${IMG_AUTH_MODE}_info.xml \
			-iskey ${SYMLINK_NATIVE_BOOT_KEY_DIR}/bl32_key.pem -imgin ${S}/out/arm-plat-${PLATFORM}/core/tee-raw.bin \
			-certout ${S}/out/arm-plat-${PLATFORM}/core/bl32-ccert.bin -imgout ${S}/out/arm-plat-${PLATFORM}/core/tee_tbb.bin
	fi
}

do_install_append_rzg2l () {
	# install firmware images
	install -d ${D}/boot

	# Copy TEE OS to install folder
	install -m 0644 ${S}/out/arm-plat-${PLATFORM}/core/tee.elf     ${D}/boot/tee-${MACHINE}.elf
	install -m 0644 ${S}/out/arm-plat-${PLATFORM}/core/tee-raw.bin ${D}/boot/tee-${MACHINE}.bin

	if [ "${TRUSTED_BOARD_BOOT}" = "1" ]; then
		install -m 0644 ${S}/out/arm-plat-${PLATFORM}/core/bl32-kcert.bin ${D}/boot/bl32-kcert-${MACHINE}.bin
		install -m 0644 ${S}/out/arm-plat-${PLATFORM}/core/bl32-ccert.bin ${D}/boot/bl32-ccert-${MACHINE}.bin
		install -m 0644 ${S}/out/arm-plat-${PLATFORM}/core/tee_tbb.bin    ${D}/boot/tee-${MACHINE}_tbb.bin
	fi
}

FILES_${PN} = "/boot "
SYSROOT_DIRS += "/boot"


