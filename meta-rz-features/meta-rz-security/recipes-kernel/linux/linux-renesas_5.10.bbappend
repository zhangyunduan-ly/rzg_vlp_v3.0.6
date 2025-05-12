FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_append_rzg2h = " \
	${@oe.utils.conditional("ENABLE_SPD_OPTEE", "1", "file://0001-arm64-dts-renesas-r8a774-a1-a3-b1-c0-e1-enable-OP-TEE.patch", "",d)} \
"

SRC_URI_append_rzg2l = " \
	${@oe.utils.conditional("ENABLE_SPD_OPTEE", "1", "file://0002-arm64-dts-renesas-r9a07g0-43-44-54-enable-OP-TEE.patch", "",d)} \
"

SRC_URI_append_rzg3s = " \
	${@oe.utils.conditional("ENABLE_SPD_OPTEE", "1", "file://0003-arm64-dts-renesas-r9a08g045-enable-OP-TEE.patch", "",d)} \
"
