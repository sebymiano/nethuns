# Copyright 2021 Larthia, University of Pisa. All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

cmake_minimum_required(VERSION 3.10)

project (nethuns)

#
# Compiler options...
#

# set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -g -O0 -march=native -fomit-frame-pointer -Wall -Wextra -Wshadow -fsanitize=address -fsanitize=undefined")
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -march=native -fomit-frame-pointer -Wall -Wextra -Wshadow -fsanitize=address -fsanitize=undefined")

set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -g -O2 -march=native -fomit-frame-pointer -Wall -Wextra -Wshadow")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -std=c++17 -march=native -fomit-frame-pointer -Wall -Wextra -Wshadow")

set (CMAKE_POSITION_INDEPENDENT_CODE ON)

option(NETHUNS_OPT_LIBPCAP              "Enable libpcap support" ON)
option(NETHUNS_OPT_BUILTIN_PCAP_READER  "Enable builtin pcap reader" OFF)
option(NETHUNS_OPT_TPACKET_V3           "Enable TPACKET v3 support" OFF)
option(NETHUNS_OPT_XDP                  "Enable XDP" ON)
option(NETHUNS_OPT_NETMAP               "Enable Netmap support" ON)

add_definitions(-D_GNU_SOURCE)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPCAP REQUIRED libpcap)

message(STATUS "Found libpcap:")
message(STATUS "  Version: ${LIBPCAP_VERSION}")
message(STATUS "  Libraries: ${LIBPCAP_LIBRARIES}")
message(STATUS "  Include Directories: ${LIBPCAP_INCLUDE_DIRS}")

set(LIBPCAP_LIBRARY ${LIBPCAP_LIBRARIES})
set(LIBPCAP_INCLUDE_DIR ${LIBPCAP_INCLUDE_DIRS})

include_directories(${LIBPCAP_INCLUDE_DIR})
# include_directories(/usr/local/opt/libpcap/include src)

if (NETHUNS_OPT_XDP)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/libbpf/)
	include_directories(${CMAKE_CURRENT_BINARY_DIR}/libxdp/include/)
endif()

#
# define the list of library to link...
#

set(NETHUNS_LIBS)

set(NETHUNS_SRC src/nethuns/nethuns.c
                src/nethuns/global.c)

if (NETHUNS_OPT_BUILTIN_PCAP_READER)
    	message("Nethuns: using built-in pcap reader")
	add_definitions(-DNETHUNS_USE_BUILTIN_PCAP_READER)
else()
    list(APPEND NETHUNS_LIBS ${LIBPCAP_LIBRARY})
    message("Nethuns: using libpcap to read files")
endif()

if (NETHUNS_OPT_TPACKET_V3)
    list(APPEND NETHUNS_SRC src/nethuns/sockets/tpacket_v3.c)

    message("Nethuns: tpcaketv3 ENABLED")
    add_definitions(-DNETHUNS_HAVE_TPACKET_V3)
endif()

if (NETHUNS_OPT_NETMAP)
    list(APPEND NETHUNS_SRC src/nethuns/sockets/netmap.c)
    list(APPEND NETHUNS_DEP -lnetmap)
    list(APPEND OTHER_LIB -lnetmap)

    message("Nethuns: netmap ENABLED")
    add_definitions(-DNETHUNS_HAVE_NETMAP)
endif()

if (NETHUNS_OPT_XDP)

    message("Nethuns: xdp ENABLED")
    add_definitions(-DNETHUNS_HAVE_XDP)

    find_package(ZLIB)

    pkg_check_modules(LIBXDP REQUIRED libxdp)

    message(STATUS "Found libxdp:")
    message(STATUS "  Version: ${LIBXDP_VERSION}")
    message(STATUS "  Libraries: ${LIBXDP_LIBRARIES}")
    message(STATUS "  Static Libraries: ${LIBXDP_STATIC_LIBRARIES}")
    message(STATUS "  Include Directories: ${LIBXDP_INCLUDE_DIRS}")

    # Check if libxdp version is 1.4.0 or greater
    if (NOT ${LIBXDP_VERSION} VERSION_GREATER_EQUAL "1.4.0")
        message(FATAL_ERROR "libxdp version must be 1.4.0 or greater")
    endif()

    include_directories(${LIBXDP_INCLUDE_DIRS})

    list(APPEND NETHUNS_SRC src/nethuns/sockets/xdp.c)
    list(APPEND NETHUNS_SRC src/nethuns/sockets/xsk_ext.c)

    list(APPEND NETHUNS_LIBS ${LIBXDP_LIBRARIES})
    list(APPEND NETHUNS_LIBS ZLIB::ZLIB)

endif()

if (NETHUNS_OPT_LIBPCAP)

    message("Nethuns: libpcap ENABLED")
    add_definitions(-DNETHUNS_HAVE_LIBPCAP)

    list(APPEND NETHUNS_SRC src/nethuns/sockets/libpcap.c)
    list(APPEND NETHUNS_LIBS ${LIBPCAP_LIBRARY})
endif()

add_custom_command(
	OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/src/nethuns/version.c
	        ${CMAKE_CURRENT_SOURCE_DIR}/src/nethuns/_version.c # fake target that is never created, here just to trigger the execution of makeRevision script
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/script/make_revision.sh ${CMAKE_CURRENT_SOURCE_DIR}  ${CMAKE_CURRENT_SOURCE_DIR}/src/nethuns/version.c.in ${CMAKE_CURRENT_SOURCE_DIR}/src/nethuns/version.c ${NETHUNS_OPT_LIBPCAP} ${NETHUNS_OPT_XDP} ${NETHUNS_OPT_NETMAP} ${NETHUNS_OPT_TPACKET_V3}
)


add_custom_target(revision ALL DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/nethuns/version.c)

list(APPEND NETHUNS_SRC src/nethuns/version.c)

add_library(nethuns STATIC ${NETHUNS_SRC})

target_link_libraries(nethuns ${NETHUNS_LIBS})

add_dependencies(nethuns revision)

if (DEBUG)
    target_compile_definitions(nethuns PRIVATE DEBUG=${DEBUG})
endif()


install(TARGETS nethuns DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY src/nethuns/ 	   	DESTINATION ${CMAKE_INSTALL_PREFIX}/include/nethuns FILES_MATCHING PATTERN "*.h" PATTERN src/nethuns/sockets/xdp EXCLUDE)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libbpf/bpf/      	DESTINATION ${CMAKE_INSTALL_PREFIX}/include/nethuns/sockets/xdp/bpf FILES_MATCHING PATTERN "*.h" )
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libxdp/include/  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/nethuns/sockets/xdp FILES_MATCHING PATTERN "*.h" )

