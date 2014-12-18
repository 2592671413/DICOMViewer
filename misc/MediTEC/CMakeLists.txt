############################################
# MediTEC Compatibility Script
# ------------------------------------------
# The script does nothing unless the
# environmental variable MEDITEC_LIBS is set
#-------------------------------------------
# Leonid Kostrykin, 18.12.2014
############################################

if( DEFINED ENV{MEDITEC_LIBS} )

	# normalize paths so we can compare them
	get_filename_component(MEDITEC_LIBS $ENV{MEDITEC_LIBS} ABSOLUTE)
	set(MEDITEC_LIBS ${MEDITEC_LIBS} PARENT_SCOPE)
	
	# adjust CMAKE_MODULE_PATH so that proper dependency versions are found
	set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" PARENT_SCOPE)
	list(APPEND CMAKE_MODULE_PATH "${MEDITEC_LIBS}/Carna/2.5")
	list(APPEND CMAKE_MODULE_PATH "${MEDITEC_LIBS}/CarnaDICOM/0.1")
	
	# find GLEW headers
	list(APPEND CMAKE_INCLUDE_PATH "$ENV{MEDITEC_LIBS}/GLEW/1.7.0/include")
	find_path(GLEW_INCLUDE_DIR GL/glew.h)
	
	# find GLEW libraries
	list(APPEND CMAKE_LIBRARY_PATH "$ENV{MEDITEC_LIBS}/GLEW/1.7.0/bin")
	find_library(GLEW_LIBRARY NAMES glew32-1.7.0 PATH_SUFFIXES lib64)
	
	# find Eigen headers
	find_path(	EIGEN3_INCLUDE_DIR
				Eigen/Core
				PATHS ENV MEDITEC_LIBS
				PATH_SUFFIXES "Eigen/3.0.5/include"
				DOC "Eigen3 headers")

endif( DEFINED ENV{MEDITEC_LIBS} )

# export CMAKE_MODULE_PATH to parent scope
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)