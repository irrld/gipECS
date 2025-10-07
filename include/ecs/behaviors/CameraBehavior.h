//
// Created by Metehan Gezer on 07/10/2025.
//

#ifndef GECS_CAMERABEHAVIOR_H
#define GECS_CAMERABEHAVIOR_H

#include "ecs/Behavior.h"

namespace gecs {

struct CameraBehavior : public BehaviorBase {
	~CameraBehavior() override {}
	void onStart() override;
	void onUpdate(float deltatime) override;
	bool onKeyPressed(KeyCode key) override;

	float movespeed = 2.0f;
private:
	ComponentRef<TransformComponent> transform;
};

}
#endif//SPACEFORCES_CAMERABEHAVIOR_H
