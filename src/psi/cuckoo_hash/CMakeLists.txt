# set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES}
#     ${CMAKE_CURRENT_LIST_DIR}/cuckoo.cpp
# )
install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/cuckoo.h
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/psi/cuckoo_hash
)
set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES} PARENT_SCOPE)