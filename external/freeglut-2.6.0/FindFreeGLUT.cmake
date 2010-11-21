if(NOT FREEGLUT_PACKAGE_DIR)
	message("ERROR: FREEGLUT_PACKAGE_DIR requires to be set")
endif(NOT FREEGLUT_PACKAGE_DIR)

if(MSVC AND NOT CMAKE_CL_64)
	if(MSVC_VERSION EQUAL 1500)
		set(BINARY_DIRECTORY "win32-vc2008")
	endif(MSVC_VERSION EQUAL 1500)	
	if(MSVC_VERSION EQUAL 1600)
		set(BINARY_DIRECTORY "win32-vc2010")
	endif(MSVC_VERSION EQUAL 1600)
endif(MSVC AND NOT CMAKE_CL_64)

if(MSVC AND CMAKE_CL_64)
	if(MSVC_VERSION EQUAL 1500)
		set(BINARY_DIRECTORY "win64-vc2008")
	endif(MSVC_VERSION EQUAL 1500)	
	if(MSVC_VERSION EQUAL 1600)
		set(BINARY_DIRECTORY "win64-vc2010")
	endif(MSVC_VERSION EQUAL 1600)
endif(MSVC AND CMAKE_CL_64)

set(FREEGLUT_INCLUDE_DIR ${FREEGLUT_PACKAGE_DIR})
set(FREEGLUT_LIBRARY_DIR ${FREEGLUT_PACKAGE_DIR}/lib/${BINARY_DIRECTORY})
set(FREEGLUT_FOUND "YES")

set(FREEGLUT_LIBRARY freeglut_static)

add_definitions(-DFREEGLUT_STATIC)
include_directories(${FREEGLUT_INCLUDE_DIR})
link_directories(${FREEGLUT_LIBRARY_DIR})