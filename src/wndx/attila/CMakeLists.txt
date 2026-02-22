## AUTHOR: 'WANDEX/attila'
## Cmake conf which uses modern cmake targets approach & per target properties.
## Compiles target which uses Qt sources as separate static library.
##
## Per target properties:
## AUTOMOC, AUTOUIC instead of obsolete & de facto standard approach.
## Old approach used by nearly everyone across the web is obsolete:
##  1. Pollutes build directory.
##  2. Increases build times as unneeded properties are added to all targets.
##  3. Which leads to excessive/unneeded code generation by the Qt framework.

## cmake-properties(7)
## cmake-qt(7): /AUTOUIC -- Multi-Config "include_<CONFIG>", otherwise "include".
# set(ATTILA_INC_DIR_NAME $<IF:$<BOOL:$<CONFIG>>,include_${CMAKE_BUILD_TYPE},include>)
# set(ATTILA_INC_DIR_NAME $<IF:$<BOOL:$<CONFIG>>,include_$<CONFIG>,include>)
# set(ATTILA_AUTOGEN_INC  "${AUTOGEN_BUILD_DIR}/${ATTILA_INC_DIR_NAME}")

# find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
# find_package(QT NAMES Qt6) # => populate QT_VERSION variables!

target_link_libraries(attila_deps PUBLIC Qt6::Core Qt6::Widgets)

## sources which do NOT use Qt headers
add_library(attila_common OBJECT)
target_sources(attila_common
  PUBLIC FILE_SET headers TYPE HEADERS FILES
    structs.hpp
    str.hpp
    stats.hpp
    attila.hpp
  PRIVATE
    str.cpp
    stats.cpp
    attila.cpp
)
target_link_libraries(attila_common PRIVATE attila_src)

set(ATTILA_UI_FILES mainwindow.ui)

## sources which use Qt headers & headers generated from .ui files
qt_add_library(attila_wdgt STATIC)
target_sources(attila_wdgt
  PUBLIC FILE_SET headers TYPE HEADERS FILES
    ui.hpp
    action.hpp
    keys.hpp
    mainwindow.hpp
  PRIVATE
    ${ATTILA_UI_FILES} # adding them here triggers AUTOUIC
    ui.cpp
    action.cpp
    keys.cpp
    mainwindow.cpp
)
target_link_libraries(attila_wdgt PRIVATE attila_common attila_deps)

set_target_properties(attila_wdgt PROPERTIES
  AUTOMOC ON # => Q_OBJECT
  AUTOUIC ON # => find -> generate: *.ui -> ui_*.h
  # AUTORCC ON
  # AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/attila_wdgt_autogen"
  LINKER_LANGUAGE CXX
)

qt_add_executable(attila_app main.cpp)
target_link_libraries(attila_app PRIVATE attila_wdgt attila_deps)
# target_link_libraries(attila_app PRIVATE attila_dev) # uncomment & FIXME

set_target_properties(attila_app PROPERTIES
  MACOSX_BUNDLE_GUI_IDENTIFIER org.wndx.attila
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  MACOSX_BUNDLE TRUE
  WIN32_EXECUTABLE TRUE
)
