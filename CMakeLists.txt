cmake_minimum_required(VERSION 2.8.7)

set( CMAKE_DEBUG_POSTFIX d )
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin )
set( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin )

############################################
# Project name
############################################
project( DICOMViewer )
############################################

############################################
# Version declaration
# Target file will be named with version suffix
############################################
set( MAJOR_VERSION		 3 )
set( MINOR_VERSION		 1 )
set( PATCH_VERSION		16 )

set( FULL_VERSION		${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION} )

set( TARGET_NAME		${PROJECT_NAME}-${FULL_VERSION} )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/misc/${PROJECT_NAME}.bat.in ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}.bat @ONLY )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/src/doc/Doxyfile.in ${CMAKE_CURRENT_SOURCE_DIR}/src/doc/Doxyfile @ONLY )
############################################


############################################
# Setup the visual studio project properly
############################################
set( VSXPROJ_USER	${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.vcxproj.user )
if( NOT EXISTS ${VSXPROJ_USER} )
	string( REGEX REPLACE "/" "\\\\" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
	configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/misc/DICOMViewer.vcxproj.user.in ${VSXPROJ_USER} @ONLY )
endif()
############################################


############################################
# Find dependencies and add them later to the linker (see below)
############################################

# Qt

set( QT_USE_QTMAIN		TRUE )
set( QT_USE_QTOPENGL	TRUE )
set( QT_USE_QTXML		TRUE )
find_package( Qt4 REQUIRED )
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )

# GLEW

include( $ENV{MEDITEC_LIBS}/GLEW/1.7.0/configure.cmake )

# Carna

include( $ENV{MEDITEC_LIBS}/Carna/2.5.0/configure.cmake )

# CRA

include( $ENV{MEDITEC_LIBS}/CRA/1.15.0/configure.cmake )

# Project

include_directories(${CMAKE_PROJECT_DIR}src)
set( QOBJECT_HEADERS
		src/BinaryDumpExportDialog.h
		src/BinaryDumpImportDialog.h
		src/CarnaModelFactory.h
		src/Component.h
		src/ComponentEmbeddable.h
		src/ComponentLauncher.h
		src/EmbedArea.h
		src/EmbedAreaArray.h
		src/Embeddable.h
		src/FileChooser.h
		src/Gulsun.h
		src/GulsunComponent.h
		src/GulsunSegmentation.h
		src/Histogram.h
		src/HistogramController.h
		src/HistogramView.h
		src/ICP.h
		src/Importer.h
		src/IntegerFormatChooser.h
		src/LocalizerController.h
		src/LocalizerProvider.h
		src/MainWindow.h
		src/MaskingDialog.h
		src/ModelInfo.h
		src/MPR.h
		src/NotificationsProvider.h
		src/Object3DEditorDetails.h
		src/ObjectsController.h
		src/ObjectsList.h
		src/ObjectsListItem.h
		src/ObjectsView.h
		src/PointCloud.h
		src/PointCloud3D.h
		src/PointCloudChooser.h
		src/PointCloudComposer.h
		src/PointCloudComposerSlot.h
		src/PointCloudsController.h
		src/PointCloudsProvider.h
		src/Pointer3D.h
		src/Pointer3DCalibration.h
		src/Pointer3DEditor.h
		src/PointRecorder.h
		src/PointRecorderView.h
		src/RegistrationController.h
		src/RegistrationProvider.h
		src/Server.h
		src/SlicePlane.h
		src/SurfaceExtractionDialog.h
		src/ToolCalibrator.h
		src/ToolChooser.h
		src/ToolEditor.h
		src/ToolSetEditor.h
		src/ViewWindow.h
		src/VolumeController.h
		src/VolumeNormalizer.h
		src/VolumeView.h
		src/VolumeViewCameraController.h
		src/WindowingController.h
)
set( HEADERS
		${QOBJECT_HEADERS}
		src/BinaryDumpProcessor.h
		src/BinaryVolumeMask.h
		src/CarnaContext.h
		src/CarnaContextClient.h
		src/CarnaContextProvider.h
		src/ClippedVolumeMask.h
		src/Components.h
		src/ComponentsClient.h
		src/ComponentsProvider.h
		src/DataSize.h
		src/Differential.h
		src/Dijkstra.h
		src/EmbeddablePlacer.h
		src/EmbedManager.h
		src/Exporter.h
		src/ExportProcessor.h
		src/FlowLayout.h
		src/glew.h
		src/GulsunRadiusStore.h
		src/ImportProcessor.h
		src/LeafFinder.h
		src/Localizer.h
		src/LocalizerClient.h
		src/LocalizerComponent.h
		src/Medialness.h
		src/MedialnessGraph.h
		src/MultiscaleDifferential.h
		src/NormalizedEdgeResponse.h
		src/Notifications.h
		src/NotificationsClient.h
		src/Object3DEditor.h
		src/Object3DEditorFactory.h
		src/ObjectsComponent.h
		src/OptimizedVolumeDecorator.h
		src/Point3DEditor.h
		src/PointCloud3DEditor.h
		src/PointClouds.h
		src/PointCloudsClient.h
		src/PointCloudsComponent.h
		src/PointToPointRegistration.h
		src/Registration.h
		src/RegistrationArguments.h
		src/RegistrationClient.h
		src/RegistrationComponent.h
		src/RegistrationOperation.h
		src/RegistredComponent.h
		src/ScalarField3ui.h
		src/Segmentation.h
		src/SuccessiveMedialness.h
		src/SurfaceExtraction.h
		src/Validation.h
		src/WindowingComponent.h
	)
