
include(${MPYADAPTORDIR}/component/minicv/base/cv.mk)
#utils
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/minicv/modminicv.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/minicv/datainput.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/minicv/imagecodec.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/minicv/imageproc.c")
if(${PY_BUILD_AI})
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/minicv/ml.c")
endif()
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/minicv/ui.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/minicv/videocodec.c")


if(${PY_BUILD_NETMGR})
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/netmgr/modnetmgr.c")
endif()

if(${PY_BUILD_BLENETCONFIG})
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/ble_netconfig/modblenetconfig.c")
endif()

if(${PY_BUILD_HTTP})
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/http/modhttp.c")
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/http/httpclient.c")
endif()

if(${PY_BUILD_TCP})
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/tcp/modtcp.c")
endif()

if(${PY_BUILD_UDP})
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/udp/modudp.c")
endif()

if(${PY_BUILD_MQTT})
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/mqtt/modmqtt.c")
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/mqtt/mqttclient.c")
endif()


if(${PY_BUILD_LINKSDK})
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/linksdk/modlinkkit.c")
endif()

if(${PY_BUILD_AUDIO})
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/audio/moduvoice.c")
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/audio/uvoicesnd.c")
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/audio/uvoiceplayer.c")
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/audio/uvoicerecorder.c")
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/audio/uvoicetts.c")
	add_definitions(-DUVOICE_EVENT_BY_BUFFER_QUEUE)
endif()

if(${PY_BUILD_OSS})
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/component/oss/modoss.c")
endif()
