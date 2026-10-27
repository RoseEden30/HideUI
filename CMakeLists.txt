cmake_minimum_required(VERSION 3.21)

project(HideUI VERSION 1.0.0 LANGUAGES CXX)

set(PLUGIN_AUTHOR "RoseEden" CACHE STRING "Author name baked into the plugin declaration")

# SKYRIM_FOLDER = game install, SKYRIM_MODS_FOLDER = MO2/Vortex mods folder.
if(DEFINED ENV{SKYRIM_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_FOLDER}/Data")
    set(OUTPUT_FOLDER "$ENV{SKYRIM_FOLDER}/Data")
endif()

if(DEFINED ENV{SKYRIM_MODS_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_MODS_FOLDER}")
    set(OUTPUT_FOLDER "$ENV{SKYRIM_MODS_FOLDER}/${PROJECT_NAME}")
endif()

set(BUILD_TESTS OFF CACHE BOOL "" FORCE)

set(
    COMMONLIBSSE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/CommonLibSSE-NG"
    CACHE PATH
    "Path to CommonLibSSE-NG"
)

add_subdirectory(
    "${COMMONLIBSSE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/commonlibsse-ng"
)

include("${COMMONLIBSSE_DIR}/cmake/CommonLibSSE.cmake")

add_commonlibsse_plugin(
    ${PROJECT_NAME}
    AUTHOR ${PLUGIN_AUTHOR}
    SOURCES
        src/plugin.cpp
)

# Framework headers are header only, resolved through GetProcAddress.
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)

target_compile_options(${PROJECT_NAME} PRIVATE /Z7)
target_link_options(${PROJECT_NAME} PRIVATE /DEBUG /OPT:REF /OPT:NOICF)

if(DEFINED OUTPUT_FOLDER)
    set(DLL_FOLDER "${OUTPUT_FOLDER}/SKSE/Plugins")

    message(STATUS "SKSE plugin output folder: ${DLL_FOLDER}")

    add_custom_command(
        TARGET "${PROJECT_NAME}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${DLL_FOLDER}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:${PROJECT_NAME}>"
            "${DLL_FOLDER}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_PDB_FILE:${PROJECT_NAME}>"
            "${DLL_FOLDER}/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/package/${PROJECT_NAME}.ini"
            "${DLL_FOLDER}/${PROJECT_NAME}.ini"
        VERBATIM
    )
endif()
