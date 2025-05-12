inherit deploy python3native

DEPENDS = " \
	python3-pyelftools-native python3-cryptography-native python3-pycryptodome-native secprv-native \
"

export CROSS_COMPILE64="${TARGET_PREFIX}"

# Let the Makefile handle setting up the flags as it is a standalone application
LD[unexport] = "1"
LDFLAGS[unexport] = "1"
export CCcore="${CC}"
export LDcore="${LD}"
libdir[unexport] = "1"

S = "${WORKDIR}/git"

CFLAGS_prepend = "--sysroot=${STAGING_DIR_HOST} "

do_compile() {
	oe_runmake
}

do_install() {
	#install TA devkit
	install -d ${D}/usr/include/optee/export-user_ta/

	for f in  ${B}/out/arm-plat-${PLATFORM}/export-ta_arm64/*; do
		cp -aR $f ${D}/usr/include/optee/export-user_ta/
	done
}

do_deploy() {
	# Create deploy folder
	install -d ${DEPLOYDIR}
}
addtask deploy before do_build after do_compile

FILES_${PN}-dev = "/usr/include/optee"

INSANE_SKIP_${PN}-dev = "staticdev"
