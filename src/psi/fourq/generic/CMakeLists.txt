# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.

# Add header files for installation
install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/fp.h
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/psi/fourq/generic
)

set(PSI_SOURCE_FILES ${PSI_SOURCE_FILES} PARENT_SCOPE)
