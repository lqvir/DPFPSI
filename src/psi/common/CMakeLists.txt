set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/thread_pool_mgr.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/item.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stopwatch.cpp

)
install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/thread_pool_mgr.h
        ${CMAKE_CURRENT_LIST_DIR}/thread_pool.h
        ${CMAKE_CURRENT_LIST_DIR}/utils.h
        ${CMAKE_CURRENT_LIST_DIR}/item.h
        ${CMAKE_CURRENT_LIST_DIR}/stopwatch.h

    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/psi/common
)
set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES} PARENT_SCOPE)