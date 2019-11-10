cmake_minimum_required(VERSION 3.2)
project(range VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "Release")

set(SOURCES
	ip_filter.cpp
	)
set(HEADERS_LIB
	ip_filter_lib.h
	utils.h
	)
set(SOURCES_LIB
	ip_filter_lib.cpp
	utils.cpp
	)
set(DATA_FILES
	Readme.md
	conanfile.txt
	)
option(OPTIMIZE_MEMORY "Enable memory optimization" true)
option(USE_VECTOR "Enable using vector for ip_pool" false)
option(USE_LIST "Enable using list for ip_pool" false)
configure_file(config.h.in config.h)
add_executable(range ${SOURCES} ${HEADERS_LIB} ${DATA_FILES})
add_library(ip_filter_lib STATIC ${SOURCES_LIB} ${HEADERS_LIB})
target_link_libraries(range ip_filter_lib)
set_target_properties(range ip_filter_lib PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	IF(WIN32)
		#COMPILE_OPTIONS "-Wall" # A lot of messages (in std libraries)
		COMPILE_OPTIONS "/permissive-;/experimental:preprocessor"
	ELSE()
		COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
	ENDIF()
)

### tests ###
add_executable(tests tests.cpp ${HEADERS_LIB})
find_package(Boost COMPONENTS unit_test_framework filesystem REQUIRED)
set_target_properties(tests PROPERTIES
	COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
	INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS}
	LINK_DIRECTORIES ${Boost_LIBRARY_DIRS}
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	IF(WIN32)
		#COMPILE_OPTIONS "-Wall" # A lot of messages (in std libraries)
		COMPILE_OPTIONS "/permissive-;/experimental:preprocessor"
	ELSE()
		COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
	ENDIF()
)
target_link_libraries(tests
  ${Boost_LIBRARIES}
  ip_filter_lib
  )
enable_testing()
add_test(ip_filter_tests tests)
###

### Packaging ###
install(TARGETS range RUNTIME DESTINATION bin)
install(TARGETS tests RUNTIME DESTINATION lib/ip_filter/tests/build/bin)
install(FILES ip_filter.tsv ip_filter.tst DESTINATION lib/ip_filter/tests)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT linkfly@yandex.ru)

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