set( SRC
		src/BinaryDumpExportDialog.cpp
		src/BinaryDumpImportDialog.cpp
		src/BinaryDumpProcessor.cpp
		src/CarnaContextProvider.cpp
		src/CarnaModelFactory.cpp
		src/ClippedVolumeMask.cpp
		src/Component.cpp
		src/ComponentEmbeddable.cpp
		src/ComponentLauncher.cpp
		src/ComponentsProvider.cpp
		src/Differential.cpp
		src/EmbedArea.cpp
		src/EmbedAreaArray.cpp
		src/Embeddable.cpp
		src/EmbeddablePlacer.cpp
		src/EmbedManager.cpp
		src/Exporter.cpp
		src/FileChooser.cpp
		src/FlowLayout.cpp
		src/Gulsun.cpp
		src/GulsunComponent.cpp
		src/GulsunRadiusStore.cpp
		src/GulsunSegmentation.cpp
		src/Histogram.cpp
		src/HistogramController.cpp
		src/HistogramView.cpp
		src/ICP.cpp
		src/Importer.cpp
		src/IntegerFormatChooser.cpp
		src/LocalizerComponent.cpp
		src/LocalizerController.cpp
		src/main.cpp
		src/MainWindow.cpp
		src/MaskingDialog.cpp
		src/Medialness.cpp
		src/MedialnessGraph.cpp
		src/ModelInfo.cpp
		src/MPR.cpp
		src/MultiscaleDifferential.cpp
		src/NormalizedEdgeResponse.cpp
		src/Object3DEditor.cpp
		src/Object3DEditorDetails.cpp
		src/Object3DEditorFactory.cpp
		src/ObjectsComponent.cpp
		src/ObjectsController.cpp
		src/ObjectsList.cpp
		src/ObjectsListItem.cpp
		src/ObjectsView.cpp
		src/OptimizedVolumeDecorator.cpp
		src/Point3DEditor.cpp
		src/PointCloud.cpp
		src/PointCloud3D.cpp
		src/PointCloud3DEditor.cpp
		src/PointCloudChooser.cpp
		src/PointCloudComposer.cpp
		src/PointCloudComposerSlot.cpp
		src/PointCloudsComponent.cpp
		src/PointCloudsController.cpp
		src/Pointer3D.cpp
		src/Pointer3DCalibration.cpp
		src/Pointer3DEditor.cpp
		src/PointRecorder.cpp
		src/PointRecorderView.cpp
		src/PointToPointRegistration.cpp
		src/RegistrationComponent.cpp
		src/RegistrationController.cpp
		src/RegistrationOperation.cpp
		src/Segmentation.cpp
		src/Server.cpp
		src/SlicePlane.cpp
		src/SuccessiveMedialness.cpp
		src/SurfaceExtraction.cpp
		src/SurfaceExtractionDialog.cpp
		src/ToolCalibrator.cpp
		src/ToolChooser.cpp
		src/ToolEditor.cpp
		src/ToolSetEditor.cpp
		src/Validation.cpp
		src/ViewWindow.cpp
		src/VolumeController.cpp
		src/VolumeNormalizer.cpp
		src/VolumeView.cpp
        src/VolumeViewCameraController.cpp
		src/WindowingComponent.cpp
		src/WindowingController.cpp
	)
