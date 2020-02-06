
#include "bg2tools/scene_object.hpp"
#include "Bg2Reader.h"
#include "Bg2UnrealTools.h"

#include <map>

namespace bg2tools {

	void PolyListData::applyTransform(const float4x4& trx) {
		// TODO: aplicar trx a vértices y normales
	}

	DrawableData::~DrawableData() {
		for (auto pl : plists) {
			delete pl;
		}
		plists.clear();
	}

	// TODO: implementar Bg2Model::Load usando un DrawableData que cargaremos
	// previamente con loadDrawable, es decir Bg2Model::Load no recibirá un path, sino
	// un DrawableData. Este DrawableData puede haber sido convertido previamente
	// con las funciones de aplicar matrices, para que las coordenadas de los objetos
	// aparezcan ya transformadas
	bool DrawableData::loadDrawable(const std::string& path) {
		Bg2Reader reader;
		PolyListData * currentPolyList = new PolyListData();

		reader.Version([&](uint8_t, uint8_t, uint8_t) {

		})
		.Materials([&](const std::string& matData) {
			materialData = matData;
		})
		.PlistName([&](const std::string & plistName) {
			currentPolyList->plistName = plistName;
		})
		.MaterialName([&](const std::string & matName) {
			currentPolyList->matName = matName;
		})
		.Vertex([&](const std::vector<float> & v) {
			currentPolyList->vertex = v;
		})
		.Normal([&](const std::vector<float> & v) {
			currentPolyList->normal = v;
		})
		.Uv0([&](const std::vector<float>& v) {
			currentPolyList->uv0 = v;
		})
		.Uv1([&](const std::vector<float>& v) {
			currentPolyList->uv1 = v;
		})
		.Index([&](const std::vector<unsigned int> & index) {
			currentPolyList->index = index;
			plists.push_back(currentPolyList);
			currentPolyList = new PolyListData();
		})
		.Error([&](const std::exception & e) {
			// ERROR
			UE_LOG(Bg2Tools, Error, TEXT("Error loading bg2 scene: %s"), e.what());
		});

		if (reader.Load(path)) {
			if (currentPolyList) {
				delete currentPolyList;
			}
			return true;
		}
		else {
			return false;
		}
	}

	void DrawableData::applyTransform(const float4x4& trx) {
		for (auto pl : plists) {
			pl->applyTransform(trx);
		}
	}
}