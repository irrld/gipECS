//
// Created by Metehan Gezer on 14/09/2025.
//

#ifndef GIPECS_GLOADER_H
#define GIPECS_GLOADER_H

#include "ecs/Entity.h"

namespace gecs {

class Loader {
public:
	static void loadModelComponent(ModelComponent& model, const std::string& modelpath);
};

}

#endif//SPACEFORCES_GLOADER_H
