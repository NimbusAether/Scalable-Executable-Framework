cmake_minimum_required(VERSION 3.8)

project(elementAPI)

# 设置该工程采用C++ 17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_EXTENSIONS OFF)

message("lib element source cmake:${CMAKE_CURRENT_LIST_DIR}")

include_directories(${CMAKE_CURRENT_LIST_DIR}/../Element)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Common/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Config)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../log/include)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../../sys/ThirdParty.win/include)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	foreach(path ${DEBUG_LIBRARY_PATH})
	link_directories(${path})
	endforeach()
else()
	foreach(path ${RELEASE_LIBRARY_PATH})
	link_directories(${path})
	endforeach()
endif()
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/../Element SOURCE_FILE)
if(DEFINED VLD_DEBUG)
	include_directories(${VLD_INCLUDE_PATH})
	link_directories(${VLD_LIB_PATH})
endif()
set(ALL_SOURCE_FILES 
	${SOURCE_FILE}
	)
message("lib element source files:${ALL_SOURCE_FILES}")	
	
set(OUT_INCLUDE_FILES ${CMAKE_CURRENT_LIST_DIR}/Element.h ${CMAKE_CURRENT_LIST_DIR}/ElementFactory.h 
	${CMAKE_CURRENT_LIST_DIR}/ElementWithConfig.h ${CMAKE_CURRENT_LIST_DIR}/Group.h ${CMAKE_CURRENT_LIST_DIR}/../Common/include/Mode.h
	${CMAKE_CURRENT_LIST_DIR}/../Config/ConfigFactory.h)
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../../sys/lib")
message("lib element out path:${LIBRARY_OUTPUT_PATH}")	

add_library(${PROJECT_NAME} SHARED ${ALL_SOURCE_FILES})
add_dependencies(${PROJECT_NAME} configAPI)
target_link_libraries(${PROJECT_NAME}  configAPI SerializeConfigAPI commonAPI)
if(DEFINED VLD_DEBUG)
	target_link_libraries(${PROJECT_NAME} vld)
endif()
install(TARGETS ${PROJECT_NAME}
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/*.h DESTINATION include)