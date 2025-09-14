I'll be adding more detailed examples, but here you go:

```c++
void gApp::setup() {
	appmanager->setTargetFramerate(100000);

	gSceneCanvas* canvas = new gSceneCanvas(this);
	canvas->setScene(std::make_unique<gScene>());
	appmanager->setCurrentCanvas(canvas);
	gScene* scene = canvas->getScene();
	{
		gEntity entity = scene->createEntity("Bullet");
		gSpriteComponent& sprite = entity.addComponent<gSpriteComponent>();
		loadSpriteComponent(sprite, "ai_bullet.png");
	}
	{
		gEntity entity = scene->createEntity("Map");
		gSpriteComponent& sprite = entity.addComponent<gSpriteComponent>();
		loadSpriteComponent(sprite, "menu_background2.png");
		gTransformComponent& transform = entity.addComponent<gTransformComponent>();
		transform.scale = glm::vec3(0.5f, 0.5f, 1.0f);
	}
	scene->bindSystem<gEntity, gTransformComponent>(gSystem::UPDATE, "Bullet", G_BIND_FUNCTION(updateBullets));
}

// Only called if the name of the entity is "Bullet" and has the gTransformComponent.
void gApp::updateBullets(float deltatime, gEntity entity, gTransformComponent& component) {
	gLogi("gApp") << "updateBullets " << (uint64_t)entity.getHandle();
	component.position.y += 10 * deltatime;
}

```

# Differences

- Y-axis is inverted for sprites. Bottom left is the origin instead of top left. y=0 is bottom, y>=0 is top.

# Requirements

- You need an up-to-date version of the GlistEngine.
- entt library is included in the vendor folder. You need to make sure to clone the repository recursively.
- Projects using this plugin need to use C++17 or higher. C++14 is not supported by the entt library.
