# For RZ/G2L Series Security Features purpose

# Trusted Board Boot Verification Mode: "encsign" or "sign"
IMG_AUTH_MODE           = "encsign"

DIRPATH_MANIFEST_GENTOOL    = "${STAGING_BINDIR_NATIVE}/manifest_generation_tool"
KCERT_COMMON_OPTION         = "-halgo SHA2-256 -salgo ECDSA-P256 -mskey ${SYMLINK_NATIVE_BOOT_KEY_DIR}/root_of_trust_key.pem"
CCERT_COMMON_OPTION         = "-halgo SHA2-256 -salgo ECDSA-P256"
CCERT_COMMON_OPTION_append  = " \
	${@oe.utils.conditional("IMG_AUTH_MODE", "encsign", " -encimage -ealgo AES-CBC -iekey ${SYMLINK_NATIVE_BOOT_KEY_DIR}/cmn_key_idx0.txt", "",d)} \
"
MANIFEST_GENERATION_KCERT   = "${DIRPATH_MANIFEST_GENTOOL}/manifest_generation_tool.py genkcert ${KCERT_COMMON_OPTION}"
MANIFEST_GENERATION_CCERT   = "${DIRPATH_MANIFEST_GENTOOL}/manifest_generation_tool.py genccert ${CCERT_COMMON_OPTION}"
