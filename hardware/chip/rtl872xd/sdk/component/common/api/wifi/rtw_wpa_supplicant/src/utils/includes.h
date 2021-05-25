#ifndef INCLUDES_H
#define INCLUDES_H

#define CONFIG_NO_STDOUT_DEBUG

#ifndef CONFIG_NO_STDOUT_DEBUG

/* Event messages with fixed prefix */
/** Authentication completed successfully and data connection enabled */
#define WPA_EVENT_CONNECTED "CTRL-EVENT-CONNECTED "
/** Disconnected, data connection is not available */
#define WPA_EVENT_DISCONNECTED "CTRL-EVENT-DISCONNECTED "
/** Association rejected during connection attempt */
#define WPA_EVENT_ASSOC_REJECT "CTRL-EVENT-ASSOC-REJECT "
/** Authentication rejected during connection attempt */
#define WPA_EVENT_AUTH_REJECT "CTRL-EVENT-AUTH-REJECT "
/** wpa_supplicant is exiting */
#define WPA_EVENT_TERMINATING "CTRL-EVENT-TERMINATING "
/** Password change was completed successfully */
#define WPA_EVENT_PASSWORD_CHANGED "CTRL-EVENT-PASSWORD-CHANGED "
/** EAP-Request/Notification received */
#define WPA_EVENT_EAP_NOTIFICATION "CTRL-EVENT-EAP-NOTIFICATION "
/** EAP authentication started (EAP-Request/Identity received) */
#define WPA_EVENT_EAP_STARTED "CTRL-EVENT-EAP-STARTED "
/** EAP method proposed by the server */
#define WPA_EVENT_EAP_PROPOSED_METHOD "CTRL-EVENT-EAP-PROPOSED-METHOD "
/** EAP method selected */
#define WPA_EVENT_EAP_METHOD "CTRL-EVENT-EAP-METHOD "
/** EAP peer certificate from TLS */
#define WPA_EVENT_EAP_PEER_CERT "CTRL-EVENT-EAP-PEER-CERT "
/** EAP peer certificate alternative subject name component from TLS */
#define WPA_EVENT_EAP_PEER_ALT "CTRL-EVENT-EAP-PEER-ALT "
/** EAP TLS certificate chain validation error */
#define WPA_EVENT_EAP_TLS_CERT_ERROR "CTRL-EVENT-EAP-TLS-CERT-ERROR "
/** EAP status */
#define WPA_EVENT_EAP_STATUS "CTRL-EVENT-EAP-STATUS "
/** EAP authentication completed successfully */
#define WPA_EVENT_EAP_SUCCESS "CTRL-EVENT-EAP-SUCCESS "
/** EAP authentication failed (EAP-Failure received) */
#define WPA_EVENT_EAP_FAILURE "CTRL-EVENT-EAP-FAILURE "
/** Network block temporarily disabled (e.g., due to authentication failure) */
#define WPA_EVENT_TEMP_DISABLED "CTRL-EVENT-SSID-TEMP-DISABLED "
/** Temporarily disabled network block re-enabled */
#define WPA_EVENT_REENABLED "CTRL-EVENT-SSID-REENABLED "
/** New scan started */
#define WPA_EVENT_SCAN_STARTED "CTRL-EVENT-SCAN-STARTED "
/** New scan results available */
#define WPA_EVENT_SCAN_RESULTS "CTRL-EVENT-SCAN-RESULTS "
/** Scan command failed */
#define WPA_EVENT_SCAN_FAILED "CTRL-EVENT-SCAN-FAILED "
/** wpa_supplicant state change */
#define WPA_EVENT_STATE_CHANGE "CTRL-EVENT-STATE-CHANGE "
/** A new BSS entry was added (followed by BSS entry id and BSSID) */
#define WPA_EVENT_BSS_ADDED "CTRL-EVENT-BSS-ADDED "
/** A BSS entry was removed (followed by BSS entry id and BSSID) */
#define WPA_EVENT_BSS_REMOVED "CTRL-EVENT-BSS-REMOVED "
/** No suitable network was found */
#define WPA_EVENT_NETWORK_NOT_FOUND "CTRL-EVENT-NETWORK-NOT-FOUND "
/** Change in the signal level was reported by the driver */
#define WPA_EVENT_SIGNAL_CHANGE "CTRL-EVENT-SIGNAL-CHANGE "
/** Regulatory domain channel */
#define WPA_EVENT_REGDOM_CHANGE "CTRL-EVENT-REGDOM-CHANGE "

