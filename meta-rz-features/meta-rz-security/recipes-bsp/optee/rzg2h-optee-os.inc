EXTRA_OEMAKE_append_rzg2h += " \
	${@oe.utils.conditional("TRUSTED_BOARD_BOOT", "1", "CFG_RZG_SEC_IP_DRV=y CFG_RZG_SEC_LIB_DIR=${SYMLINK_NATIVE_SEC_LIB_DIR}", "",d)} \
"

SRCREV_rzg2h = "c6e430d7b8140aaff9a59adc2f9a76ed3ef39d42"

PLATFORM_rzg2h = "rzg"
PLATFORM_FLAVOR_rzg2h = "${@d.getVar("MACHINE", False).replace("-", "_")}"

EXTRA_OEMAKE_rzg2h = " \
	PLATFORM=${PLATFORM} PLATFORM_FLAVOR=${PLATFORM_FLAVOR} \
	CFG_ARM64_core=y CFG_REE_FS=y CFG_RPMB_FS=n CFG_CRYPTO_WITH_CE=y \
	RZG_DRAM_ECC=${USE_ECC} RZG_ECC_FULL=${ECC_FULL} \
	LIBGCC_LOCATE_CFLAGS=--sysroot=${STAGING_DIR_HOST} \
	CROSS_COMPILE=${TARGET_PREFIX} \
"

do_deploy_append_rzg2h() {
	# Copy TEE OS to deploy folder
	install -m 0644 ${S}/out/arm-plat-${PLATFORM}/core/tee.elf ${DEPLOYDIR}/tee-${MACHINE}.elf
	install -m 0644 ${S}/out/arm-plat-${PLATFORM}/core/tee-raw.bin ${DEPLOYDIR}/tee-${MACHINE}.bin

	# SREC file is generated from RAW bin and it has start address at 0x0,
	# so we must adjust it to our address for our platform before using it.
	objcopy --adjust-vma=0x44100000 -I srec -O srec \
		${S}/out/arm-plat-${PLATFORM}/core/tee.srec ${DEPLOYDIR}/tee-${MACHINE}.srec
}
