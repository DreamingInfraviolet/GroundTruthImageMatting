#link_directories(${GTKMM_LIBRARY_DIRS})
#include_directories(${GTKMM_INCLUDE_DIRS})

set(MAIN_SOURCES
    "window.cpp"
    "vertex.cpp"
    "source.cpp"
    "shaderprogram.cpp"
    "openglbox.cpp"
    "io.cpp"
    "imageshader.cpp"
    "image.cpp"
    "colourshader.cpp"
    "actionclass.cpp"
    "camera.cpp"
    "edsstreamcontainer.cpp")

set(MAIN_HEADERS
	"window.h"
	"vertex.h"
	"Vec.h"
	"shaderprogram.h"
	"propertymap.h"
	"openglbox.h"
	"io.h"
	"imageshader.h"
	"image.h"
	"colourshader.h"
	"actionclass.h"
	"camera.h"
	"rawrgbeds.h"
	"rawrgbchar.h"
	"edsstreamcontainer.h")

set(GROUND_TRUTH_SOURCES "groundtruthsource.cpp" "groundtruth.cpp" "io.cpp")
set(GROUND_TRUTH_HEADERS "image.h" "camera.h" "image.h" "rawrgbchar.h" "groundtruth.h")


set(MOCS window.h openglbox.h)
set(UIS mainui.ui)

#prepare QT
# Find includes in corresponding build directories
set(CMAKE_PREFIX_PATH C:/Qt/5.4/msvc2013_opengl ${CMAKE_PREFIX_PATH})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)  

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

# Make sure the compiler can find the pre-processed files from qt5_wrap_ui
include_directories("${CMAKE_BINARY_DIR}")

add_executable(
				CameraControl # name of the executable
					 ${MAIN_SOURCES}
					 ${GENERATED_SOURCES}
			         ${MAIN_HEADERS}
				     ${MOCS}
			    )

add_executable(
				GroundTruth # name of the executable
					 ${GROUND_TRUTH_SOURCES}
			         ${GROUND_TRUTH_HEADERS}
			    )

#Add OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(CameraControl ${OPENGL_LIBRARIES})
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

#Add sfml
#set(SFML_ROOT "${CMAKE_SOURCE_DIR}/third_party/sfml_git/")
#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/third_party/sfml_git/cmake/Modules" ${CMAKE_MODULE_PATH})
#find_package(SFML 2 REQUIRED system window graphics)
#include_directories(${SFML_INCLUDE_DIR})
#target_link_libraries(CameraControl ${SFML_LIBRARIES})

#Add SDL
target_link_libraries(CameraControl "${PROJECT_SOURCE_DIR}/third_party/sdl2/lib/x86/SDL2main.lib"
									"${PROJECT_SOURCE_DIR}/third_party/sdl2/lib/x86/SDL2.lib")
include_directories("${PROJECT_SOURCE_DIR}/third_party/sdl2/include")

#Add canon API
target_link_libraries(CameraControl "${PROJECT_SOURCE_DIR}/third_party/canon/EDSDK/Library/EDSDK.lib")
include_directories("${PROJECT_SOURCE_DIR}/third_party/canon/EDSDK/Header")

#Add OpenCV
include_directories("${PROJECT_SOURCE_DIR}/third_party/opencv/build/include")
target_link_libraries(CameraControl debug 	  "${PROJECT_SOURCE_DIR}/third_party/opencv/build/x86/vc12/lib/opencv_ts300d.lib"
											 "${PROJECT_SOURCE_DIR}/third_party/opencv/build/x86/vc12/lib/opencv_world300d.lib")
target_link_libraries(CameraControl optimized "${PROJECT_SOURCE_DIR}/third_party/opencv/build/x86/vc12/lib/opencv_ts300.lib"
											 "${PROJECT_SOURCE_DIR}/third_party/opencv/build/x86/vc12/lib/opencv_world300.lib")

target_link_libraries(GroundTruth debug 	  "${PROJECT_SOURCE_DIR}/third_party/opencv/build/x86/vc12/lib/opencv_ts300d.lib"
											 "${PROJECT_SOURCE_DIR}/third_party/opencv/build/x86/vc12/lib/opencv_world300d.lib")
target_link_libraries(GroundTruth optimized "${PROJECT_SOURCE_DIR}/third_party/opencv/build/x86/vc12/lib/opencv_ts300.lib"
											 "${PROJECT_SOURCE_DIR}/third_party/opencv/build/x86/vc12/lib/opencv_world300.lib")

#Set 32/64 bit properties.
#set_target_properties(CameraControl PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
#set_target_properties(GroundTruth PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")

# Link QT
target_link_libraries(CameraControl 
					${Qt5Core_LIBRARIES}
					${Qt5Gui_LIBRARIES}
					${Qt5OpenGL_LIBRARIES}
					${Qt5Widgets_LIBRARIES})