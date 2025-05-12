ATFW_OPT_append_rzg2h += " \
	${@oe.utils.conditional("TRUSTED_BOARD_BOOT", "1", "RZG2_SECURE_BOOT=1 TSIP_LIB_PATH=${TSIP_LIB_PATH}", "",d)} \
	${@oe.utils.conditional("ENABLE_SPD_OPTEE", "1", " SPD=opteed", "",d)} \
"

DEPENDS_append_rzg2h += " \
	${@oe.utils.conditional("TRUSTED_BOARD_BOOT", "1", " secprv-native u-boot", "",d)} \
	${@oe.utils.conditional("ENABLE_SPD_OPTEE", "1", " optee-os", "",d)} \
"

do_compile_append_rzg2h () {
	if [ "1" = "${TRUSTED_BOARD_BOOT}" ] ; then
		oe_runmake sectools PLAT=${PLATFORM} ${ATFW_OPT}
	fi
}


do_deploy_append_rzg2h () {
	if [ "1" = "${TRUSTED_BOARD_BOOT}" ] ; then
		temp_encrypt ${DEPLOYDIR}/bl31-${MACHINE}.bin ${DEPLOYDIR}/bl31-${MACHINE}_Enc.bin
		objcopy -I binary -O srec --adjust-vma=0x44000000 ${DEPLOYDIR}/bl31-${MACHINE}_Enc.bin \
			${DEPLOYDIR}/bl31-${MACHINE}_Enc.srec
	
		temp_encrypt ${DEPLOY_DIR_IMAGE}/u-boot-${MACHINE}.bin ${DEPLOYDIR}/u-boot-${MACHINE}_Enc.bin
		objcopy -I binary -O srec --adjust-vma=0x50000000 ${DEPLOYDIR}/u-boot-${MACHINE}_Enc.bin \
			${DEPLOYDIR}/u-boot-${MACHINE}_Enc.srec
	
		if [ "1" = "${ENABLE_SPD_OPTEE}" ]; then
			temp_encrypt ${DEPLOY_DIR_IMAGE}/tee-${MACHINE}.bin ${DEPLOYDIR}/tee-${MACHINE}_Enc.bin
			objcopy -I binary -O srec --adjust-vma=0x44100000 ${DEPLOYDIR}/tee-${MACHINE}_Enc.bin \
				${DEPLOYDIR}/tee-${MACHINE}_Enc.srec
		fi
	
		cp ${SYMLINK_NATIVE_BOOT_KEY_DIR}/${FILE_BOOT_KEYRING_ENC} ${DEPLOYDIR}/Keyring-${MACHINE}_Enc.bin
		objcopy -I binary -O srec --adjust-vma=0x440FE000 ${DEPLOYDIR}/Keyring-${MACHINE}_Enc.bin \
			${DEPLOYDIR}/Keyring-${MACHINE}_Enc.srec
	fi

	if [ "1" = "${TRUSTED_BOARD_BOOT}" ] ; then
		mkdir -p ${WORKDIR}/fwpkg
		cd ${WORKDIR}
		cp ${DEPLOYDIR}/bl31-${MACHINE}_Enc.bin fwpkg

		if [ "1" = "${ENABLE_SPD_OPTEE}" ]; then
			cp ${DEPLOYDIR}/tee-${MACHINE}_Enc.bin fwpkg
		fi

		cp ${DEPLOYDIR}/u-boot-${MACHINE}_Enc.bin fwpkg
		cp ${DEPLOYDIR}/Keyring-${MACHINE}_Enc.bin fwpkg

		tar zcvf ${DEPLOYDIR}/fwpkg-${MACHINE}.tar.gz fwpkg
	fi
}

temp_encrypt () {
	FILE_PATH_IN=${1}
	FILE_PATH_OUT=${2}
	FILE_PATH_SIG="./tools/renesas/rzg_security_tools/sign_fw/sign.bin"

	STR_EKEY_DATA=$(cat ${SYMLINK_NATIVE_BOOT_KEY_DIR}/${FILE_BOOTPRG_VERIFY_ENC_KEY} | hexdump -v -e '/1 "%02X"')
	STR_ENC_KEY="$(echo "${STR_EKEY_DATA}" | cut -c 1-32)"
	STR_ENC_IV0="$(echo "${STR_EKEY_DATA}" | cut -c 33-64)"
	FILE_PATH_SIG_KEY="${SYMLINK_NATIVE_BOOT_KEY_DIR}/${FILE_BOOTPRG_VERIFY_SIG_KEY}"

	./tools/renesas/rzg_security_tools/sign_fw/sign_fw --key-alg rsa --hash-alg sha256 --align 16 \
		--key ${FILE_PATH_SIG_KEY} --in ${FILE_PATH_IN} --out ${FILE_PATH_SIG}

	./tools/renesas/rzg_security_tools/encrypt_fw/encrypt_fw --key-alg cbc --nonce ${STR_ENC_IV0} \
		--key ${STR_ENC_KEY} --in ${FILE_PATH_SIG} --out ${FILE_PATH_OUT}
}
