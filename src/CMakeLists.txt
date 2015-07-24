#link_directories(${GTKMM_LIBRARY_DIRS})
#include_directories(${GTKMM_INCLUDE_DIRS})


file(GLOB_RECURSE AllSrcFiles
    "*.h"
    "*.cpp"
)
add_executable(
				CameraControl # name of the executable
			    ${AllSrcFiles}
			    )

include_directories("${PROJECT_SOURCE_DIR}/src")


#Add OpenGL
find_package(OpenGL REQUIRED)
TARGET_LINK_LIBRARIES(CameraControl ${OPENGL_LIBRARIES})
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )


#Add canon API
target_link_libraries(CameraControl "${PROJECT_SOURCE_DIR}/third_party/canon/EDSDK/Library/EDSDK.lib")
include_directories("${PROJECT_SOURCE_DIR}/third_party/canon/EDSDK/Header")

#Add SOIL
target_link_libraries(CameraControl debug "${PROJECT_SOURCE_DIR}/third_party/soil/projects/VC9/Debug/SOIL.lib")
target_link_libraries(CameraControl optimized "${PROJECT_SOURCE_DIR}/third_party/soil/projects/VC9/Release/SOIL.lib")
include_directories("${PROJECT_SOURCE_DIR}/third_party/soil/src")

#Add nana
include_directories("${PROJECT_SOURCE_DIR}/third_party/nana/include/")
target_link_libraries(CameraControl debug "${PROJECT_SOURCE_DIR}/third_party/nana/build/bin/vc2013/nana_Debug_x86.lib")
target_link_libraries(CameraControl optimized "${PROJECT_SOURCE_DIR}/third_party/nana/build/bin/vc2013/nana_Release_x86.lib")




#target_link_libraries(example${GTKMM_LIBRARIES})