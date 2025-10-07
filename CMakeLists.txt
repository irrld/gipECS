cmake_minimum_required (VERSION 3.10.2)

##### PLUGIN NAME #####
set(pluginname gipECS)


##### PLUGIN PATHS #####
set(PLUGIN_DIR ${PLUGINS_DIR}/${pluginname})
set(VENDOR_DIR ${PLUGIN_DIR}/vendor)


list(APPEND PLUGIN_SRCS
			${PLUGIN_DIR}/src/Scene.cpp
			${PLUGIN_DIR}/src/Entity.cpp
			${PLUGIN_DIR}/src/Components.cpp
			${PLUGIN_DIR}/src/Loader.cpp
			${PLUGIN_DIR}/src/Behavior.cpp
			${PLUGIN_DIR}/src/InputManager.cpp
			${PLUGIN_DIR}/src/CameraBehavior.cpp
)

list(APPEND PLUGIN_INCLUDES
		${PLUGIN_DIR}/include
		${VENDOR_DIR}/entt/single_include
)

