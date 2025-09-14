//
// Created by Metehan Gezer on 14/09/2025.
//

#ifndef GIPECS_GSYSTEM_H
#define GIPECS_GSYSTEM_H

#include <functional>
#include <type_traits>
#include "entt/entt.hpp"

/**
 * @brief Type trait to detect component references
 *
 * Components are identified as reference types (T&).
 * Used to validate system function parameters at compile time.
 */
template<typename T>
struct is_component : std::false_type {};

template<typename T>
struct is_component<T&> : std::true_type {};

/**
 * @brief Alternative system function wrapper using std::function
 *
 * This version uses type erasure to handle generic lambdas from G_BIND_FUNCTION.
 * It's slightly less efficient but more flexible.
 */
template<typename Entity, typename Scene, typename... Components>
class TypedSystemFunction {
private:
    std::function<void(float, Entity, Components&...)> func;

public:
    /**
     * @brief Construct from any callable that matches the signature
     */
    template<typename Func>
    explicit TypedSystemFunction(Func f) : func(f) {}

    /**
     * @brief Execute the system function
     */
    void operator()(Scene* scene, float deltatime, entt::entity entity, entt::registry& registry) {
    	if constexpr (sizeof...(Components) == 0) {
    		func(deltatime, Entity{entity, scene});
    	} else {
    		std::apply([this, deltatime, entity, scene](auto&&... comps) {
				func(deltatime, Entity{entity, scene}, comps...);
			}, std::forward_as_tuple(registry.get<std::remove_reference_t<Components>>(entity)...));
    	}
    }

    /**
     * @brief Check if entity has all required components
     */
    bool hasComponents(entt::entity entity, entt::registry& registry) {
        if constexpr (sizeof...(Components) == 0) {
            return true;
        } else {
            return (registry.any_of<std::remove_reference_t<Components>>(entity) && ...);
        }
    }
};

/**
 * @brief Helper to create TypedSystemFunction with deduced component types
 *
 * Usage: makeSystemFunction<gTransformComponent>(G_BIND_FUNCTION(updateBullets))
 */
template<typename... Components, typename Func>
auto makeSystemFunction(Func&& f) {
    return TypedSystemFunction<Components...>(std::forward<Func>(f));
}

/**
 * @brief Macro for binding member functions to systems with explicit component types
 *
 * Creates a lambda that captures 'this' and creates a typed system function.
 *
 * Usage: G_BIND_TYPED_FUNCTION(memberFunctionName, ComponentA, ComponentB)
 */
#define G_BIND_TYPED_FUNCTION(fn, ...) \
    makeSystemFunction<__VA_ARGS__>([this](float dt, gEntity e, __VA_ARGS__& ... args) { \
        return this->fn(dt, e, args...); })

#endif//GIPECS_GSYSTEM_H
