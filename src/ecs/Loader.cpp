//
// Created by Metehan Gezer on 14/09/2025.
//

#include "Loader.h"

namespace gecs {

void Loader::loadSpriteComponent(SpriteComponent& component, const std::string& imagepath) {
	component.data.loadImage(imagepath);
}

void Loader::loadModelComponent(ModelComponent& model, const std::string& modelpath) {
	model.data.loadModel(modelpath);
}


}
