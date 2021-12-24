#include "pch.h"

#include "Engine/Scene/SceneManager.h"

namespace ChickenEngine
{
	using namespace DirectX;

	UINT SceneManager::renderObjectCount = 0;
	std::shared_ptr<RenderObject> SceneManager::GetRenderObject(UINT id)
	{
		std::deque<std::shared_ptr<RenderObject>>& mAllRO = GetAllRenderObjects();
		for (auto& ro : mAllRO)
		{
			if (ro->renderObjectID == id)
				return ro;
		}
		return nullptr;
	}

	std::shared_ptr<RenderObject> SceneManager::CreateRenderObject(std::string n)
	{
		std::shared_ptr<RenderObject> renderObject = std::make_shared<RenderObject>(n);
		renderObject->renderObjectID = renderObjectCount++;
		instance().mRenderObjects.push_back(renderObject);
		return renderObject;
	}

	std::shared_ptr<RenderObject> SceneManager::CreateRenderObject(std::string n, XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s, XMFLOAT4 c, float ro, float me)
	{
		std::shared_ptr<RenderObject> renderObject = std::make_shared<RenderObject>(n,p,r,s,c,ro,me);
		renderObject->renderObjectID = renderObjectCount++;
		instance().mRenderObjects.push_back(renderObject);
		return renderObject;
	}

	std::vector<BYTE> SceneManager::GetSceneData(int width, int height)
	{
		Camera& camera = instance().mCamera;
		PassConstants pc;
		XMMATRIX view = camera.GetView();
		XMMATRIX proj = camera.GetProj();
		XMMATRIX viewProj = XMMatrixMultiply(view, proj);
		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
		XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
		XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

		XMStoreFloat4x4(&pc.View, XMMatrixTranspose(view));
		XMStoreFloat4x4(&pc.InvView, XMMatrixTranspose(invView));
		XMStoreFloat4x4(&pc.Proj, XMMatrixTranspose(proj));
		XMStoreFloat4x4(&pc.InvProj, XMMatrixTranspose(invProj));
		XMStoreFloat4x4(&pc.ViewProj, XMMatrixTranspose(viewProj));
		XMStoreFloat4x4(&pc.InvViewProj, XMMatrixTranspose(invViewProj));
		pc.EyePosW = camera.GetPosition3f();
		pc.RenderTargetSize = XMFLOAT2((float)width, (float)height);
		pc.InvRenderTargetSize = XMFLOAT2(1.0f /width, 1.0f /height);
		pc.NearZ = 1.0f;
		pc.FarZ = 1000.0f;

		pc.DirLight = instance().mDirLight.data;

		BYTE data[sizeof(pc)];
		memcpy(&data, &pc, sizeof(pc));
		std::vector<BYTE> dataVector(data, data + sizeof(pc));
		return dataVector;
	}

	void SceneManager::UpdateDirLightDirection()
	{
		XMFLOAT3 initialDir = XMFLOAT3(0.0, -1.0, 0.0);
		XMFLOAT3 rotation = instance().mDirLight.Rotation;
		rotation.x *= (DirectX::XM_PI / 180.0f);
		rotation.y *= (DirectX::XM_PI / 180.0f);
		rotation.z *= (DirectX::XM_PI / 180.0f);
		XMMATRIX transform = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		XMVECTOR dir = XMLoadFloat3(&initialDir);
		XMVECTOR newDir = XMVector3Transform(dir, transform);
		XMStoreFloat3(&instance().mDirLight.data.Direction, newDir);
	}

}
