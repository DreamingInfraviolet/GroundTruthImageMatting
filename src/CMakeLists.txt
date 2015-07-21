#link_directories(${GTKMM_LIBRARY_DIRS})
#include_directories(${GTKMM_INCLUDE_DIRS})

add_executable(
				CameraControl # name of the executable
			    source.cpp
			    camera.cpp
			    camera.h
			    io.cpp
			    io.h
			    propertymap.cpp
			    propertymap.h
			    )

include_directories("${PROJECT_SOURCE_DIR}/src")


#Add canon API
target_link_libraries(CameraControl "${PROJECT_SOURCE_DIR}/third_party/canon/EDSDK/Library/EDSDK.lib")
include_directories("${PROJECT_SOURCE_DIR}/third_party/canon/EDSDK/Header")

#Generate folders
file(MAKE_DIRECTORY "captured")
file(MAKE_DIRECTORY "tmp")

#target_link_libraries(example${GTKMM_LIBRARIES})