############################################
# Locates Flann library.
# ------------------------------------------
# If Flann headers and library files are not
# found in the standard CMake locations,
# then %MEDITEC_LIBS%/Flann/<version> is
# checked. In this, letter case it is
# necessary that the version is matched
# exactly. Consider using symbolic links
# when you have a compatible version.
#-------------------------------------------
# The following variables are set:
#	FLANN_INCLUDE_DIR	points to headers
#	FLANN_LIBRARIES		points to lib files
#	FLANN_FOUND			indicates success
#-------------------------------------------
# Leonid Kostrykin, 18.12.2014
############################################

find_path(	FLANN_INCLUDE_DIR
			flann/flann.h
			PATHS ENV MEDITEC_LIBS
			PATH_SUFFIXES "flann/${Flann_FIND_VERSION}/include"
			DOC "Flann headers" )

find_library(	FLANN_LIBRARY_RELEASE
				NAMES "flann-${Flann_FIND_VERSION}" flann
				PATHS ENV MEDITEC_LIBS
				PATH_SUFFIXES "flann/${Flann_FIND_VERSION}/bin"
				DOC "Flann library (Release)" )

find_library(	FLANN_LIBRARY_DEBUG
				NAMES	"flann-${Flann_FIND_VERSION}d"
						"flann-${Flann_FIND_VERSION}-debug"
						flannd
						flann-debug
				PATHS ENV MEDITEC_LIBS
				PATH_SUFFIXES "flann/${Flann_FIND_VERSION}/bin"
				DOC "Flann library (Debug)" )

# handles REQUIRED, QUIET and version-related
# arguments and also sets the _FOUND variable
find_package_handle_standard_args( FLANN
	REQUIRED_VARS FLANN_INCLUDE_DIR FLANN_LIBRARY_RELEASE FLANN_LIBRARY_DEBUG )

if( DEFINED FLANN_LIBRARY_RELEASE AND DEFINED FLANN_LIBRARY_DEBUG )
	set( FLANN_LIBRARIES
			optimized	${FLANN_LIBRARY_RELEASE}
			debug		${FLANN_LIBRARY_DEBUG} )
endif()