cmake_minimum_required(VERSION 3.8)

project(execAPI)

# 设置该工程采用C++ 17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


message("lib udm source cmake:${CMAKE_CURRENT_LIST_DIR}")
# 设置包含目录和链接库目录
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

include_directories(${CMAKE_CURRENT_LIST_DIR}/../Db)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Config)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Common/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Element)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../udm)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../udm/NetUdm)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../udm/IPCUdm)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../udmService)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../udmService/udm)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../kernel/LocalhostKernel)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../kernel/)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../kernel/MultiNodesKernel)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../log/include)
if(DEFINED VLD_DEBUG)
	include_directories(${VLD_INCLUDE_PATH})
	link_directories(${VLD_LIB_PATH})
endif()
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/ BASE_SOURCE)

set(ALL_SOURCE_FILES 
	${BASE_SOURCE}
	)
message("lib exec source files:${ALL_SOURCE_FILES}")	
	
set(OUT_INCLUDE_FILES ${CMAKE_CURRENT_LIST_DIR}/UdmService.h ${CMAKE_CURRENT_LIST_DIR}/UdmServiceFactory.h
	${CMAKE_CURRENT_LIST_DIR}/udm/ZeroMQServiceFactory.h)	
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../../sys/lib")
message("lib udm out path:${LIBRARY_OUTPUT_PATH}")


add_library(${PROJECT_NAME} SHARED ${ALL_SOURCE_FILES})
add_dependencies(${PROJECT_NAME} elementAPI)
add_dependencies(${PROJECT_NAME} commonAPI)
add_dependencies(${PROJECT_NAME} udmServiceAPI)
add_dependencies(${PROJECT_NAME} configAPI)
add_dependencies(${PROJECT_NAME} kernelAPI)
add_dependencies(${PROJECT_NAME} udmAPI)
add_dependencies(${PROJECT_NAME} LogAPI)
target_link_libraries(${PROJECT_NAME}  LogAPI  udmAPI kernelAPI configAPI udmServiceAPI commonAPI elementAPI)
if(DEFINED VLD_DEBUG)
	target_link_libraries(${PROJECT_NAME} vld)
endif()
install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/*.h DESTINATION include)