/** RSN IBSS 4-way handshakes completed with specified peer */
#define IBSS_RSN_COMPLETED "IBSS-RSN-COMPLETED "

/** Notification of frequency conflict due to a concurrent operation.
 *
 * The indicated network is disabled and needs to be re-enabled before it can
 * be used again.
 */
#define WPA_EVENT_FREQ_CONFLICT "CTRL-EVENT-FREQ-CONFLICT "
/** Frequency ranges that the driver recommends to avoid */
#define WPA_EVENT_AVOID_FREQ "CTRL-EVENT-AVOID-FREQ "
/** WPS overlap detected in PBC mode */
#define WPS_EVENT_OVERLAP "WPS-OVERLAP-DETECTED "
/** Available WPS AP with active PBC found in scan results */
#define WPS_EVENT_AP_AVAILABLE_PBC "WPS-AP-AVAILABLE-PBC "
/** Available WPS AP with our address as authorized in scan results */
#define WPS_EVENT_AP_AVAILABLE_AUTH "WPS-AP-AVAILABLE-AUTH "
/** Available WPS AP with recently selected PIN registrar found in scan results
 */
#define WPS_EVENT_AP_AVAILABLE_PIN "WPS-AP-AVAILABLE-PIN "
/** Available WPS AP found in scan results */
#define WPS_EVENT_AP_AVAILABLE "WPS-AP-AVAILABLE "
/** A new credential received */
#define WPS_EVENT_CRED_RECEIVED "WPS-CRED-RECEIVED "
/** M2D received */
#define WPS_EVENT_M2D "WPS-M2D "
/** WPS registration failed after M2/M2D */
#define WPS_EVENT_FAIL "WPS-FAIL "
/** WPS registration completed successfully */
#define WPS_EVENT_SUCCESS "WPS-SUCCESS "
/** WPS enrollment attempt timed out and was terminated */
#define WPS_EVENT_TIMEOUT "WPS-TIMEOUT "
/* PBC mode was activated */
#define WPS_EVENT_ACTIVE "WPS-PBC-ACTIVE "
/* PBC mode was disabled */
#define WPS_EVENT_DISABLE "WPS-PBC-DISABLE "

#define WPS_EVENT_ENROLLEE_SEEN "WPS-ENROLLEE-SEEN "

#define WPS_EVENT_OPEN_NETWORK "WPS-OPEN-NETWORK "

/* WPS ER events */
#define WPS_EVENT_ER_AP_ADD "WPS-ER-AP-ADD "
#define WPS_EVENT_ER_AP_REMOVE "WPS-ER-AP-REMOVE "
#define WPS_EVENT_ER_ENROLLEE_ADD "WPS-ER-ENROLLEE-ADD "
#define WPS_EVENT_ER_ENROLLEE_REMOVE "WPS-ER-ENROLLEE-REMOVE "
#define WPS_EVENT_ER_AP_SETTINGS "WPS-ER-AP-SETTINGS "
#define WPS_EVENT_ER_SET_SEL_REG "WPS-ER-AP-SET-SEL-REG "

#endif /* CONFIG_NO_STDOUT_DEBUG */

//#define DBG_871X(...)  vTaskDelay(100)
//DBG_871X_LEVEL(_drv_always_, "no beacon for a long time, disconnect or roaming\n");
//#define DBG_871X(...) DBG_871X_LEVEL(_drv_always_,...)

#endif /* INCLUDES_H */
