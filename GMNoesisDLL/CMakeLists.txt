cmake_minimum_required(VERSION 3.15)
project(GMNoesis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
)
add_library(GMNoesis SHARED ${SOURCES})

set_target_properties(GMNoesis PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin
)

foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    set_target_properties(GMNoesis PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_SOURCE_DIR}/build/lib
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_SOURCE_DIR}/build/bin
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_SOURCE_DIR}/build/bin
    )
endforeach()

target_include_directories(GMNoesis PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/ThirdParty/Noesis/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/Noesis/Src/Packages/App/ApplicationLauncher/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/Noesis/Src/Packages/App/Theme/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/Noesis/Src/Packages/App/Providers/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/Noesis/Src/Packages/Render/D3D11RenderDevice/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/Noesis/Src/Packages/Render/RenderContext/include
)

if(WIN32)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
endif()

target_link_libraries(GMNoesis PRIVATE
    d2d1
    d3d11
    dxgi
    ${CMAKE_SOURCE_DIR}/ThirdParty/Noesis/Lib/windows_x86_64//Noesis.lib
    ${CMAKE_SOURCE_DIR}/ThirdParty/Noesis/Lib/windows_x86_64/NoesisApp.lib
)

set(EXTENSIONS_DIR "${CMAKE_SOURCE_DIR}/../GMNoesis/extensions/GMNoesis")


add_custom_command(TARGET GMNoesis POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${EXTENSIONS_DIR}"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:GMNoesis>
        "${EXTENSIONS_DIR}/GMNoesis.dll"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_PDB_FILE:GMNoesis>
        "${EXTENSIONS_DIR}/GMNoesis.pdb"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/ThirdParty/Noesis/bin/windows_x86_64/Noesis.dll
        "${EXTENSIONS_DIR}/Noesis.dll"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/ThirdParty/Noesis/bin/windows_x86_64/NoesisApp.dll
        ${EXTENSIONS_DIR}/NoesisApp.dll

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/ThirdParty/Noesis/bin/windows_x86_64/NoesisApp.pdb
        ${EXTENSIONS_DIR}/NoesisApp.pdb)


set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GMNoesis)