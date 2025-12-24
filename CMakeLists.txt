cmake_minimum_required(VERSION 3.6)

set(APP_NAME COC)

project(${APP_NAME})

if(XCODE)
    if(NOT DEFINED CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET)
        SET (CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 8.0)
    endif()
endif()

if(NOT DEFINED BUILD_ENGINE_DONE) # to test COC into root project
    set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cocos2d)
    set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

    include(CocosBuildSet)
    # Build external dependencies first to ensure sqlite3 is available
    add_subdirectory(${COCOS2DX_ROOT_PATH}/external ${ENGINE_BINARY_PATH}/external)
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)
endif()

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    )
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

# add cross-platforms source files and header files 
list(APPEND GAME_SOURCE
     Classes/AppDelegate.cpp
     Classes/BattleManager.cpp
     Classes/AttackerData.cpp
     Classes/HelloWorldScene.cpp
     Classes/SecondScene.cpp
     Classes/CharacterUnitCard.cpp
     Classes/BarracksPopup.cpp
     Classes/BomberBehavior.cpp
     Classes/GoblinBehavior.cpp
     Classes/ArcherNavigation.cpp
     Classes/BarbarianNavigation.cpp
     Classes/BomberNavigation.cpp
     Classes/GiantNavigation.cpp
     Classes/GoblinNavigation.cpp
     Classes/UnitNavigation.cpp
     Classes/BattleUnit.cpp
     Classes/BaseBehavior.cpp
     Classes/AttackerNormalBehavior.cpp
     Classes/DefenderData.cpp
     Classes/DefenderNormalBehavior.cpp
     Classes/BattleTestLayer.cpp
     Classes/UnitState.cpp
     Classes/SQLiteTest.cpp
     Classes/SessionManager.cpp
     )
list(APPEND GAME_HEADER
     Classes/AppDelegate.h
     Classes/BattleManager.h
     Classes/AttackerData.h
     Classes/BomberBehaviorData.h
     Classes/HelloWorldScene.h
     Classes/SecondScene.h
     Classes/CharacterUnitCard.h
     Classes/BarracksPopup.h
     Classes/UnitState.h
     Classes/BattleUnit.h
     Classes/UnitFactory.h
     Classes/UnitNavigation.h
     Classes/UnitBehavior.h
     Classes/BomberBehavior.h
     Classes/GoblinBehavior.h
     Classes/ArcherNavigation.h
     Classes/BarbarianNavigation.h
     Classes/BomberNavigation.h
     Classes/GiantNavigation.h
     Classes/GoblinNavigation.h
     Classes/BaseUnitData.h
     Classes/BaseBehavior.h
     Classes/AttackerNormalBehavior.h
     Classes/DefenderData.h
     Classes/DefenderNormalBehavior.h
     Classes/BattleTestLayer.h
     Classes/SQLiteTest.h
     Classes/SessionManager.h
     )

if(ANDROID)
    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
    set(APP_NAME MyGame)
    list(APPEND GAME_SOURCE
         proj.android/app/jni/hellocpp/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# mark app complie info and libs info
set(all_code_files
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )
if(NOT ANDROID)
    add_executable(${APP_NAME} ${all_code_files})
else()
    add_library(${APP_NAME} SHARED ${all_code_files})
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos/platform/android ${ENGINE_BINARY_PATH}/cocos/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
endif()

# Add SQLite3 dependency
find_package(SQLite3 REQUIRED)

# Link SQLite3 library to the project
target_link_libraries(${APP_NAME} cocos2d ${SQLITE3_LIBRARIES})
target_include_directories(${APP_NAME}
        PRIVATE Classes
        PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
        PRIVATE ${SQLITE3_INCLUDE_DIRS}
)

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")

    if(MACOSX)
        set_xcode_property(${APP_NAME} INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist")
    elseif(IOS)
        set_xcode_property(${APP_NAME} INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    #set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "GRLXXXX2K9")
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
    cocos_copy_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
