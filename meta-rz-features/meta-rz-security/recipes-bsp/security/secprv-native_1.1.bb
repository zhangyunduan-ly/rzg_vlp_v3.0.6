SUMMARY = "RZ Security Provisioning Tool"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

require include/rz-common-security-config.inc
require include/rzg2l-security-config.inc
require include/rzg2h-security-config.inc
inherit native

def check_platform(d):
    soc = d.getVar("SOC_FAMILY")
    if soc == 'rzg2l:r9a07g044l' or soc == 'rzg2l:r9a07g044c' or soc == 'rzg2ul:r9a07g043u':
        return 'RZG2L'
    if soc == 'rzv2l:r9a07g054l':
        return 'RZG2L'
    if soc == 'r9a08g045':
        return 'RZG3S'
    if soc == 'rzg2:r8a774a1' or soc == 'rzg2:r8a774b1' or soc == 'rzg2:r8a774c0' or soc == 'rzg2:r8a774e1':
        return 'RZG2H'
    bb.fatal("\nPlatform %s did not support" %soc);

PLATFORM="${@check_platform(d)}"

def check_user_factory_prog(d):
    platform = d.getVar("PLATFORM")
    if platform == 'RZG2L':
        return 'user_factory_prog'
    if platform == 'RZG3S':
        return 'user_factory_prog'
    if platform == 'RZG2H':
        return 'Provisioning'


DIRPATH_SEC_STORAGE  = "${HOME}/rz_secprv/${MACHINE}"
DIRPATH_GEN_KEY_ROOT = "${DIRPATH_SEC_STORAGE}/Key"
DIRPATH_SEC_LIB_ROOT = "${DIRPATH_SEC_STORAGE}/Lib"
DIR_USER_FACTORY_PROG_KEY = "${@check_user_factory_prog(d)}"

S = "${WORKDIR}"

DEPENDS += " openssl-native util-linux-native bc-native "

SRC_URI = " \
    ${@oe.utils.conditional("PLATFORM", "RZG2H", "file://g2h/key_management_tool.tar.gz", "",d)} \
    ${@oe.utils.conditional("PLATFORM", "RZG2L", "file://g2l/key_management_tool.tar.gz", "",d)} \
    ${@oe.utils.conditional("PLATFORM", "RZG2L", "file://manifest_generation_tool.tar.gz", "",d)} \
    ${@oe.utils.conditional("PLATFORM", "RZG3S", "file://g3s/key_management_tool.tar.gz", "",d)} \
    ${@oe.utils.conditional("PLATFORM", "RZG3S", "file://manifest_generation_tool.tar.gz", "",d)} \
"

addtask newkey after do_configure
do_newkey[dirs] = "${S}"
do_newkey[nostamp] = "1"
do_newkey () {
    cd ./key_management_tool
    sh ./sec_keygen.sh -t ${DIRPATH_GEN_KEY_ROOT}
}

addtask update after do_configure
do_update[dirs] = "${S}"
do_update[nostamp] = "1"
do_update () {
        cd ./key_management_tool
    if [ "${PLATFORM}" = "RZG2L" ] || [ "${PLATFORM}" = "RZG3S" ]; then
        sh ./sec_keygen.sh -t ${DIRPATH_GEN_KEY_ROOT} -d ${DIR_USER_KEY_VERSION} -u
    else
        sh ./sec_keygen.sh -t ${DIRPATH_GEN_KEY_ROOT} -d ${DIR_V_MAJOR} -u
    fi
}

# do_compile() nothing
do_compile[noexec] = "1"

do_install () {
    if [ -d "${DIRPATH_GEN_KEY_ROOT}" ]; then
        install -d "${D}/${DIRPATH_SEC_DATADIR_NATIVE}"

        boot_key=$(find "${DIRPATH_GEN_KEY_ROOT}" -name "${DIR_USER_KEY_VERSION}")
        if [ -z ${boot_key} ]; then
            echo "Could not find the directory: ${DIRPATH_GEN_KEY_ROOT}/${DIR_USER_KEY_VERSION}"
            exit 1
        fi
        ln -nfs "${boot_key}" "${D}/${SYMLINK_NATIVE_BOOT_KEY_DIR}"

        prov_key=$(find "${DIRPATH_GEN_KEY_ROOT}" -name "${DIR_USER_FACTORY_PROG_KEY}")
        if [ -z ${prov_key} ]; then
            echo "Could not find the directory: ${DIRPATH_GEN_KEY_ROOT}/${DIR_USER_FACTORY_PROG_KEY}"
            exit 1
        fi
        ln -nfs "${prov_key}" "${D}/${SYMLINK_NATIVE_PROV_KEY_DIR}"
    fi

    if [ "${PLATFORM}" = "RZG2L" ] || [ "${PLATFORM}" = "RZG3S" ]; then
        if [ "${TRUSTED_BOARD_BOOT}" = "1" ]; then
            install -d "${D}${bindir}"
            cp -rf --no-preserve=ownership "${S}"/manifest_generation_tool "${D}${bindir}"
        fi
    fi

    if [ -d "${DIRPATH_SEC_LIB_ROOT}" ]; then
        install -d "${D}/${DIRPATH_SEC_LIBDIR_NATIVE}";
        ln -nfs "${DIRPATH_SEC_LIB_ROOT}" "${D}/${SYMLINK_NATIVE_SEC_LIB_DIR}"
    fi
}
