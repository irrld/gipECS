//
// Created by Metehan Gezer on 07/10/2025.
//

#ifndef GECS_ASSETSMANAGER_H
#define GECS_ASSETSMANAGER_H

#include "gImage.h"

namespace gecs {

enum class AssetType {
	STATIC_SPRITE
};

class AssetBase {
public:
	AssetBase(AssetType type, const std::string& assetpath)
		: type(type), assetpath(assetpath) {

	}
private:
	AssetType type;
	std::string assetpath;
};

class SpriteAsset : public AssetBase {
public:
	SpriteAsset(const std::string& assetpath, int width, int height, int channels, bool ishdr, void* data)
		: AssetBase(AssetType::STATIC_SPRITE, assetpath), width(width), height(height), channels(channels), ishdr(ishdr), data(data) {

	}
	~SpriteAsset() {
		if (data) {
			stbi_image_free(data);
			data = nullptr;
		}
	}

	void applyTo(gImage& image);
private:
	int width, height, channels;
	bool ishdr;
	void* data;
};

class AssetsManager {
public:
	AssetsManager();

	void loadAsset(AssetType type, const std::string& path, const std::string& ref);
	std::shared_ptr<AssetBase> getAsset(const std::string& ref);

private:
	void loadStaticSprite( const std::string& path, const std::string& ref);

	std::unordered_map<std::string, std::shared_ptr<AssetBase>> assets;
};


}
#endif//GECS_ASSETSMANAGER_H
