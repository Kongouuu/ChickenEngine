#pragma once
#include "pch.h"
#include "Core.h"
#include "Layer.h"

namespace ChickenEngine
{
	class CHICKEN_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		// Overlay is sth to render always last (always after layer)

		void PushLayer(std::shared_ptr<Layer> layer);
		void PushOverlay(std::shared_ptr<Layer> overlay);
		void PopLayer(std::shared_ptr<Layer> layer);
		void PopOverlay(std::shared_ptr<Layer> overlay);

		std::vector<std::shared_ptr<Layer>>::iterator begin() { return mLayers.begin(); }
		std::vector<std::shared_ptr<Layer>>::iterator end() { return mLayers.end(); }
	private:
		std::vector<std::shared_ptr<Layer>> mLayers;
		std::vector<std::shared_ptr<Layer>>::iterator mLayerInsert;
	};

}

