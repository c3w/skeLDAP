### skeLDAP @c3w

### SYSTEM REQUIREMENTS
INIT_DIR="/etc/init.d"
NFS_INIT="${INIT_DIR}/nfs"
NFSLOCK_INIT="${INIT_DIR}/nfslock"
INSTALL_DIR="/usr/local/bin"
INSTALL_ETC="/etc/skeldap"

### SETUP DIRS
LDAP_SKEL="skel/ldap.conf.skel"
LDAP_PROD="production/ldap.conf"
INIT_LDIF_SKEL="skel/init.ldif.skel"
INIT_LDIF_PROD="production/init.ldif"
USER_LDIF_SKEL="skel/user.ldif.skel"
USER_LDIF_PROD="production/user.skel.ldif"
ENCRYPTEDPASSWD_FILE="skel/ldap.passwd"
SLAPD_SKEL="skel/slapd.conf.skel"
SLAPD_PROD="production/slapd.conf"
SKELOAD_SKEL="skel/skeload.sh.skel"
SKELOAD_PROD="production/skeload.sh"
UID_FILE="skel/uid.txt"

### PRODUCTION INFO
LDAP_USER="ldap"
LDAP_GROUP="ldap"
LDAP_DIR="/etc/openldap"
OPENLDAP_DATADIR="/var/local/openldap-data"
OPENLDAP_DIR="/var/local/openldap"
DB_CONFIG_EXAMPLE="${LDAP_DIR}/DB_CONFIG.example"
