#pragma once

#include "bg2tools/matrix.hpp"

#include <vector>
#include <string>

namespace bg2tools {

	struct PolyListData {
		std::string matName;
		std::string plistName;
		std::string groupName;
		std::vector<float> vertex;
		std::vector<float> normal;
		std::vector<float> uv0;
		std::vector<float> uv1;
		std::vector<unsigned int> index;

		// Aplica trx a vértices y normales
		void applyTransform(const float4x4& trx);
	};

	struct DrawableData {
		std::vector<PolyListData*> plists;
		std::string materialData;
		std::string modelPath;

		bool loadDrawable(const std::string & path);

		// Aplica trx a los polyList
		void applyTransform(const float4x4& trx);

		~DrawableData();
	};

	struct SceneObject {
		float4x4 worldTransform = float4x4::Identity();
		DrawableData * drawable = nullptr;
		std::string name;

		~SceneObject() {
			if (drawable) {
				delete drawable;
			}
		}
	};

}
