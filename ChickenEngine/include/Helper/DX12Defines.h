#pragma once

#define CONCAT2(X, Y) X##Y
#define CONCAT(X, Y) CONCAT2(X, Y)
#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)
#define WIDEN(X) CONCAT(L, STRINGIFY(X))

namespace ChickenEngine
{
	enum ETextureDimension
	{
		TEXTURE2D = 0,
		TEXTURE3D
	};

	enum EBasicShapeType
	{
		CUBE = 0,
		PLANE,
		SPHERE
	};


	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw std::exception();
		}
	}
}