cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(Unbalanced-PSI)

set(CMAKE_TOOLCHAIN_FILE "/home/yuyu/liuqi/vcpkg/scripts/buildsystems/vcpkg.cmake")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    #add_compile_options("/MTd")
else()
    #add_compile_options("/MT")
endif()
string(REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
string(REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
message(STATUS ${CMAKE_CXX_FLAGS_RELEASE})
####################
# Dependencies     #
####################
find_package(Kuku REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Microsoft.GSL REQUIRED)
# find_package(Boost REQUIRED)
message(${VCPKG_TARGET_TRIPLET})
set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set_property(DIRECTORY PROPERTY EP_BASE ${CMAKE_BINARY_DIR}/subprojects)
# set(STAGED_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/stage)
# message(STATUS "${PROJECT_NAME} staged install: ${STAGED_INSTALL_PREFIX}")
    set(
        BOOST_ROOT ${CMAKE_CURRENT_LIST_DIR}/../boost-release
        CACHE PATH "Path to internally built Boost installation root"
        FORCE
    )
    message(${BOOST_ROOT})
    set(
        BOOST_INCLUDEDIR ${BOOST_ROOT}/include
        CACHE PATH "Path to internally built Boost include directories"
        FORCE
    )
    set(
        BOOST_LIBRARYDIR ${BOOST_ROOT}/lib
        CACHE PATH "Path to internally built Boost library directories"
        FORCE
    )
list(APPEND BOOST_COMPONENTS_REQUIRED  system thread atomic regex date_time)
find_package(Boost REQUIRED QUIET COMPONENTS system regex)



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

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)






set(PSI_SOURCE_FILES "")
add_subdirectory(src/psi)

add_library(psi)
include(DetectArch)
if(PSI_FOURQ_AMD64)
    target_compile_options(psi PUBLIC -D_AMD64_)
    message(STATUS "FourQlib optimization: arch=AMD64")
else()
    target_compile_options(psi PUBLIC -D_GENERIC_)
    message(STATUS "FourQlib optimization: arch=GENERIC")
endif()

# Set AVX or AVX2 if not generic
if(PSI_FOURQ_AMD64)
    include(FindAVX)
    check_for_avx(psi)
    if (HAVE_AVX2_EXTENSIONS)
        target_compile_options(psi PUBLIC -D_AVX2_)
        message(STATUS "FourQlib optimization: simd=AVX2")
    elseif(HAVE_AVX_EXTENSIONS)
        target_compile_options(psi PUBLIC -D_AVX_)
        message(STATUS "FourQlib optimization: simd=AVX")
    endif()
else()
    message(STATUS "FourQlib optimization: simd=OFF")
endif()

if(MSVC)
    target_compile_options(psi PUBLIC -D__WINDOWS__)
elseif (UNIX)
    target_compile_options(psi PUBLIC -D__LINUX__)
endif()
include(CheckLanguage)
check_language(ASM)
if(CMAKE_ASM_COMPILER)
    enable_language(ASM)
    target_compile_options(psi PUBLIC -D_ASM_)
    message(STATUS "FourQlib optimization: asm=ON")
endif()

# set_target_properties(psi PROPERTIES LINK_FLAGS "-Wl,--stack,16777216")

# include_directories(
#     ${OPENSSL_INCLUDE_DIR}
# )
target_include_directories(psi PUBLIC ${OPENSSL_INCLUDE_DIR})
target_link_libraries(psi 
    PRIVATE OpenSSL::Crypto 
    PUBLIC Microsoft.GSL::GSL
    PUBLIC Kuku::kuku
    )
target_link_libraries(psi
    PUBLIC Boost::boost Boost::regex Boost::system
)
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

# Include(FetchContent)
# include(CTest)

# FetchContent_Declare(
#   Catch2
#   GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#   GIT_TAG        v3.3.2 # or a later release
# )

# FetchContent_MakeAvailable(Catch2)
# include(Catch)
add_executable(psitest)

set(PSI_TEST_FILES "")
add_subdirectory(tests/psi)

target_sources(psitest PRIVATE ${PSI_TEST_FILES})

target_link_libraries(psitest PRIVATE psi)
# target_link_libraries(psitest PRIVATE Catch2::Catch2WithMain)
# catch_discover_tests(psitest)

# message(${Microsoft.GSL_DIR})

# add_executable(test test.cpp)
# target_link_libraries(test OpenSSL::Crypto)
#enable_testing()
#add_test(
#  NAME catch_test
#  COMMAND $<TARGET_FILE:cpp_test> --success
#  )
