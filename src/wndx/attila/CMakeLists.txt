set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON) # XXX

# include_directories(/home/wndx/source/projects/attila/build/unix_makefiles/x86_64-gcc-debug/src/wndx/attila/attila_app_autogen/include) # XXX ui_mainwindow.h

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(ATTILA_SOURCES "")
list(APPEND ATTILA_SOURCES
  action.cpp
  action.hpp
  attila.cpp
  attila.hpp
  keys.cpp
  keys.hpp
  mainwindow.cpp
  mainwindow.hpp
  stats.cpp
  stats.hpp
  str.cpp
  str.hpp
  structs.hpp
)


set(ATTILA_UI_FILES mainwindow.ui)
# qt_wrap_ui(ATTILA_UI_HEADERS ${ATTILA_UI_FILES}) # XXX: without this ui_mainwindow.h is not generated!
if(NOT ATTILA_UI_HEADERS)
  list(APPEND ATTILA_UI_HEADERS ${ATTILA_UI_FILES})
endif()

set(ATTILA_AUTOGEN_INC ${CMAKE_CURRENT_BINARY_DIR}/src/wndx/attila/attila_app_autogen/include)

if(FALSE)
  ## XXX: if enabled cannot find ui_mainwindow.h
  target_include_directories(attila_src PUBLIC ${CMAKE_CURRENT_LIST_DIR})
  # target_include_directories(attila_src PUBLIC ${CMAKE_CURRENT_BINARY_DIR}) # XXX: fixes?
  # target_include_directories(attila_src PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/src/wndx/attila) # XXX: fixes?
  target_include_directories(attila_app PUBLIC ${ATTILA_AUTOGEN_INC}) # XXX ui_mainwindow.h
  target_sources(attila_src PUBLIC ${ATTILA_SOURCES} ${ATTILA_UI_HEADERS}) # XXX: PRIVATE or PUBLIC?
  ## XXX: qt_add_ui() not works on my target?
  # qt_add_ui(attila_src INCLUDE_PREFIX "src/wndx/attila" SOURCES mainwindow.ui)
  # qt_add_ui(attila_src INCLUDE_PREFIX "src/wndx/attila" SOURCES ${ATTILA_UI_HEADERS})
  # qt_add_ui(attila_src SOURCES mainwindow.ui) # XXX
endif()

qt_add_executable(attila_app ${ATTILA_SOURCES} ${ATTILA_UI_HEADERS} main.cpp)
# target_include_directories(attila_app PUBLIC /home/wndx/source/projects/attila/build/unix_makefiles/x86_64-gcc-debug/src/wndx/attila/attila_app_autogen/include) # XXX  ui_mainwindow.h
target_include_directories(attila_app PUBLIC ${ATTILA_AUTOGEN_INC}) # XXX ui_mainwindow.h
# qt_add_ui(attila_app SOURCES ${ATTILA_UI_HEADERS})
target_link_libraries(attila_app PRIVATE attila_deps)
target_link_libraries(attila_app PRIVATE Qt6::Core Qt6::Widgets)

set_target_properties(attila_app PROPERTIES
  MACOSX_BUNDLE_GUI_IDENTIFIER org.wndx.attila
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  MACOSX_BUNDLE TRUE
  WIN32_EXECUTABLE TRUE
)
