#include "pch.h"
#include "Engine/LayerStack.h"

ChickenEngine::LayerStack::LayerStack()
{
	mLayerInsert = mLayers.begin();
}

ChickenEngine::LayerStack::~LayerStack()
{
}

void ChickenEngine::LayerStack::PushLayer(std::shared_ptr<Layer> layer)
{
	mLayerInsert = mLayers.emplace(mLayerInsert, layer);
}

void ChickenEngine::LayerStack::PushOverlay(std::shared_ptr<Layer> overlay)
{
	mLayers.emplace_back(overlay);
}

void ChickenEngine::LayerStack::PopLayer(std::shared_ptr<Layer> layer)
{
	auto it = std::find(begin(), end(), layer);
	if (it != end())
	{
		mLayers.erase(it);
		mLayerInsert--;
	}
}

void ChickenEngine::LayerStack::PopOverlay(std::shared_ptr<Layer> overlay)
{
	auto it = std::find(begin(), end(), overlay);
	if (it != end())
	{
		mLayers.erase(it);
	}
}
