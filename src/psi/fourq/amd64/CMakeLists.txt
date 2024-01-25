# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.

# Source files in this directory

if(HAVE_AVX2_EXTENSIONS)
    message(STATUS "AVX2")
    set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/consts.c
        ${CMAKE_CURRENT_LIST_DIR}/fp2_1271_AVX2.S
    )
else()
    set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/fp2_1271.S
    )
endif()


# Add header files for installation
install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/fp_x64.h
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/apsi/fourq/amd64
)

set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES} PARENT_SCOPE)
