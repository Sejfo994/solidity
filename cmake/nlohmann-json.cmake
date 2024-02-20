include(ExternalProject)

ExternalProject_Add(nlohmann-json
        DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/deps/nlohmann/json"
        DOWNLOAD_NAME json.hpp
        DOWNLOAD_NO_EXTRACT 1
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp
        URL_HASH SHA256=9bea4c8066ef4a1c206b2be5a36302f8926f7fdc6087af5d20b417d0cf103ea6
        CMAKE_COMMAND true
        BUILD_COMMAND true
        INSTALL_COMMAND true
)

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/deps/nlohmann)
