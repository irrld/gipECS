# gipECS
gipECS or gecs for short is a plugin for the GlistEngine that provides an Entity Component System.

All functions and classes are in the namespace gecs, this is to provide a clear distinction between other code. We are defining some stuff that is very likely to exist in other places, by using a namespace we can guarantee that we are completely unique and developers can chose to add `using namespace gecs;` in case there is no conflict.

# Examples

I'll be adding more detailed examples, but here you go:

```c++
using namespace gecs;

void gApp::setup() {
	ismuted = false;
	appmanager->setTargetFramerate(100000);

	SceneCanvas* canvas = new SceneCanvas(this);
	appmanager->setCurrentCanvas(canvas);

	// An empty scene is automatically created.
	Scene* scene = canvas->getScene();
	{
		// Create an entity named "Bullet"
		Entity entity = scene->createEntity("Bullet");
		SpriteComponent& sprite = entity.addComponent<SpriteComponent>();
		// Load the image from assets/images/ai_bullet.png
		Loader::loadSpriteComponent(sprite, "ai_bullet.png");
	}
	{
		// Create an entity named "Map"
		Entity entity = scene->createEntity("Map");
		SpriteComponent& sprite = entity.addComponent<SpriteComponent>();
		// Load the image from assets/images/menu_background2.png
		Loader::loadSpriteComponent(sprite, "menu_background2.png");
	}
	{
		Entity entity = scene->createEntity("Camera");
		CameraComponent& cam = entity.addComponent<CameraComponent>();
		cam.isenabled = true;
		TransformComponent& transform = entity.getComponent<TransformComponent>();
		transform.setPosition(0.2f, 0, 0.5f);
		transform.lookAt(0, 0, 0);
		BehaviorsComponent& behaviors = entity.addComponent<BehaviorsComponent>();
		behaviors.addBehavior<CameraBehavior>();
	}
	{
		Entity entity = scene->createEntity("Light");
		LightAmbientComponent& light = entity.addComponent<LightAmbientComponent>();
		light.isenabled = true;
	}
	{
		Entity entity = scene->createEntity("Solider");
		ModelComponent& model = entity.addComponent<ModelComponent>();
		// Load the image from assets/images/menu_background2.png
		Loader::loadModelComponent(model, "link/untitled.fbx");
		TransformComponent& transform = entity.getComponent<TransformComponent>();
		transform.setScale(0.001f, 0.001f, 0.001f);
		transform.rotate(180, 0, 0);
	}
	// Bind a system, which runs on each update cycle. Only runs on entities which are named "Bullet"
	scene->bindSystem<Entity, TransformComponent>(SystemType::UPDATE, "Bullet", G_BIND_FUNCTION(updateBullet));
	// Bind a system, which runs on each update cycle. Only runs on entities which are named "Solider"
	scene->bindSystem<Entity, ModelComponent>(SystemType::UPDATE, "Solider", G_BIND_FUNCTION(moveModel));
}

void gApp::updateBullet(float deltatime, Entity entity, TransformComponent& transform) {
	transform.move(10 * deltatime, 0.0f, 0.0f);
}

void gApp::moveModel(float deltatime, Entity entity, ModelComponent& component) {
	TransformComponent& transform = entity.getComponent<TransformComponent>();
	transform.move(0, 0, deltatime * -0.1f);
}
```

No other code is required to get the sprites to draw on the screen.

# Differences

- Y-axis is inverted for sprites. Bottom left is the origin instead of top left. y=0 is bottom, y>=0 is top.

# Requirements

- You need an up-to-date version of the GlistEngine.
- entt library is included in the vendor folder. You need to make sure to clone the repository recursively.
- Projects using this plugin need to use C++17 or higher. C++14 is not supported by the entt library.
