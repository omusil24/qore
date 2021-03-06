cmake_minimum_required(VERSION 3.0.0)

project(qore-language)

option(ICONV_TRANSLIT
"Force //Translit to iconv encoding to do transliteration, if OFF it is tested for"
OFF)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 8)
set(VERSION_SUB 12)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "release")
endif (NOT CMAKE_BUILD_TYPE)
string(TOLOWER ${CMAKE_BUILD_TYPE} QORE_BUILD_TYPE_LWR)
if (${QORE_BUILD_TYPE_LWR} MATCHES "debug")
    add_definitions(-DDEBUG)
else ()
    add_definitions(-DNDEBUG)
endif ()

execute_process(COMMAND ${CMAKE_SOURCE_DIR}/cmake/get-define.sh QORE_MODULE_API_MAJOR ${CMAKE_SOURCE_DIR}/include/qore/ModuleManager.h
                OUTPUT_VARIABLE MODULE_API_MAJOR 
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${CMAKE_SOURCE_DIR}/cmake/get-define.sh QORE_MODULE_API_MINOR ${CMAKE_SOURCE_DIR}/include/qore/ModuleManager.h
                OUTPUT_VARIABLE MODULE_API_MINOR 
                OUTPUT_STRIP_TRAILING_WHITESPACE)

include("${CMAKE_SOURCE_DIR}/cmake/QoreMacros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/QoreMacrosIntern.cmake")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include_directories( ${CMAKE_BINARY_DIR} )
include_directories( ${CMAKE_BINARY_DIR}/include/ )
include_directories( ${CMAKE_SOURCE_DIR}/include )
include_directories( ${CMAKE_SOURCE_DIR}/include/qore )

include(CheckIncludeFiles)
include(GNUInstallDirs)
# usage: CHECK_INCLUDE_FILES (<header> <RESULT_VARIABLE> )
#check_include_files(getopt.h HAVE_GETOPT_H)
#check_include_files(malloc.h HAVE_MALLOC_H)
#check_include_files("sys/param.h;sys/mount.h" HAVE_SYS_MOUNT_H)
#configure_file(${CMAKE_SOURCE_DIR}/include/qore/intern/unix-config.h.in
#               ${CMAKE_BINARY_DIR}/include/qore/intern/unix-config.h)
add_definitions(-DHAVE_UNIX_CONFIG_H)
set(HAVE_SIGNAL_HANDLING true)
#add_definitions(-DHAVE_GETOPT_H)
#add_definitions(-DHAVE_STRCASESTR)

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(myprefix ${CMAKE_INSTALL_PREFIX})
set(includedir ${CMAKE_INSTALL_FULL_INCLUDEDIR})
set(libdir ${CMAKE_INSTALL_FULL_LIBDIR})
set(MODULE_DIR ${CMAKE_INSTALL_FULL_LIBDIR}/qore-modules)
set(MODULE_VER_DIR ${CMAKE_INSTALL_FULL_LIBDIR}/qore-modules/${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_SUB})
set(USER_MODULE_DIR ${CMAKE_INSTALL_FULL_DATADIR}/qore-modules)
set(USER_MODULE_VER_DIR ${CMAKE_INSTALL_FULL_DATADIR}/qore-modules/${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_SUB})

# libqore - unused for now
set(CPP_SRC_LIB lib/single-compilation-unit.cpp )

