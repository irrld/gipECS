//
// Created by Metehan Gezer on 07/10/2025.
//

#include "ecs/AssetsManager.h"

namespace gecs {
/*
SpriteAsset::SpriteAsset(const std::string& assetpath)
	   : AssetBase(AssetType::STATIC_SPRITE, assetpath) {
		gImage image;
	std::string fullpath = gObject::gGetAssetsDir() + assetpath;
	ishdr = stbi_is_hdr(fullpath.c_str());
	if (is_hdr) {
		data = stbi_loadf(filepath, &width, &height, &channels, 0);
	} else {
		data = stbi_load(filepath, &width, &height, &channels, 0);
	}

	if (!data) {
		gLoge("SpriteAsset")
		printf("Failed to load %s: %s\n", filepath, stbi_failure_reason());
	}

	image.loadImageData()
}*/

void SpriteAsset::applyTo(gImage& image) {
	if (ishdr) {
		image.setDataHDR(static_cast<float*>(data), width, height, channels, false, false);
	} else {
		image.setData(static_cast<unsigned char*>(data), width, height, channels, false, false);
	}
}

AssetsManager::AssetsManager() {

}

void AssetsManager::loadAsset(AssetType type, const std::string& path, const std::string& ref) {
	switch (type) {
	case AssetType::STATIC_SPRITE:
		loadStaticSprite(path, ref);
		break;
	}

}

void AssetsManager::loadStaticSprite(const std::string& path, const std::string& ref) {
	std::string fullpath = gObject::gGetAssetsDir() + path;
	bool ishdr = stbi_is_hdr(fullpath.c_str());
	void* data;
	int width, height, channels;
	if (ishdr) {
		data = stbi_loadf(fullpath.c_str(), &width, &height, &channels, 0);
	} else {
		data = stbi_load(fullpath.c_str(), &width, &height, &channels, 0);
	}

	if (!data) {
		gLoge("AssetsManager") << "Failed to to load asset at " << path << " (" << fullpath << ")";
		return;
	}
	assets[ref] = std::make_shared<SpriteAsset>(path, width, height, channels, ishdr, data);
}


std::shared_ptr<AssetBase> AssetsManager::getAsset(const std::string& ref) {
	return assets[ref];
}



}