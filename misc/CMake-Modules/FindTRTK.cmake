############################################
# Locates TRTK library.
# ------------------------------------------
# If TRTK headers and library files are not
# found in the standard CMake locations,
# then %MEDITEC_LIBS%/TRTK/<version> is
# checked. In this, letter case it is
# necessary that the version is matched
# exactly. Consider using symbolic links
# when you have a compatible version.
#-------------------------------------------
# The following variables are set:
#	TRTK_INCLUDE_DIR	points to headers
#	TRTK_LIBRARIES		points to lib files
#	TRTK_FOUND			indicates success
#-------------------------------------------
# Leonid Kostrykin, 12.12.2014
############################################

find_path(	TRTK_INCLUDE_DIR
			TRTK/Version.hpp
			PATHS ENV MEDITEC_LIBS
			PATH_SUFFIXES "TRTK/${TRTK_FIND_VERSION}/include"
			DOC "TRTK headers" )

find_library(	TRTK_LIBRARY_RELEASE
				NAMES "TRTK-${TRTK_FIND_VERSION}" TRTK
				PATHS ENV MEDITEC_LIBS
				PATH_SUFFIXES "TRTK/${TRTK_FIND_VERSION}/bin"
				DOC "TRTK library (Release)" )

find_library(	TRTK_LIBRARY_DEBUG
				NAMES	"TRTK-${TRTK_FIND_VERSION}d"
						"TRTK-${TRTK_FIND_VERSION}-debug"
						TRTKd
						TRTK-debug
				PATHS ENV MEDITEC_LIBS
				PATH_SUFFIXES "TRTK/${TRTK_FIND_VERSION}/bin"
				DOC "TRTK library (Debug)" )

# handles REQUIRED, QUIET and version-related
# arguments and also sets the _FOUND variable
find_package_handle_standard_args( TRTK
	REQUIRED_VARS TRTK_INCLUDE_DIR TRTK_LIBRARY_RELEASE TRTK_LIBRARY_DEBUG )

if( DEFINED TRTK_LIBRARY_RELEASE AND DEFINED TRTK_LIBRARY_DEBUG )
	set( TRTK_LIBRARIES
			optimized	${TRTK_LIBRARY_RELEASE}
			debug		${TRTK_LIBRARY_DEBUG} )
endif()