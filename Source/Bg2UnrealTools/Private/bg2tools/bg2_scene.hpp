#pragma once

// Una clase donde se a�aden scene_objects. Esta clase es la encargada de almacenar el estado de la matriz de modelado actual
// de forma que tenga funciones push/pop y en un momento dado tenga la matriz global del nodo actual. A partir de aqu�, cuando se
// encuentre un objeto drawable, se llamar� a una funci�n para que lo a�ada, junto con su matriz, a la lista de scene_objects
// El resultado ser� una lista plana de scene_objects con su transformaci�n global.
// El sistema de coordenadas ser� el de bg2, y cuando se vaya a cargar la escena se har�:
//		1 - cambio de coordenadas
//		2 - aplicar escala

#include "bg2tools/scene_object.hpp"

namespace bg2tools {

	class Bg2Scene {
	public:
		
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
			_matrixStack.pop_back();
		}

		inline void addDrawable(const DrawableData& drw) {
			_sceneObjects.push_back({
				_currentMatrix,
				drw
			});
		}

		inline const std::vector<SceneObject> sceneObjects() const {
			return _sceneObjects;
		}

	protected:
		float4x4 _currentMatrix;
		std::vector<float4x4> _matrixStack;
		std::vector<SceneObject> _sceneObjects;
	};
}

