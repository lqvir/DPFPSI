
set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/dhoprf_sender.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dhoprf_receiver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ecpoint_openssl.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GCOPRF_receiver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GCOPRF_sender.cpp


)
install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/dhoprf_sender.h
        ${CMAKE_CURRENT_LIST_DIR}/dhoprf_receiver.h
        ${CMAKE_CURRENT_LIST_DIR}/ecpoint_openssl.h
        ${CMAKE_CURRENT_LIST_DIR}/dhoprf_common.h

        ${CMAKE_CURRENT_LIST_DIR}/GCOPRF_receiver.h
        ${CMAKE_CURRENT_LIST_DIR}/GCOPRF_sender.h
        ${CMAKE_CURRENT_LIST_DIR}/GCOPRF.h
        DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/psi/oprf
)



set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES} PARENT_SCOPE)
