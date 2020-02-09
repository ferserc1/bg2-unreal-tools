
#include "bg2tools/scene_object.hpp"
#include "Bg2Reader.h"
#include "Bg2UnrealTools.h"

#include <map>

namespace bg2tools {

	void PolyListData::applyTransform(const float4x4& trx) {
		for (size_t i = 0; i < vertex.size(); i += 3)
		{
			float3 v(vertex[i], vertex[i + 1], vertex[i + 2]);
			float3 n(normal[i], normal[i + 1], normal[i + 2]);

			v = trx *  v;
			float4x4 nmat = trx.rotation();
			n = nmat * n;
			n.normalize();

			vertex[i] = v[0]; vertex[i + 1] = v[1]; vertex[i + 2] = v[2];
			normal[i] = n[0]; normal[i + 1] = n[1]; normal[i + 2] = n[2];
		}
	}

	DrawableData::~DrawableData() {
		for (auto pl : plists) {
			delete pl;
		}
		plists.clear();
	}

	bool DrawableData::loadDrawable(const std::string& path) {
		modelPath = path;
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