//
// Created by Metehan Gezer on 07/10/2025.
//

#ifndef GECS_ASSETSMANAGER_H
#define GECS_ASSETSMANAGER_H

#include "gImage.h"
#include "gSkybox.h"

namespace gecs {

enum class AssetType {
	STATIC_SPRITE,
	STATIC_SKYBOX_6,
};

class AssetBase {
public:
	AssetBase(AssetType type)
		: type(type) {
	}

private:
	AssetType type;
	std::string assetpath;
};

struct ImageData {
	int width;
	int height;
	int channels;
	bool ishdr;
	void* image;

	ImageData() : width(0), height(0), channels(0), ishdr(false), image(nullptr) {
	}

	ImageData(int width,
			  int height,
			  int channels,
			  bool ishdr,
			  void* image) : width(width), height(height), channels(channels), ishdr(ishdr), image(image) {
	}

	ImageData(ImageData&& data) noexcept {
		width = data.width;
		height = data.height;
		channels = data.channels;
		ishdr = data.ishdr;
		image = data.image;
		data.image = nullptr;
	}
	~ImageData() {
		if (image) {
			stbi_image_free(image);
			image = nullptr;
		}
	}
};

class SpriteAsset : public AssetBase {
public:
	explicit SpriteAsset(ImageData data)
		: AssetBase(AssetType::STATIC_SPRITE), data(std::move(data)) {
	}

	void applyTo(gImage& image);

private:
	const ImageData data;
};

class SkyboxAsset : public AssetBase {
public:
	SkyboxAsset(std::array<ImageData, 6> faces) : AssetBase(AssetType::STATIC_SKYBOX_6), faces(std::move(faces)) {
	}

	void applyTo(gSkybox& skybox);

private:
	std::array<ImageData, 6> faces;
};

class AssetsManager {
public:
	AssetsManager();

	void createStaticSkybox6(std::array<std::string, 6> paths, const std::string& ref);
	void createStaticSprite(const std::string& path, const std::string& ref);
	std::shared_ptr<AssetBase> getAsset(const std::string& ref);

private:
	std::unordered_map<std::string, std::shared_ptr<AssetBase>> assets;
};


}// namespace gecs
#endif//GECS_ASSETSMANAGER_H
