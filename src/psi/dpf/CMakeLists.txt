
set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/dpf_client.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dpf_server.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dpf_common.cpp
)

install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/dpf_client.h
        ${CMAKE_CURRENT_LIST_DIR}/dpf_server.h
        ${CMAKE_CURRENT_LIST_DIR}/dpf_common.h
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/psi/dpf
)

set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES} PARENT_SCOPE)
