set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/thread_pool_mgr.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/item.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stopwatch.cpp
    ${CMAKE_CURRENT_LIST_DIR}/BitOpreate.cpp


)
install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/thread_pool_mgr.h
        ${CMAKE_CURRENT_LIST_DIR}/thread_pool.h
        ${CMAKE_CURRENT_LIST_DIR}/utils.h
        ${CMAKE_CURRENT_LIST_DIR}/item.h
        ${CMAKE_CURRENT_LIST_DIR}/stopwatch.h
        ${CMAKE_CURRENT_LIST_DIR}/BitOpreate.h

    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/psi/common
)
add_subdirectory(Network)

set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES} PARENT_SCOPE)