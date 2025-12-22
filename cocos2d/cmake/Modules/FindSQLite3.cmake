# Find SQLite3
#
#  SQLITE3_INCLUDE_DIRS - where to find sqlite3.h header.
#  SQLITE3_LIBRARIES    - List of SQLite3 libraries.
#  SQLITE3_FOUND        - True if SQLite3 found.


set(SQLITE3_FOUND false)

find_path(SQLITE3_INCLUDE_DIRS
	NAMES
		sqlite3.h
	PATHS
		${CMAKE_CURRENT_SOURCE_DIR}/../external/sqlite
		/usr/include
		/usr/local/include
		/opt/local/include
		/sw/include
)

find_library(SQLITE3_LIBRARIES
	NAMES
		sqlite3
	PATH_SUFFIXES
		lib64
		lib
	PATHS
		${CMAKE_BINARY_DIR}/lib
		${CMAKE_CURRENT_SOURCE_DIR}/../external/sqlite
		/usr/lib
		/usr/local/lib
		/opt/local/lib
		/sw/lib
		/usr/lib/x86_64-linux-gnu
)


if (SQLITE3_INCLUDE_DIRS AND SQLITE3_LIBRARIES)
	set(SQLITE3_FOUND TRUE)
	#these next two are needed by the CocosBuildHelpers cocos_find_package macro
	set(SQLITE3_INCLUDE_DIR ${SQLITE3_INCLUDE_DIRS})
	set(SQLITE3_LIBRARY ${SQLITE3_LIBRARIES})
endif (SQLITE3_INCLUDE_DIRS AND SQLITE3_LIBRARIES)

if (NOT SQLITE3_FOUND)
	# Try to use our own compiled sqlite3 library
	set(SQLITE3_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../external/sqlite)
	# Directly use ext_sqlite3 library which will be built
	set(SQLITE3_LIBRARIES ext_sqlite3)
	set(SQLITE3_FOUND TRUE)
	#these next two are needed by the CocosBuildHelpers cocos_find_package macro
	set(SQLITE3_INCLUDE_DIR ${SQLITE3_INCLUDE_DIRS})
	set(SQLITE3_LIBRARY ${SQLITE3_LIBRARIES})
endif (NOT SQLITE3_FOUND)


mark_as_advanced(SQLITE3_INCLUDE_DIRS SQLITE3_LIBRARIES)

