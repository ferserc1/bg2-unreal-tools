#pragma once

// Una clase donde se añaden scene_objects. Esta clase es la encargada de almacenar el estado de la matriz de modelado actual
// de forma que tenga funciones push/pop y en un momento dado tenga la matriz global del nodo actual. A partir de aquí, cuando se
// encuentre un objeto drawable, se llamará a una función para que lo añada, junto con su matriz, a la lista de scene_objects
// El resultado será una lista plana de scene_objects con su transformación global.
// El sistema de coordenadas será el de bg2, y cuando se vaya a cargar la escena se hará:
//		1 - cambio de coordenadas
//		2 - aplicar escala

#include "bg2tools/scene_object.hpp"

namespace bg2tools {

	class Bg2Scene {
	public:
		~Bg2Scene() {
			for (auto so : _sceneObjects) {
				delete so;
			}
			_sceneObjects.clear();
		}

		inline const std::string& currentName() const {
			return _currentName;
		}

		inline void setCurrentName(const std::string& name) {
			_currentName = name;
		}

		inline const float4x4 & currentMatrix() const {
			return _currentMatrix;
		}

		inline void multMatrix(const float4x4& mat) {
			_currentMatrix = _currentMatrix * mat;
		}

		inline void pushMatrix() {
			_matrixStack.push_back(_currentMatrix);
		}

		inline void popMatrix() {
			_currentMatrix = _matrixStack.back();
			_matrixStack.pop_back();
		}

		inline void setCurrentDrawable(DrawableData* drw) {
			_currentDrawable = drw;
		}

		inline void tryAddNode() {
			if (_currentDrawable) {
				auto so = new SceneObject();
				so->worldTransform = _currentMatrix;
				so->drawable = _currentDrawable;
				so->name = _currentName;
				_sceneObjects.push_back(so);
				_currentDrawable = nullptr;
			}
		}

		inline const std::vector<SceneObject*> sceneObjects() const {
			return _sceneObjects;
		}

		inline SceneObject* currentObject() {
			if (_sceneObjects.size() > 0) {
				return _sceneObjects.back();
			}
			else {
				return nullptr;
			}
		}

	protected:
		float4x4 _currentMatrix = float4x4::Identity();
		std::string _currentName;
		DrawableData * _currentDrawable = nullptr;
		std::vector<float4x4> _matrixStack;
		std::vector<SceneObject*> _sceneObjects;
	};
}

