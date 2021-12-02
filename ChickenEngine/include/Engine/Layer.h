#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace ChickenEngine
{
	class CHICKEN_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetatch() {}
		virtual void OnUpdate() {}
		virtual void OnDraw() {}
		virtual void OnEvent(Event& event) {}
		virtual void OnImguiRender(){}

		// Debug use
		inline const std::string& GetName() const { return mName; }
	protected:
		std::string mName;
	};

}

