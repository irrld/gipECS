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
	if (data.ishdr) {
		image.setDataHDR(static_cast<float*>(data.image), data.width, data.height, data.channels, false, false);
	} else {
		image.setData(static_cast<unsigned char*>(data.image), data.width, data.height, data.channels, false, false);
	}
}

void SkyboxAsset::applyTo(gSkybox& skybox) {
	std::array<int, 6> widths{};
	std::array<int, 6> heights{};
	std::array<void*, 6> rawdata{};
	std::array<bool, 6> ishdr{};
	for (size_t i = 0; i < 6; i++) {
		widths[i] = faces[i].width;
		heights[i] = faces[i].height;
		rawdata[i] = faces[i].image;
		ishdr[i] = faces[i].ishdr;
	}
	skybox.loadSkybox(widths, heights, rawdata, ishdr);
}

AssetsManager::AssetsManager() {

}

void AssetsManager::createStaticSkybox6(std::array<std::string, 6> paths, const std::string& ref) {
	std::array<ImageData, 6> data{};
	for (size_t i = 0; i < 6; i++) {
		const std::string& path = paths[i];
		std::string fullpath = gObject::gGetAssetsDir() + path;
		bool ishdr = stbi_is_hdr(fullpath.c_str());
		if (ishdr) {
			data[i].image = stbi_loadf(fullpath.c_str(), &data[i].width, &data[i].height, &data[i].channels, 3);
		} else {
			data[i].image = stbi_load(fullpath.c_str(), &data[i].width, &data[i].height, &data[i].channels, 3);
		}

		if (!data[i].image) {
			gLoge("AssetsManager") << "Failed to to load asset at " << path << " (" << fullpath << ")";
			return;
		}
	}
	assets[ref] = std::make_shared<SkyboxAsset>(std::move(data));
}

void AssetsManager::createStaticSprite(const std::string& path, const std::string& ref) {
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
	assets[ref] = std::make_shared<SpriteAsset>(ImageData{width, height, channels, ishdr, data});
}


std::shared_ptr<AssetBase> AssetsManager::getAsset(const std::string& ref) {
	return assets[ref];
}



}