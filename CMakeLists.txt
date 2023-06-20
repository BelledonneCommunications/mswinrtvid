############################################################################
# CMakeLists.txt
# Copyright (C) 2016-2023  Belledonne Communications, Grenoble France
#
############################################################################
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
############################################################################

cmake_minimum_required(VERSION 3.22)

project(MSWINRTVID CXX)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/mediastreamer2/plugins")

option(ENABLE_STRICT "Build with strict compile options." YES)

find_package(Mediastreamer2 5.3.0 REQUIRED)

set(SOURCE_FILES
	"IVideoDispatcher.h"
	"IVideoRenderer.h"
	"LinkList.h"
	"MediaEngineNotify.cpp"
	"MediaEngineNotify.h"
	"MediaStreamSource.cpp"
	"MediaStreamSource.h"
	"mswinrtbackgrounddis.cpp"
	"mswinrtbackgrounddis.h"
	"mswinrtcap.cpp"
	"mswinrtcap.h"
	"mswinrtdis.cpp"
	"mswinrtdis.h"
	"mswinrtmediasink.cpp"
	"mswinrtmediasink.h"
	"mswinrtvid.cpp"
	"mswinrtvid.h"
	"RemoteHandle.cpp"
	"RemoteHandle.h"
	"Renderer.cpp"
	"Renderer.h"
	"ScopeLock.cpp"
	"ScopeLock.h"
	"MSWinRTVideo/SharedData.h"
	"VideoBuffer.h"
)

add_library(mswinrtvid MODULE ${SOURCE_FILES})
set_target_properties(mswinrtvid PROPERTIES VERSION 0)
set_target_properties(mswinrtvid PROPERTIES PREFIX "lib")
target_link_libraries(mswinrtvid PRIVATE ${Mediastreamer2_TARGET} mfplat.lib mfuuid.lib WindowsApp.lib)
target_compile_definitions(mswinrtvid PRIVATE "_WIN32_WINNT=0x0A00" "_ALLOW_KEYWORD_MACROS")
if(MSVC)
	set_target_properties(mswinrtvid PROPERTIES VS_WINRT_COMPONENT TRUE)
else()
	target_compile_options(mswinrtvid PRIVATE -ZW -EHsc)
endif()
if(ENABLE_STRICT)
	target_compile_options(mswinrtvid PRIVATE -WX)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	install(FILES $<TARGET_PDB_FILE:mswinrtvid>
		DESTINATION lib/mediastreamer/plugins
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
endif()

add_subdirectory("MSWinRTVideo")

install(TARGETS mswinrtvid
	RUNTIME DESTINATION "${Mediastreamer2_PLUGINS_DIR}"
	LIBRARY DESTINATION "${Mediastreamer2_PLUGINS_DIR}"
	ARCHIVE DESTINATION "${Mediastreamer2_PLUGINS_DIR}"
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
