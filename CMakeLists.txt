cmake_minimum_required (VERSION 3.10.2)

##### PLUGIN NAME #####
set(pluginname gipECS)


##### PLUGIN PATHS #####
set(PLUGIN_DIR ${PLUGINS_DIR}/${pluginname})
set(VENDOR_DIR ${PLUGIN_DIR}/vendor)


list(APPEND PLUGIN_SRCS
			${PLUGIN_DIR}/src/gScene.cpp
			${PLUGIN_DIR}/src/gEntity.cpp
			${PLUGIN_DIR}/src/gUUID.cpp
#			${PLUGIN_DIR}/src/gComponentBase.cpp
)

list(APPEND PLUGIN_INCLUDES
		${PLUGIN_DIR}/src
		${VENDOR_DIR}/entt/single_include
)

