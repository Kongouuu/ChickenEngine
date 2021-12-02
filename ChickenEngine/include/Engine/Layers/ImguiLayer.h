#pragma once
#include "Engine/Layer.h"
#include <Engine/Events/MouseEvent.h>
#include <Engine/Events/KeyEvent.h>
#include <Engine/Events/ApplicationEvent.h>
namespace ChickenEngine
{
	class CHICKEN_API ImguiLayer :public Layer
	{
	public:
		ImguiLayer();
		~ImguiLayer();

		virtual void OnAttach() override;
		virtual void OnDetatch() override;
		virtual void OnUpdate() override;
		virtual void OnDraw() override;
		virtual void OnImguiRender() override;
		virtual void OnEvent(Event& event) override;

		void Begin();
		void End();
	protected:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);
	};


}
