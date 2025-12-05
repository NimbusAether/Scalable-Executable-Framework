# CMakeList.txt: Exec 的 CMake 项目, 在此处包括源代码并定义(项目特定的逻辑)

cmake_minimum_required (VERSION 3.8)

project ("Exec")

# 设置该工程采用C++ 17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置包含目录和链接库目录
include_directories(${CMAKE_CURRENT_LIST_DIR}/../sys/ThirdParty.win/include)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	foreach(path ${DEBUG_LIBRARY_PATH})
	link_directories(${path})
	endforeach()
else()
	foreach(path ${RELEASE_LIBRARY_PATH})
	link_directories(${path})
	endforeach()
endif()

# set src files path
include_directories(${CMAKE_CURRENT_LIST_DIR}/Kernel)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Kernel/LocalhostKernel)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Kernel/MultiNodesKernel)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Config)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Element)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Common)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Common/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Udm)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Udm/IPCUdm)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Udm/NetUdm)
include_directories(${CMAKE_CURRENT_LIST_DIR}/UdmService)
include_directories(${CMAKE_CURRENT_LIST_DIR}/UdmService/udm)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Elements)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Elements/Element1)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Elements/Element2)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Factory)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Exec)
include_directories(${CMAKE_CURRENT_LIST_DIR}/log/include)

message(WARNING ${CMAKE_CURRENT_LIST_DIR})
if(DEFINED VLD_DEBUG)
	include_directories(${VLD_INCLUDE_PATH})
	link_directories(${VLD_LIB_PATH})
endif()
# 将源代码添加到此项目的可执行文件
add_executable (Exec "main.cpp")

add_dependencies(${PROJECT_NAME} elementAPI)
add_dependencies(${PROJECT_NAME} commonAPI)
add_dependencies(${PROJECT_NAME} udmServiceAPI)
add_dependencies(${PROJECT_NAME} configAPI)
add_dependencies(${PROJECT_NAME} kernelAPI)
add_dependencies(${PROJECT_NAME} udmAPI)
add_dependencies(${PROJECT_NAME} execAPI)
add_dependencies(${PROJECT_NAME} LogAPI)

target_link_libraries(Exec execAPI 
	udmAPI kernelAPI configAPI udmServiceAPI commonAPI elementAPI LogAPI SerializeConfigAPI)
if(DEFINED VLD_DEBUG)
	target_link_libraries(Exec vld)
endif()
# TODO: 如有需要, 请添加测试并安装目标

# 设置生成的可执行文件路径
# get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../sys/lib")
install(TARGETS Exec DESTINATION ${EXECUTABLE_OUTPUT_PATH})

# file(COPY ../bin/Debug/Exec.exe DESTINATION ../../sys/bin/)