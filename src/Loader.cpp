//
// Created by Metehan Gezer on 14/09/2025.
//

#include "ecs/Loader.h"

namespace gecs {

void Loader::loadModelComponent(ModelComponent& model, const std::string& modelpath) {
	model.data.loadModel(modelpath);
}


}
