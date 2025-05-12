ENC_UFPK_BIN_rzg2h = "${SYMLINK_NATIVE_PROV_KEY_DIR}/${FILE_PROVISIONING_KEY_ENC}"

EXTRA_OEMAKE_append_rzg2h += " \
	${@oe.utils.conditional("TRUSTED_BOARD_BOOT", "1", "ENC_UFPK_BIN=${ENC_UFPK_BIN} TSIP_LIB_PATH=${TSIP_LIB_PATH}", "",d)} \
"

DEPENDS_rzg2h += " \
	${@oe.utils.conditional("TRUSTED_BOARD_BOOT", "1", "secprv-native", "",d)} \
"