set(QPP_SRC_LIB
	lib/QC_AbstractSmartLock.qpp
	lib/QC_AutoGate.qpp
	lib/QC_AutoLock.qpp
	lib/QC_AutoReadLock.qpp
	lib/QC_AutoWriteLock.qpp
	lib/QC_Condition.qpp
	lib/QC_Counter.qpp
	lib/QC_AbstractIterator.qpp
	lib/QC_AbstractQuantifiedIterator.qpp
	lib/QC_AbstractBidirectionalIterator.qpp
	lib/QC_AbstractQuantifiedBidirectionalIterator.qpp
	lib/QC_ListIterator.qpp
	lib/QC_ListReverseIterator.qpp
	lib/QC_HashIterator.qpp
	lib/QC_HashReverseIterator.qpp
	lib/QC_HashKeyIterator.qpp
	lib/QC_HashKeyReverseIterator.qpp
	lib/QC_HashPairIterator.qpp
	lib/QC_HashPairReverseIterator.qpp
	lib/QC_ObjectIterator.qpp
	lib/QC_ObjectReverseIterator.qpp
	lib/QC_ObjectKeyIterator.qpp
	lib/QC_ObjectKeyReverseIterator.qpp
	lib/QC_ObjectPairIterator.qpp
	lib/QC_ObjectPairReverseIterator.qpp
	lib/QC_HashListIterator.qpp
	lib/QC_HashListReverseIterator.qpp
	lib/QC_ListHashIterator.qpp
	lib/QC_ListHashReverseIterator.qpp
	lib/QC_FileLineIterator.qpp
	lib/QC_SingleValueIterator.qpp
	lib/QC_AbstractDatasource.qpp
	lib/QC_RangeIterator.qpp
	lib/QC_DataLineIterator.qpp
	lib/QC_Datasource.qpp
	lib/QC_DatasourcePool.qpp
	lib/QC_Dir.qpp
	lib/QC_ReadOnlyFile.qpp
	lib/QC_File.qpp
	lib/QC_FtpClient.qpp
	lib/QC_Gate.qpp
	lib/QC_GetOpt.qpp
	lib/QC_HTTPClient.qpp
	lib/QC_Mutex.qpp
	lib/QC_Program.qpp
	lib/QC_Queue.qpp
	lib/QC_RWLock.qpp
	lib/QC_SQLStatement.qpp
	lib/QC_Sequence.qpp
	lib/QC_Socket.qpp
	lib/QC_TermIOS.qpp
	lib/QC_TimeZone.qpp
        lib/QC_TreeMap.qpp
	lib/QC_SSLCertificate.qpp
	lib/QC_SSLPrivateKey.qpp
	lib/QC_ThreadPool.qpp
	lib/Pseudo_QC_All.qpp
	lib/Pseudo_QC_Nothing.qpp
	lib/Pseudo_QC_Date.qpp
	lib/Pseudo_QC_Object.qpp
	lib/Pseudo_QC_Hash.qpp
	lib/Pseudo_QC_String.qpp
	lib/Pseudo_QC_Binary.qpp
	lib/Pseudo_QC_List.qpp
	lib/Pseudo_QC_Bool.qpp
	lib/Pseudo_QC_Int.qpp
	lib/Pseudo_QC_Float.qpp
	lib/Pseudo_QC_Number.qpp
	lib/Pseudo_QC_Closure.qpp
	lib/Pseudo_QC_Callref.qpp
	lib/ql_misc.qpp
	lib/ql_compression.qpp
	lib/ql_thread.qpp
	lib/ql_crypto.qpp
	lib/ql_lib.qpp
	lib/ql_file.qpp
	lib/ql_string.qpp
	lib/ql_time.qpp
	lib/ql_math.qpp
	lib/ql_list.qpp
	lib/ql_type.qpp
	lib/ql_pwd.qpp
	lib/ql_object.qpp
	lib/ql_env.qpp
	lib/ql_dbi.qpp
	lib/ql_context.qpp
	lib/qc_option.qpp
	lib/qc_errno.qpp
	lib/qc_qore.qpp
)

# qore interpreter
set(QORE_CPP_SRC qore-single-compilation-unit.cpp)
set(QR_CPP_SRC qr-single-compilation-unit.cpp)

qore_find_pthreads()
find_package(ICONV REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX 2.5.35 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(MPFR REQUIRED)
find_package(BZip2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCRE REQUIRED libpcre)

create_git_revision()

qore_os_checks()
qore_iconv_translit_check()
qore_openssl_checks()
qore_mpfr_checks()

qore_check_headers_cxx(fcntl.h inttypes.h netdb.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/socket.h sys/time.h unistd.h execinfo.h cxxabi.h arpa/inet.h sys/socket.h sys/statvfs.h winsock2.h ws2tcpip.h glob.h sys/un.h termios.h netinet/tcp.h pwd.h sys/wait.h getopt.h stdint.h grp.h)

qore_search_libs(LIBQORE_LIBS setsockopt socket)
qore_search_libs(LIBQORE_LIBS gethostbyname nsl)
qore_search_libs(LIBQORE_LIBS clock_gettime rt)

set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${LIBQORE_LIBS})
qore_check_funcs(bzero floor gethostbyaddr gethostbyname gethostname gettimeofday memmove memset mkfifo putenv regcomp select socket setsockopt getsockopt strcasecmp strchr strdup strerror strspn strstr atoll strtol strtoll isblank localtime_r gmtime_r exp2 clock_gettime realloc timegm seteuid setegid setenv unsetenv round pthread_attr_getstacksize getpwuid_r getpwnam_r getgrgid_r getgrnam_r backtrace glob system inet_ntop inet_pton lstat fsync lchown chown setsid setuid mkfifo random kill getppid getgid getegid getuid geteuid setuid seteuid setgid setegid sleep usleep nanosleep readlink symlink access strcasestr strncasecmp setgroups getgroups realpath memmem)
qore_func_strerror_r()
unset(CMAKE_REQUIRED_LIBRARIES)

qore_other_checks()
qore_dlcpp_check()
qore_gethost_checks()
qore_stl_hash_map(include/qore/hash_map_include.h)
qore_stl_slist(include/qore/slist_include.h)
qore_cpu_checks()

