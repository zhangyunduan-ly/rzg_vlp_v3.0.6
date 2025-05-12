# Support secure environment
IMAGE_INSTALL_append = " \
	${@oe.utils.conditional("ENABLE_SPD_OPTEE", "1", " optee-client", "",d)} \
"

BASIC_32BIT_PKGS_append_rzg2h = " \
	${@oe.utils.conditional("ENABLE_SPD_OPTEE", "1", " lib32-optee-client", "",d)} \
"
