inherit python3native

DEPENDS_rzg3s = " \
	${@oe.utils.conditional("TRUSTED_BOARD_BOOT", "1", " \
		python3-pycryptodome-native python3-pycryptodomex-native secprv-native bptool-native \
	", "",d)}"

BUILD_TBB_DIR_rzg3s = "${S}/build_tbb"

do_compile_append_rzg3s () {
	#create certification binary
	if [ "${TRUSTED_BOARD_BOOT}" = "1" ]; then
		mkdir -p ${BUILD_TBB_DIR}/tmp

		python3 ${MANIFEST_GENERATION_KCERT} \
			-info ${DIRPATH_MANIFEST_GENTOOL}/info/g3s/flash_${IMG_AUTH_MODE}_info.xml \
			-iskey ${SYMLINK_NATIVE_BOOT_KEY_DIR}/bl2_key.pem \
			-certout ${BUILD_TBB_DIR}/tmp/flash_writer-kcert.bin

		python3 ${MANIFEST_GENERATION_CCERT} \
			-info ${DIRPATH_MANIFEST_GENTOOL}/info/g3s/flash_${IMG_AUTH_MODE}_info.xml \
			-iskey ${SYMLINK_NATIVE_BOOT_KEY_DIR}/bl2_key.pem \
			-imgin ${S}/FlashWriter.bin \
			-certout ${BUILD_TBB_DIR}/tmp/flash_writer-ccert.bin \
			-imgout ${BUILD_TBB_DIR}/tmp/flash_writer-sign.bin

		objcopy -I binary --pad-to=0x00000400 ${BUILD_TBB_DIR}/tmp/flash_writer-kcert.bin \
			${BUILD_TBB_DIR}/tmp/flash_writer-kcert_pad.bin
		objcopy -I binary --pad-to=0x00000C00 ${BUILD_TBB_DIR}/tmp/flash_writer-ccert.bin \
			${BUILD_TBB_DIR}/tmp/flash_writer-ccert_pad.bin

		cat ${BUILD_TBB_DIR}/tmp/flash_writer-kcert_pad.bin \
			${BUILD_TBB_DIR}/tmp/flash_writer-ccert_pad.bin \
			${BUILD_TBB_DIR}/tmp/flash_writer-sign.bin > ${BUILD_TBB_DIR}/tmp/flash_writer-image.bin

		bptool ${BUILD_TBB_DIR}/tmp/flash_writer-image.bin ${BUILD_TBB_DIR}/tmp/flash_writer-bparm.bin 0x107000 scif

		objcopy -I binary -O elf64-little --add-section .parm=${BUILD_TBB_DIR}/tmp/flash_writer-bparm.bin \
			--set-section-flags .parm=alloc --adjust-section-vma .parm=0xA1E00 \
			--adjust-section-vma .data=0x107000 ${BUILD_TBB_DIR}/tmp/flash_writer-image.bin \
			${BUILD_TBB_DIR}/tmp/flash_writer_bp_tbb.elf
		objcopy -O srec --srec-forceS3 ${BUILD_TBB_DIR}/tmp/flash_writer_bp_tbb.elf \
			${BUILD_TBB_DIR}/tmp/flash_writer_bp_tbb.mot
	fi
}

do_deploy_append_rzg3s () {
	if [ "${TRUSTED_BOARD_BOOT}" = "1" ]; then
		install -m 0644  ${BUILD_TBB_DIR}/tmp/flash_writer_bp_tbb.elf ${DEPLOYDIR}/FlashWriter-${MACHINE}_TBB.elf
		install -m 0644  ${BUILD_TBB_DIR}/tmp/flash_writer_bp_tbb.mot ${DEPLOYDIR}/FlashWriter-${MACHINE}_TBB.mot
	fi
}
