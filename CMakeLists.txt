cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(Unbalanced-PSI)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}")

####################
# Dependencies     #
####################

find_package(OpenSSL REQUIRED)
find_package(Microsoft.GSL REQUIRED)
# find_package(Boost REQUIRED)
message(${VCPKG_TARGET_TRIPLET})
set(CMAKE_CXX_STANDARD 17)
####################
# Libs             #
####################
# Source Tree
set(PSI_INCLUDES_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
# Build tree
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

macro(psi_install_target target export)
    install(TARGETS ${target} EXPORT ${export}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endmacro()



# set(PSI_CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/psi)
message(${CMAKE_CURRENT_BINARY_DIR})


set(PSI_SOURCE_FILES "")
add_subdirectory(src/psi)

add_library(psi)
# include_directories(
#     ${OPENSSL_INCLUDE_DIR}
# )
target_include_directories(psi PUBLIC ${OPENSSL_INCLUDE_DIR})
target_link_libraries(psi 
    PRIVATE OpenSSL::Crypto 
    PUBLIC Microsoft.GSL::GSL)
target_sources(psi PRIVATE ${PSI_SOURCE_FILES})

target_include_directories(psi PUBLIC
    $<BUILD_INTERFACE:${PSI_INCLUDES_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(psi PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

psi_install_target(psi psiTargets)
####################
# Tests            #
####################
add_executable(psitest)

set(PSI_TEST_FILES "")
add_subdirectory(tests/psi)

target_sources(psitest PRIVATE ${PSI_TEST_FILES})

target_link_libraries(psitest PRIVATE psi)
message(${Microsoft.GSL_DIR})
add_executable(test test.cpp)
target_link_libraries(test OpenSSL::Crypto)

