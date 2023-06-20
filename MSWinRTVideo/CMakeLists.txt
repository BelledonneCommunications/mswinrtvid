############################################################################
# CMakeLists.txt
# Copyright (C) 2010-2023 Belledonne Communications, Grenoble France
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

include(GNUInstallDirs)

set(SOURCE_FILES
	"SchemeHandler.cpp"
	"SwapChainPanelSource.cpp"
	"SchemeHandler.h"
	"SharedData.h"
	"SwapChainPanelSource.h"
)

add_library(MSWinRTVideo ${SOURCE_FILES})
target_link_libraries(MSWinRTVideo PRIVATE mfplat.lib mfuuid.lib WindowsApp.lib)
if(MSVC)
	set_target_properties(MSWinRTVideo PROPERTIES VS_WINRT_COMPONENT TRUE)
else()
	target_compile_options(MSWinRTVideo PRIVATE -ZW -EHsc)
endif()
if(ENABLE_STRICT)
	target_compile_options(MSWinRTVideo PRIVATE -WX)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	install(FILES $<TARGET_PDB_FILE:MSWinRTVideo>
		DESTINATION ${CMAKE_INSTALL_BINDIR}
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
endif()

install(TARGETS MSWinRTVideo
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
