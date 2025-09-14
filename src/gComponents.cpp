//
// Created by Metehan Gezer on 14/09/2025.
//

#include "gComponents.h"

#include "gEntity.h"

void loadSpriteComponent(gSpriteComponent& component, const std::string& imagepath) {
	component.image.loadImage(imagepath);
}