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

		void applyTransform(const float4x4& trx);

		bool isValid();
	};

	struct DrawableData {
		std::vector<PolyListData*> plists;
		std::string materialData;
		std::string modelPath;

		bool loadDrawable(const std::string & path);

		void applyTransform(const float4x4& trx);

		bool isValid();

		~DrawableData();
	};

	struct SceneObject {
		float4x4 worldTransform = float4x4::Identity();
		DrawableData * drawable = nullptr;
		float3 teleportBox = float3(0.f, 0.f, 0.f);
		std::string name;

		bool isValid() {
			if (drawable) {
				return drawable->isValid();
			}
			return true;
		}

		~SceneObject() {
			if (drawable) {
				delete drawable;
			}
		}
	};

}
