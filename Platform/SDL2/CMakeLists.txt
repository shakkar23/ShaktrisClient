
set(PLATFORM_SOURCES
	${GAME_SOURCES}
	"${SRC}/Platform/SDL2/Engine.cpp"
	"${SRC}/Platform/SDL2/TextureManager.cpp"
	"${SRC}/Platform/SDL2/Menus/Menus.cpp"
	"${SRC}/Platform/SDL2/Menus/MainMenu/MainMenu.cpp"
	"${SRC}/Platform/SDL2/Menus/MainMenu/Play/Play.cpp"
	"${SRC}/Platform/SDL2/Menus/MainMenu/Selection/Selection.cpp"
	"${SRC}/Platform/SDL2/Menus/MainMenu/Settings/Settings.cpp"
	"${SRC}/Platform/SDL2/gameManager/gameManager.cpp"
)

set(PLATFORM_HEADERS 
	"${SRC}/Platform/SDL2/headers/Engine.hpp"
	"${SRC}/Platform/SDL2/headers/TextureManager.hpp"
	"${SRC}/Platform/SDL2/Menus/Menus.hpp"
	"${SRC}/Platform/SDL2/Menus/MainMenu/MainMenu.hpp"
	"${SRC}/Platform/SDL2/Menus/MainMenu/Play/Play.hpp"
	"${SRC}/Platform/SDL2/Menus/MainMenu/Selection/Selection.hpp"
	"${SRC}/Platform/SDL2/Menus/MainMenu/Settings/Settings.hpp"
	"${SRC}/Platform/SDL2/gameManager/gameManager.hpp"
	"${SRC}/Platform/SDL2/Audio/Audio.hpp"
	)

set(REGISTRY_HEADERS 
	"${SRC}/Platform/SDL2/headers/Game.hpp"
	"${SRC}/Platform/SDL2/Audio/Audio.hpp"
	"${SRC}/ShakTris/Plugins/PluginHeaders/PluginRegistry.hpp"
	"${SRC}/ShakTris/Plugins/PluginManager.hpp"
)
set(REGISTRY_INCLUDE 
"${SRC}/Platform/SDL2/headers"
"${SRC}/ShakTris/Plugins"
)


set(REGISTRY_SOURCES 
	"${SRC}/ShakTris/Plugins/PluginHeaders/PluginRegistry.cpp"
	"${SRC}/ShakTris/Plugins/PluginManager.cpp"
	"${SRC}/Platform/SDL2/Audio/Audio.cpp"
)


set(ICON_DIR "${CMAKE_SOURCE_DIR}/Asset/Icon")

include("${SRC}/Platform/SDL2/ExecutableTypeSimple.cmake")

#AddBinary(
#	Registry
#	RUNTIME
#	SOURCES ${REGISTRY_SOURCES}
#	INCLUDE_DIRS ${REGISTRY_HEADERS}
#	LIBRARIES
#		libshaktris
#		${PLATFORM_LIBRARIES}
#)
project(Registry)

if (WIN32)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-export-all-symbols")

endif()


add_library(Registry SHARED ${REGISTRY_SOURCES})
target_include_directories(Registry PUBLIC ${REGISTRY_INCLUDE})
target_link_libraries(Registry 
	PUBLIC SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf 
	PRIVATE $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
)
set_target_properties( 
		Registry
    PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
		PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/pdb"
)
