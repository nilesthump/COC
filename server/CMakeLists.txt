cmake_minimum_required(VERSION 3.10)
project(WebSocketServer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置项目根目录和CMake模块路径
set(PROJECT_ROOT "${CMAKE_SOURCE_DIR}/../")
set(CMAKE_MODULE_PATH ${PROJECT_ROOT}/cocos2d/cmake/Modules/)

# 包含Cocos构建设置
include(CocosBuildSet)

# 构建external依赖
add_subdirectory(${PROJECT_ROOT}/cocos2d/external ${CMAKE_BINARY_DIR}/external)

# 添加可执行文件和源文件
add_executable(WebSocketServer 
    WebSocketServer.cpp
)

# 设置输出目录
set_target_properties(WebSocketServer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

# 链接依赖库
target_link_libraries(WebSocketServer PRIVATE 
    ext_websockets 
    ext_sqlite3 
    ws2_32.lib 
    userenv.lib 
    advapi32.lib 
    crypt32.lib
)

# 复制必要的依赖文件
if(WIN32)
    # 在Windows平台上复制必要的DLL文件
    set(EXTERNAL_DIR "${PROJECT_ROOT}/cocos2d/external")
    
    # 复制websockets.dll
    set(WEBSOCKETS_DLL_PATH "${EXTERNAL_DIR}/websockets/prebuilt/win32/websockets.dll")
    if(EXISTS ${WEBSOCKETS_DLL_PATH})
        add_custom_command(TARGET WebSocketServer POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${WEBSOCKETS_DLL_PATH}
            $<TARGET_FILE_DIR:WebSocketServer>
        )
    endif()
    
    # 复制openssl库
    set(OPENSSL_DIR "${EXTERNAL_DIR}/openssl/prebuilt/win32")
    file(GLOB OPENSSL_DLLS "${OPENSSL_DIR}/*.dll")
    foreach(DLL ${OPENSSL_DLLS})
        if(EXISTS ${DLL})
            add_custom_command(TARGET WebSocketServer POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${DLL}
                $<TARGET_FILE_DIR:WebSocketServer>
            )
        endif()
    endforeach()
    
    # 复制libuv库
    set(UV_DLL_PATH "${EXTERNAL_DIR}/uv/prebuilt/win32/uv.dll")
    if(EXISTS ${UV_DLL_PATH})
        add_custom_command(TARGET WebSocketServer POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${UV_DLL_PATH}
            $<TARGET_FILE_DIR:WebSocketServer>
        )
    endif()
    
    # 如果使用了cocos_copy_target_dll，保留它以确保其他依赖被复制
    cocos_copy_target_dll(WebSocketServer)
endif()
