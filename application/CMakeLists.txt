cmake_minimum_required(VERSION 2.8)

project("muhkuh")

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

include(${wxWidgets_USE_FILE})


set_source_files_properties(muhkuh_app.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(muhkuh_app.i PROPERTIES SWIG_FLAGS "")
swig_build_wrapper(muhkuh_app_lua_wrapper lua muhkuh_app.i)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${LUA_INCLUDE_DIR})


set(muhkuh_SOURCES
	growbuffer.cpp
	license.cpp
	muhkuh_aboutDialog.cpp
	muhkuh_app.cpp
	muhkuh_configDialog.cpp
	muhkuh_config_reposEntryDialog.cpp
	muhkuh_dirlistbox.cpp
	muhkuh_htmltag_lua.cpp
	muhkuh_icons.cpp
	muhkuh_lua.cpp
	${muhkuh_app_lua_wrapper}
	muhkuh_mainFrame.cpp
	muhkuh_repository.cpp
	muhkuh_repository_manager.cpp
	muhkuh_wrap_xml.cpp
	readFsFile.cpp
)

set(muhkuh_HEADERS
	growbuffer.h
	license.h
	muhkuh_aboutDialog.h
	muhkuh_app.h
	muhkuh_configDialog.h
	muhkuh_config_reposEntryDialog.h
	muhkuh_dirlistbox.h
	muhkuh_icons.h
	muhkuh_id.h
	muhkuh_mainFrame.h
	muhkuh_repository.h
	muhkuh_repository_manager.h
	muhkuh_testTreeItemData.h
	muhkuh_version.h
	muhkuh_wrap_xml.h
	readFsFile.h
)

# Build the executable.
add_executable(muhkuh ${muhkuh_SOURCES})
target_link_libraries(muhkuh ${wxWidgets_LIBRARIES} lua)
set_target_properties(muhkuh PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

# Copy the tips file.
file(COPY ${CMAKE_HOME_DIRECTORY}/bin/muhkuh_tips.txt DESTINATION ${CMAKE_BINARY_DIR}/bin/)
