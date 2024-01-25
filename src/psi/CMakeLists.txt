


set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/client.cpp
    ${CMAKE_CURRENT_LIST_DIR}/server.cpp
    ${CMAKE_CURRENT_LIST_DIR}/aid_server.cpp

)
install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/client.h
        ${CMAKE_CURRENT_LIST_DIR}/server.h
        ${CMAKE_CURRENT_LIST_DIR}/aid_server.h
        ${CMAKE_CURRENT_LIST_DIR}/param.h
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/psi
)
add_subdirectory(oprf)
add_subdirectory(common)
add_subdirectory(dpf)
add_subdirectory(cuckoo_hash)
add_subdirectory(fourq)
set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES} PARENT_SCOPE)