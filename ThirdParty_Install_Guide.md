# 第三方库安装指南

## 声明
- 该文档暂未使用单一包管理工具或脚本进行测试及编写，未来将会更新
- 也欢迎使用者上传自己的安装指南


## 依赖
- CMake
- vcpkg

## 安装指南
### vcpkg
1. 下载vcpkg：`git clone https://github.com/microsoft/vcpkg.git`
2. `cd vcpkg & .\bootstrap-vcpkg.bat`
3. `.\vcpkg.exe integrate install`

### libevent
1. 安装Win64OpenSSL-3_6_0.exe，安装时选择OpenSSL库位于其bin文件夹下
2. 配置环境变量`OPENSSL_ROOT_DIR=C:\Program Files\OpenSSL-Win64`
3. 配置环境变量`OPENSSL_INCLUDE_DIR=%OPENSSL_ROOT_DIR%\include`
4. 配置环境变量`OPENSSL_LIBRARIES=%OPENSSL_ROOT_DIR%\lib`
5. 配置环境变量`PATH=%OPENSSL_ROOT_DIR%\bin`
6. 下载libevent-2.1.12-stable.zip，解压
7. `cd libevent-2.1.12-stable`
8. 修改CMakeLists第22行cmake版本到3.15
9. `md build & cd build`
10. `cmake ..`
11. 打开libevent.sln，选择Release，编译
11. 复制`libevent-2.1.12-stable\build\bin\Release`中内容至`Scalable-Executable-Framework\sys\ThirdParty.win\bin\libevent`
12. 复制`libevent-2.1.12-stable\build\lib\Release`中内容至`Scalable-Executable-Framework\sys\ThirdParty.win\lib\libevent`

### libuv
1. 下载libuv-1.51.0.zip，解压
2. `cd libuv-1.51.0`
3. `md build & cd build`
4. `cmake ..`
5. 打开libuv.sln，选择Release，编译
6. 复制`libuv-1.51.0\build\Release`中的exe和dll内容至`Scalable-Executable-Framework\sys\ThirdParty.win\bin\libuv`
5. 复制`libuv-1.51.0\build\Release`中的lib内容至`Scalable-Executable-Framework\sys\ThirdParty.win\lib\libuv`

### lwip
1. 下载lwip-2.2.1.zip，解压
2. `cd lwip-2.2.1\contrib\examples\example_app`
3. 复制lwipcfg.h.example文件为lwipcfg.h
4. 打开`lwip-2.2.1\contrib\ports\win32\msvc`下lwip_fuzz.sln，选择Release，编译lwip项目
5. 复制`lwip-2.2.1\contrib\ports\win32\msvc\x64\Release fuzz\lwIP\lwIP.lib`至`Scalable-Executable-Framework\sys\ThirdParty.win\lib\lwip`

### protobuf
1. `.\vcpkg.exe install protobuf:x64-windows`
2. 复制`vcpkg\installed\x64-windows\bin`中libprotoc和libprotobuf的dll至`Scalable-Executable-Framework\sys\ThirdParty.win\bin\google\protobuf`
3. 复制`vcpkg\installed\x64-windows\bin`中libprotoc和libprotobuf的lib至`Scalable-Executable-Framework\sys\ThirdParty.win\lib\google\protobuf`

### jsoncpp
1. `.\vcpkg.exe install jsoncpp`
2. 复制`vcpkg\installed\x64-windows\bin`中jsoncpp.dll至`Scalable-Executable-Framework\sys\ThirdParty.win\bin\jsoncpp`
3. 复制`vcpkg\installed\x64-windows\bin`中jsoncpp.lib至`Scalable-Executable-Framework\sys\ThirdParty.win\lib\jsoncpp`

### libpqxx
1. 安装postgreSQL
2. 下载libpqxx: `git clone https://github.com/jtv/libpqxx.git`
3. `cd libpqxx & md build`
4. 注意其中D:\Apps\PostgreSQL\18要替换成你安装postgreSQL的路径：
`cmake -D CMAKE_BUILD_TYPE=Release -DSKIP_BUILD_TEST=on -DBUILD_SHARED_LIBS=ON -DPostgreSQL_ROOT=D:\Apps\PostgreSQL\18 ..`
5. 打开libpqxx.sln，编译pqxx
6. 复制`libpqxx-7.10.1\build\src\Release`中pqxx.dll至`Scalable-Executable-Framework\sys\ThirdParty.win\bin\libpqxx`
7. 复制`libpqxx-7.10.1\build\src\Release`中pqxx.lib至`Scalable-Executable-Framework\sys\ThirdParty.win\lib\libpqxx`

### zmqpp
1. 下载zeromq-4.3.5.zip，解压
2. `cd zeromq-4.3.5`
3. `md build & cd build`
4. `cmake -DCMAKE_POLICY_VERSION_MINIMUM='3.5' ..`
5. 打开ZeroMQ.sln，选择Release，编译
6. 复制`zeromq-4.3.5\build\bin\Release\libzmq-v143-mt-4_3_5.dll`至`Scalable-Executable-Framework\sys\ThirdParty.win\bin\libzmq`
7. 复制`zeromq-4.3.5\build\lib\Release中libzmq`相关lib至`Scalable-Executable-Framework\sys\ThirdParty.win\lib\libzmq`

### c-logger
1. 下载c-logger-0.4.1.zip，解压
2. `cd c-logger-0.4.1`
3. `md build & cd build`
4. `cmake ..`
5. 打开logger.sln，选择Release，编译
6. 复制`c-logger-0.4.1\build\Release\logger.dll`至`Scalable-Executable-Framework\sys\ThirdParty.win\bin\c-logger`
7. 复制`c-logger-0.4.1\build\Release\logger.lib`至`Scalable-Executable-Framework\sys\ThirdParty.win\lib\c-logger`

### log4z
1. 下载log4z-3.4.0.zip，解压
2. `cd log4z-3.4.0`
3. `cd vc2015`
4. 打开project.sln，选择Release，编译
5. 复制`log4z-3.4.0\lib`下log4z_win64相关lib至`Scalable-Executable-Framework\sys\ThirdParty.win\lib\log4z`

### vld
1. 安装vld-2.5.1-setup.exe
2. 打开安装路径
3. 复制`Visual Leak Detector\bin\Win64`下内容至`Scalable-Executable-Framework\sys\ThirdParty.win\bin\vld`
4. 复制`Visual Leak Detector\lib\Win64`下内容至`Scalable-Executable-Framework\sys\ThirdParty.win\lib\vld`
