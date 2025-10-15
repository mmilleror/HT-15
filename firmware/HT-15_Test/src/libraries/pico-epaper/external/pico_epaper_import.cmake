# This is a copy of <PICO_EPAPER_PATH>/external/pico_epaper_import.cmake

# This can be dropped into an external project to help locate pico-epaper
# It should be include()ed prior to project()

if (DEFINED ENV{PICO_EPAPER_PATH} AND (NOT PICO_EPAPER_PATH))
    set(PICO_EPAPER_PATH $ENV{PICO_EPAPER_PATH})
    message("Using PICO_EPAPER_PATH from environment ('${PICO_EPAPER_PATH}')")
endif ()

if (NOT PICO_EPAPER_PATH)
    if (PICO_SDK_PATH AND EXISTS "${PICO_SDK_PATH}/../pico-epaper")
        set(PICO_EPAPER_PATH ${PICO_SDK_PATH}/../pico-epaper)
        message("Defaulting PICO_EPAPER_PATH as sibling of PICO_SDK_PATH: ${PICO_EPAPER_PATH}")
    else()
        message(FATAL_ERROR
                "PICO EPAPER location was not specified. Please set PICO_EPAPER_PATH or set PICO_EPAPER_FETCH_FROM_GIT to on to fetch from git."
                )
    endif()
endif ()

set(PICO_EPAPER_PATH "${PICO_EPAPER_PATH}" CACHE PATH "Path to the PICO EPAPER")

get_filename_component(PICO_EPAPER_PATH "${PICO_EPAPER_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
if (NOT EXISTS ${PICO_EPAPER_PATH})
    message(FATAL_ERROR "Directory '${PICO_EPAPER_PATH}' not found")
endif ()

set(PICO_EPAPER_PATH ${PICO_EPAPER_PATH} CACHE PATH "Path to the PICO EPAPER" FORCE)

add_subdirectory(${PICO_EPAPER_PATH} pico_epaper)