cmake_minimum_required(VERSION 3.8)

project(GPTE_SDK_API)

# 设置该工程采用C++ 17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


message("lib udm source cmake:${CMAKE_CURRENT_LIST_DIR}")
# 设置包含目录和链接库目录
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../sys/ThirdParty.win/include)
foreach(path ${COMMON_LIBRARY_PATH})
	link_directories(${path})
	message("lib exec add third path:${path}")
endforeach()

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

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/ BASE_SOURCE)

set(ALL_SOURCE_FILES 
	${BASE_SOURCE}
	)
message("GPTE_SDK_API exec source files:${ALL_SOURCE_FILES}")	
	
set(OUT_INCLUDE_FILES ${CMAKE_CURRENT_LIST_DIR}/UdmSdk.h)	
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../../sys/lib")
message("lib  out path:${LIBRARY_OUTPUT_PATH}")


add_library(${PROJECT_NAME} SHARED ${ALL_SOURCE_FILES})
add_dependencies(${PROJECT_NAME} elementAPI)
add_dependencies(${PROJECT_NAME} commonAPI)
add_dependencies(${PROJECT_NAME} udmServiceAPI)
add_dependencies(${PROJECT_NAME} configAPI)
add_dependencies(${PROJECT_NAME} kernelAPI)
add_dependencies(${PROJECT_NAME} udmAPI)
target_link_libraries(${PROJECT_NAME} libzmq-v143-mt-gd-4_3_5.lib jsoncpp_static.lib udmAPI kernelAPI configAPI udmServiceAPI commonAPI elementAPI)

if(BUILD_STATIC_LIBRARY)
	add_library(${PROJECT_NAME}_static STATIC ${ALL_SOURCE_FILES})
	add_dependencies(${PROJECT_NAME}_static elementAPI)
	add_dependencies(${PROJECT_NAME}_static commonAPI)
	add_dependencies(${PROJECT_NAME}_static udmServiceAPI)
	add_dependencies(${PROJECT_NAME}_static configAPI)
	add_dependencies(${PROJECT_NAME}_static kernelAPI)
	add_dependencies(${PROJECT_NAME}_static udmAPI)
	target_link_libraries(${PROJECT_NAME}_static tinyxml2D.lib libzmq-v143-mt-gd-4_3_5.lib jsoncpp_static.lib udmAPI kernelAPI configAPI udmServiceAPI commonAPI elementAPI)
	install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_static 
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
elseif()
	install(TARGETS ${PROJECT_NAME}
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
endif()
	







install(FILES ${CMAKE_CURRENT_LIST_DIR}/*.h DESTINATION include)