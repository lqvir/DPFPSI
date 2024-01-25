
set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/Channel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/IoBuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/IOService.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Session.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SocketAdapter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TLS.cpp
)
install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/Channel.h
        ${CMAKE_CURRENT_LIST_DIR}/IoBuffer.h
        ${CMAKE_CURRENT_LIST_DIR}/utils.h
        ${CMAKE_CURRENT_LIST_DIR}/IOService.h
        ${CMAKE_CURRENT_LIST_DIR}/Session.h
        ${CMAKE_CURRENT_LIST_DIR}/SocketAdapter.h
        ${CMAKE_CURRENT_LIST_DIR}/TLS.h
        ${CMAKE_CURRENT_LIST_DIR}/Endpoint.h

    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/psi/common/Network
)



set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES} PARENT_SCOPE)
