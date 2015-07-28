#link_directories(${GTKMM_LIBRARY_DIRS})
#include_directories(${GTKMM_INCLUDE_DIRS})


file(GLOB SOURCES
    "*.cpp"
)

file(GLOB HEADERS
	".h")

set(HEADERS
			camera.h
			colour.h
			compatabilitylayer.h
			event.h
			image.h
			io.h
			propertymap.h
			vertex.h
			shaderprogram.h
			imageshader.h
			)

set(MOCS window.h openglbox.h)
set(UIS mainui.ui)

#prepare QT
# Find includes in corresponding build directories
set(CMAKE_PREFIX_PATH C:/Qt/5.4/msvc2013_opengl)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOUIC ON)  

find_package(Qt5 COMPONENTS Core Widgets Gui OpenGL REQUIRED)

#qt5_wrap_ui(GENERATED_SOURCES ${UIS})



include_directories(${Qt5Core_INCLUDE_DIRS}
					${Qt5Gui_INCLUDE_DIRS}
					${Qt5OpenGL_INCLUDE_DIRS}
					${Qt5Widgets_INCLUDE_DIRS})

  add_definitions(
  					${Qt5Core_DEFINITIONS}
					${Qt5Gui_DEFINITIONS}
					${Qt5OpenGL_DEFINITIONS}
					${Qt5Widgets_DEFINITIONS}
  )

source_group("Generated Sources - Do Not Edit" FILES ${GENERATED_SOURCES})

# Make sure the compiler can find the pre-processed files from qt5_wrap_ui
include_directories("${CMAKE_BINARY_DIR}")

add_executable(
				CameraControl # name of the executable
				${UIS}
					 ${SOURCES}
					 ${GENERATED_SOURCES}
			         ${HEADERS}
				     ${MOCS}
			    )

#Add OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(CameraControl ${OPENGL_LIBRARIES})
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )


#Add canon API
target_link_libraries(CameraControl "${PROJECT_SOURCE_DIR}/third_party/canon/EDSDK/Library/EDSDK.lib")
include_directories("${PROJECT_SOURCE_DIR}/third_party/canon/EDSDK/Header")

#Add OpenCV
include_directories("${PROJECT_SOURCE_DIR}/third_party/opencv/build/include")
target_link_libraries(CameraControl debug 	  "${PROJECT_SOURCE_DIR}/third_party/opencv/sources/build/lib/Debug/opencv_ts300d.lib"
											  "${PROJECT_SOURCE_DIR}/third_party/opencv/sources/build/lib/Debug/opencv_world300d.lib")
target_link_libraries(CameraControl optimized "${PROJECT_SOURCE_DIR}/third_party/opencv/sources/build/lib/Release/opencv_ts300.lib"
											  "${PROJECT_SOURCE_DIR}/third_party/opencv/sources/build/lib/Release/opencv_world300.lib")

# Link QT
target_link_libraries(CameraControl 
					${Qt5Core_LIBRARIES}
					${Qt5Gui_LIBRARIES}
					${Qt5OpenGL_LIBRARIES}
					${Qt5Widgets_LIBRARIES})