set( FORMS
		""
	)
set( RESOURCES
		src/ComponentIcons.qrc
		src/shaders.qrc
		src/Icons.qrc
	)
	
QT4_WRAP_CPP( HEADERS_MOC ${QOBJECT_HEADERS} )
QT4_WRAP_UI( FORMS_HEADERS ${FORMS} )
QT4_ADD_RESOURCES( RESOURCES_RCC ${RESOURCES} )
	
############################################
# Exclude all source files from build,
# since we are using unity build.
############################################
set_source_files_properties(	${SRC}
								${HEADERS_MOC}
								${FORMS_HEADERS}
	PROPERTIES
	HEADER_FILE_ONLY TRUE )
############################################
	
############################################
# Write unity build file
############################################
set( UNITY_BUILD_FILE	${CMAKE_CURRENT_BINARY_DIR}/unity_build.cpp )

file( REMOVE	${UNITY_BUILD_FILE} )
file( WRITE		${UNITY_BUILD_FILE} "// This file is automatically generated by CMake.\n\n" )
file( APPEND	${UNITY_BUILD_FILE}	"#include \"${CMAKE_CURRENT_SOURCE_DIR}/src/glew.h\"\n" )

foreach( SOURCE_FILE ${SRC} )
	file( APPEND	${UNITY_BUILD_FILE}	"#include \"${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}\"\n" )
endforeach( SOURCE_FILE )

foreach( QT_GENERATED_SOURCE_FILE ${HEADERS_MOC} ${FORMS_HEADERS} )
	file( APPEND	${UNITY_BUILD_FILE}	"#include \"${QT_GENERATED_SOURCE_FILE}\"\n" )
endforeach( QT_GENERATED_SOURCE_FILE )
############################################

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_executable( ${TARGET_NAME} WIN32
			${SRC}
			${UNITY_BUILD_FILE}
			${HEADERS}
			${HEADERS_MOC}
			${FORMS_HEADERS}
			${RESOURCES_RCC}
		)

add_definitions( -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS )

if( MSVC )
   add_definitions( /arch:SSE2 )
endif( MSVC )


############################################
# Add dependencies to the linker
############################################

target_link_libraries( ${TARGET_NAME}
			opengl32
			glu32
			${GLEW_LIBLIST}
			${QT_LIBRARIES}
			${TRTK_LIBLIST}
			${CARNA_LIBLIST}
			${CRA_LIBLIST}
		)






#vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
### NO CHANGES EVER NECESSARY BELOW THIS POINT!!
##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
#vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

install(	TARGETS	${TARGET_NAME}
		RUNTIME	DESTINATION	${CMAKE_CURRENT_SOURCE_DIR}/bin/debug	CONFIGURATIONS Debug
		)

install(	TARGETS	${TARGET_NAME}
		RUNTIME	DESTINATION	${CMAKE_CURRENT_SOURCE_DIR}/bin/release	CONFIGURATIONS Release
		)
