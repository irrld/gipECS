cmake_minimum_required (VERSION 3.10.2)

##### PLUGIN NAME #####
set(pluginname gipECS)


##### PLUGIN PATHS #####
set(PLUGIN_DIR ${PLUGINS_DIR}/${pluginname})
set(VENDOR_DIR ${PLUGIN_DIR}/vendor)


list(APPEND PLUGIN_SRCS
			${PLUGIN_DIR}/src/ecs/Scene.cpp
			${PLUGIN_DIR}/src/ecs/Entity.cpp
			${PLUGIN_DIR}/src/ecs/Components.cpp
			${PLUGIN_DIR}/src/ecs/Loader.cpp
)

list(APPEND PLUGIN_INCLUDES
		${PLUGIN_DIR}/src
		${VENDOR_DIR}/entt/single_include
)

