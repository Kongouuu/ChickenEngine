#pragma once

#define CONCAT2(X, Y) X##Y
#define CONCAT(X, Y) CONCAT2(X, Y)
#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)
#define WIDEN(X) CONCAT(L, STRINGIFY(X))

namespace ChickenEngine
{
	enum EVertexLayout
	{
		POS_NORM_COL = 0,
		POS_NORM_TEX, // mainly use this
		POS_NORM_COL_TEX,
		POS_NORM_TAN_TEX,
		POS_NORM_TAN_COL_TEX,
		NONE
	};

	enum ETextureType
	{
		TEXTURE2D = 0,
		TEXTURE3D
	};


	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw std::exception();
		}
	}
}