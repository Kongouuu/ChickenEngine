#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace ChickenEngine
{
	class CHICKEN_API GameLayer
	{
		public:
			GameLayer() {}
			virtual ~GameLayer() = default;

			virtual void LoadScene(){}
			virtual void Update() {}
			virtual void ImGuiRender() {}
			virtual void OnEvent(Event& event) {}
	};
}