execute_process(COMMAND uname -a OUTPUT_VARIABLE QORE_BUILD_HOST OUTPUT_STRIP_TRAILING_WHITESPACE)
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
   set(TARGET_BITS 64)
else()
   set(TARGET_BITS 32)
endif()

bison_target(qoreparser lib/parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
flex_target(qorescanner lib/scanner.lpp ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)
add_flex_bison_dependency(qorescanner qoreparser)

# qpp
if(NOT HAVE_GETOPT_H)
set(QPP_GETOPT_LONG_SRC lib/getopt_long.cpp)
endif()
add_executable(qpp
    lib/qpp.cpp
    ${QPP_GETOPT_LONG_SRC}
)
# just fake qpp for macro
set(QORE_QPP_EXECUTABLE ${CMAKE_BINARY_DIR}/qpp)
target_link_libraries(qpp ${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES})

# libqore generators
qore_wrap_qpp_value(QPP_CPP_SRC_LIB ${QPP_SRC_LIB})

add_custom_target(QPP_GENERATED_FILES DEPENDS ${QPP_CPP_SRC_LIB})
add_custom_target(BISON_GENERATED_FILES DEPENDS ${BISON_qoreparser_OUTPUTS})
add_custom_target(FLEX_GENERATED_FILES DEPENDS ${FLEX_qorescanner_OUTPUTS})

add_library(libqore SHARED ${CPP_SRC_LIB})
if (APPLE)
    set_target_properties(libqore PROPERTIES INSTALL_NAME_DIR ${CMAKE_INSTALL_FULL_LIBDIR})
endif (APPLE)
add_dependencies(QPP_GENERATED_FILES qpp)
add_dependencies(libqore QPP_GENERATED_FILES BISON_GENERATED_FILES FLEX_GENERATED_FILES)
target_compile_definitions(libqore PUBLIC -D_QORE_LIB_INTERN)
target_include_directories(libqore PUBLIC ${BZIP2_INCLUDE_DIR} ${ICONV_INCLUDE_DIR} ${MPFR_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${PCRE_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(libqore ${CMAKE_THREAD_LIBS_INIT} ${BZIP2_LIBRARIES} ${CMAKE_DL_LIBS} ${ICONV_LIBRARY} ${MPFR_LIBRARIES} ${OPENSSL_LIBRARIES} ${PCRE_LDFLAGS} ${ZLIB_LIBRARIES})
set_property(TARGET libqore PROPERTY OUTPUT_NAME qore)

add_executable(qore ${QORE_CPP_SRC})
target_link_libraries(qore libqore)
add_executable(qr ${QR_CPP_SRC})
target_link_libraries(qr libqore)

configure_file(${CMAKE_SOURCE_DIR}/cmake/unix-config.h.cmake
               ${CMAKE_BINARY_DIR}/include/qore/intern/unix-config.h)
configure_file(${CMAKE_SOURCE_DIR}/cmake/qore-version.h.cmake
               ${CMAKE_BINARY_DIR}/include/qore/qore-version.h)
configure_file(${CMAKE_SOURCE_DIR}/cmake/QoreConfigVersion.cmake.in
               ${CMAKE_BINARY_DIR}/cmake/QoreConfigVersion.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/cmake/QoreConfig.cmake.in
               ${CMAKE_BINARY_DIR}/cmake/QoreConfig.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/qore.pc.in
               ${CMAKE_BINARY_DIR}/qore.pc @ONLY)

install(PROGRAMS doxygen/qdx
        DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
install(TARGETS qore qpp qr libqore
	RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
install(FILES ${CMAKE_BINARY_DIR}/cmake/QoreConfig.cmake ${CMAKE_BINARY_DIR}/cmake/QoreConfigVersion.cmake cmake/QoreMacros.cmake
        DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/Qore)
install(FILES ${CMAKE_BINARY_DIR}/qore.pc
        DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig)
install(DIRECTORY include/qore
        DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}
        PATTERN intern EXCLUDE
        PATTERN minitest.hpp EXCLUDE
        PATTERN macros-none.h EXCLUDE
        PATTERN hash_map_include.h EXCLUDE
        PATTERN slist_include.h EXCLUDE
        PATTERN qore-version.h EXCLUDE)
install(FILES ${CMAKE_BINARY_DIR}/include/qore/qore-version.h
        DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/qore)
install(FILES qore.1
        DESTINATION ${CMAKE_INSTALL_FULL_MANDIR}/man1)
install(DIRECTORY qlib/
        DESTINATION ${USER_MODULE_VER_DIR})

message(STATUS "CXX ${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES}")
message(WARNING "Please do not use cmake for production/real builds. Cmake support is experimental and it does not contant all required setup")
