//
// Created by Metehan Gezer on 07/10/2025.
//

#ifndef GECS_REF_H
#define GECS_REF_H

#include "entt/entt.hpp"

namespace gecs {

template<typename T>
class ComponentRef {
public:
	ComponentRef()
		: entity_(entt::null), registry_(nullptr) {}
	ComponentRef(entt::registry& registry, entt::entity entity)
		: entity_(entity), registry_(&registry) {}

	T* get() {
		if (registry_ && registry_->valid(entity_)) {
			return registry_->try_get<T>(entity_);
		}
		return nullptr;
	}

	// Check if still valid
	bool isValid() const {
		return registry_ && registry_->valid(entity_) &&
			   registry_->all_of<T>(entity_);
	}

	// Arrow operator for convenience
	T* operator->() {
		return get();
	}

	// Explicit conversion
	explicit operator bool() const {
		return isValid();
	}
private:
	entt::entity entity_;
	entt::registry* registry_;
};

}
#endif//GECS_REF_H
