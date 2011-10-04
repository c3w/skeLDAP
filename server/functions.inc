### skeLDAP @c3w

function serverReqs {
# check system requirements
# openldap
if [ ! -f "${LDAP_DIR}/slapd.conf" ]; then { echo "OpenLDAP Server is not installed."; return 1; }; fi
if [ ! -f /usr/bin/ldapadd ]; then { echo "OpenLDAP Clients is not installed."; return 1; }; fi
# nfs
if [ ! -f "${NFS_INIT}" ]; then { echo "NFS is not installed."; return 1; }; fi
# nfslock
if [ ! -f "${NFSLOCK_INIT}" ]; then { echo "NFSLOCK is not installed."; return 1; }; fi
}


function serverConfig {
echo -n "Enter your top-level domain name (e.g. twitter.com): "
read FULLDOMAIN
echo -n "Enter the LDAP ServerName: "
read LDAPSERVER

DOMAIN="${FULLDOMAIN%%.*}"
TLD="${FULLDOMAIN##*.}"

echo "We're going to create an SMD5 Encrypted LDAP Password"
echo
slappasswd -h '{SMD5}'|cat - > "${ENCRYPTEDPASSWD_FILE}"

echo
echo "dc=${DOMAIN}, dc=${TLD}"
echo "URI	ldap://${LDAPSERVER}/"
echo "Encrypted Password: $(cat ${ENCRYPTEDPASSWD_FILE})"
echo
echo -n "Is this Correct? [Y/n]: "
read CONFIRM
echo

ENCRYPTEDPASSWD="$(cat ${ENCRYPTEDPASSWD_FILE})"


case "${CONFIRM}" in
	"n"|"n"|"no"|"No"|"NO")
		return 1
		break
		;;
	*)
		sed -e \
		"s/%%DOMAIN%%/${DOMAIN}/;s/%%TLD%%/${TLD}/;s/%%LDAPSERVER%%/${LDAPSERVER}/" \
		"${LDAP_SKEL}" > "${LDAP_PROD}" || return 1
		sed -e \
		"s/%%DOMAIN%%/${DOMAIN}/;s/%%TLD%%/${TLD}/" \
		"${INIT_LDIF_SKEL}" > "${INIT_LDIF_PROD}" || return 1
		sed -e \
		"s/%%DOMAIN%%/${DOMAIN}/;s/%%TLD%%/${TLD}/" \
		"${USER_LDIF_SKEL}" > "${USER_LDIF_PROD}" || return 1
		sed -e \
		"s/%%DOMAIN%%/${DOMAIN}/;s/%%TLD%%/${TLD}/" \
		"${USERPASSWD_LDIF_SKEL}" > "${USERPASSWD_LDIF_PROD}" || return 1
		sed -e \
		"s/%%DOMAIN%%/${DOMAIN}/;s/%%TLD%%/${TLD}/" \
		"${SKELOAD_SKEL}" > "${SKELOAD_PROD}" || return 1
		sed -e \
		"s/%%DOMAIN%%/${DOMAIN}/;s/%%TLD%%/${TLD}/" \
		"${SKELSEARCH_SKEL}" > "${SKELSEARCH_PROD}" || return 1
		sed -e \
		"s/%%DOMAIN%%/${DOMAIN}/;s/%%TLD%%/${TLD}/" \
		"${SKELDEL_SKEL}" > "${SKELDEL_PROD}" || return 1
		sed -e \
		"s/%%DOMAIN%%/${DOMAIN}/;s/%%TLD%%/${TLD}/" \
		"${SLAPD_SKEL}" > "${SLAPD_PROD}" || return 1
		echo "rootpw	${ENCRYPTEDPASSWD}" >> "${SLAPD_PROD}"
		;;
esac
}

function copyProduction {
if [ ! -d "${OPENLDAP_DATADIR}" ]; then { mkdir -p "${OPENLDAP_DATADIR}" && chown "${LDAP_USER}":"${LDAP_GROUP}" "${OPENLDAP_DATADIR}"|| return 1; }; fi

if [ ! -d "${OPENLDAP_DIR}" ]; then { mkdir -p "${OPENLDAP_DIR}" &&  chown "${LDAP_USER}":"${LDAP_GROUP}" "${OPENLDAP_DIR}"|| return 1; }; fi

if [ ! -d "${INSTALL_ETC}" ]; then { mkdir -p "${INSTALL_ETC}" || return 1; }; fi

if [ ! -f "${DB_CONFIG_EXAMPLE}" ]; then {
	echo "Missing ${DB_CONFIG_EXAMPLE}"; return 1;
} else {
	cp "${DB_CONFIG_EXAMPLE}" "${OPENLDAP_DATADIR}"/DB_CONFIG \
	&& echo "Copying ${DB_CONFIG_EXAMPLE} to ${OPENLDAP_DATADIR}" \
	|| return 1;
}; fi 

cp "${LDAP_PROD}" "${INSTALL_ETC}" || return 1;
cp "${LDAP_PROD}" /etc || return 1;
cp "${USER_LDIF_PROD}" "${INSTALL_ETC}" || return 1;
cp "${USERPASSWD_LDIF_PROD}" "${INSTALL_ETC}" || return 1;
cp "${INIT_LDIF_PROD}" "${INSTALL_ETC}" || return 1;
cp "${SLAPD_PROD}" "${LDAP_DIR}" || return 1;
cp "${SKELOAD_PROD}" "${INSTALL_DIR}" && chmod u+x ${INSTALL_DIR}/skeload.sh || return 1;
cp "${SKELMOD_PROD}" "${INSTALL_DIR}" && chmod u+x ${INSTALL_DIR}/skelmod.sh || return 1;
cp "${SKELSEARCH_PROD}" "${INSTALL_DIR}" && chmod u+x ${INSTALL_DIR}/skelsearch || return 1;
cp "${SKELDEL_PROD}" "${INSTALL_DIR}" && chmod u+x ${INSTALL_DIR}/skeldel || return 1;
cp "${UID_FILE}" "${INSTALL_ETC}" || return 1;
cp scripts/* "${INSTALL_DIR}" || return 1;
cp skel/pam.sshd /etc/pam.d/sshd
cp skel/pam.passwd /etc/pam.d/passwd

}

function startServices {
chkconfig ldap on && service ldap restart  || return 1;
chkconfig nfs on && service nfs restart  || return 1;
chkconfig nfslock on && service nfslock restart  || return 1;
}

function initLDAP {
echo "Initializing LDAP Database"
${INSTALL_DIR}/skeload.sh ${INSTALL_ETC}/init.ldif || return 1;
}
