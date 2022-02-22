#include "pch.h"

#include "Engine/Scene/SceneManager.h"

namespace ChickenEngine
{
	using namespace DirectX;

	uint32_t SceneManager::renderObjectCount = 0;
	std::shared_ptr<RenderObject> SceneManager::GetRenderObject(uint32_t id)
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

	std::shared_ptr<RenderObject> SceneManager::CreateRenderObject(Mesh m, std::string n, XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s, XMFLOAT4 c, float ro, float me)
	{
		std::shared_ptr<RenderObject> renderObject = std::make_shared<RenderObject>(n, p, r, s, c, ro, me);
		renderObject->mMeshes.emplace_back(m);
		renderObject->renderObjectID = renderObjectCount++;
		instance().mRenderObjects.push_back(renderObject);
		return renderObject;
	}

	std::shared_ptr<RenderObject> SceneManager::CreateRenderObject(Model m, std::string n, XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s, XMFLOAT4 c, float ro, float me)
	{
		std::shared_ptr<RenderObject> renderObject = std::make_shared<RenderObject>(n, p, r, s, c, ro, me);
		renderObject->mMeshes.insert(renderObject->mMeshes.end(), m.mMeshes.begin(), m.mMeshes.end());
		renderObject->renderObjectID = renderObjectCount++;
		instance().mRenderObjects.push_back(renderObject);
		return renderObject;
	}

	void SceneManager::LoadAllRenderObjects()
	{
		DX12Renderer& renderer = DX12Renderer::GetInstance();
		for (auto& ro : instance().mRenderObjects)
		{
			for (auto& m : ro->mMeshes)
			{
				BYTE* data = reinterpret_cast<BYTE*>(m.vertices.data());
				if (m.debug)
				{
					m.renderItemID = renderer.CreateDebugRenderItem(m.vertices.size(), sizeof(Vertex), data, m.GetIndices16());
					continue;
				}
				
				m.renderItemID = renderer.CreateRenderItem(m.vertices.size(), sizeof(Vertex), data, m.GetIndices16(), ro->renderObjectID);
				renderer.SetTexture(m.renderItemID, m.diffuseMap.id, ETextureType::DIFFUSE);
				renderer.SetTexture(m.renderItemID, m.specularMap.id, ETextureType::SPECULAR);
				renderer.SetTexture(m.renderItemID, m.normalMap.id, ETextureType::NORMAL);
				renderer.SetTexture(m.renderItemID, m.heightMap.id, ETextureType::HEIGHT);
			}
			SetRenderObjectCB(*ro);
		}
	}

	void SceneManager::UpdateRenderObjects()
	{
		for (auto& ro : instance().mRenderObjects)
		{
			if (ro->dirty)
			{
				SetRenderObjectCB(*ro);
				ro->dirty = false;
			}
		}
	}

	std::vector<BYTE> SceneManager::GetSceneData(int width, int height)
	{
		Camera& camera = instance().mCamera;
		PassConstants& pc = instance().mPassCB;
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
		pc.InvRenderTargetSize = XMFLOAT2(1.0f / width, 1.0f / height);
		pc.NearZ = 1.0f;
		pc.FarZ = 1000.0f;
		
		// light data
		pc.DirLight = instance().mDirLight.data;


		XMMATRIX lView = instance().mLightCamera.GetView();
		XMMATRIX lProj = instance().mLightCamera.GetProj();
		XMMATRIX shadowTransform = lView * lProj;
		XMStoreFloat4x4(&pc.ShadowTransform, XMMatrixTranspose(shadowTransform));
		BYTE data[sizeof(pc)];
		memcpy(&data, &pc, sizeof(pc));
		std::vector<BYTE> dataVector(data, data + sizeof(pc));
		return dataVector;
	}

	void SceneManager::UpdateLightCamera()
	{
		auto& sm = instance();
		sm.UpdateDirLightPosition();
		XMFLOAT3 dir = sm.GetDirLightDirection();
		XMFLOAT3 pos = sm.mDirLight.Position;

		XMFLOAT3 target = XMFLOAT3(dir.x + pos.x, dir.y + pos.y, dir.z + pos.z);
		sm.mLightCamera.LookAt(pos, target, XMFLOAT3(0.0, 1.0, 0.0));
		sm.mLightCamera.UpdateViewMatrix();
	}

	void SceneManager::UpdateSceneData(int width, int height)
	{
		UpdateDirLightDirection();
		UpdateLightCamera();
		std::vector<BYTE> data = GetSceneData(width , height);
		DX12Renderer::GetInstance().SetPassSceneData(data.data());
	}

	void SceneManager::UpdateRenderSettings()
	{
		auto& sm = instance();
		DX12Renderer::GetInstance().UpdateRenderSettings(sm.mRenderSettings);
	}

	void SceneManager::UpdateDirLightPosition()
	{
		if (mRenderSettings.sm_generateSM == false)
			return;
		float nearDist = mCamera.GetNearZ();
		float farDist = mCamera.GetFarZ();
		float simpleLongestSide = max(farDist-nearDist,max(mCamera.GetFarWindowWidth(), mCamera.GetFarWindowHeight()));

		XMVECTOR up = mCamera.GetUp();
		XMVECTOR right = mCamera.GetRight();

		XMVECTOR centerFar = mCamera.GetPosition() + mCamera.GetLook() * farDist;
		XMVECTOR centerNear = mCamera.GetPosition() + mCamera.GetLook() * nearDist;
		XMVECTOR frustumCenter = (centerFar + centerNear) * 0.5f;

		XMVECTOR dir = XMLoadFloat3(&mDirLight.data.Direction);
		static bool bInit = false;
		if (mDirLight.bAutoPosition)
		{
			XMVECTOR position;
			if (!bInit)
			{
				mDirLight.distFrustumCenter = simpleLongestSide / 2.0;
				mDirLight.offsetViewDir = 0.0;
				position = frustumCenter - dir * (simpleLongestSide / 2.0);
				bInit = true;
			}
			else
			{
				position = frustumCenter - dir * (mDirLight.distFrustumCenter);
				int offset = mDirLight.offsetViewDir - (farDist + nearDist) / 2.0f;
				position = position + mCamera.GetLook() * offset;

			}
			XMStoreFloat3(&mDirLight.Position, position);
		}
	}

	void SceneManager::ToggleRenderObjectVisibility(uint32_t id)
	{
		std::shared_ptr<RenderObject> ro = GetRenderObject(id);
		for (auto& m : ro->mMeshes)
		{
			DX12Renderer::GetInstance().SetVisibility(m.renderItemID, ro->visible);
		}
	}

	void SceneManager::SetRenderObjectCB(RenderObject& ro)
	{
		XMMATRIX world;
		ObjectConstants oc;

		// translation
		XMFLOAT3 position = XMFLOAT3(ro.position.x, ro.position.y, ro.position.z);
		// rotation
		XMFLOAT3 rotationPI = ro.rotation;
		rotationPI.x *= (DirectX::XM_PI / 180.0f);
		rotationPI.y *= (DirectX::XM_PI / 180.0f);
		rotationPI.z *= (DirectX::XM_PI / 180.0f);
		// scale
		XMFLOAT3 scale = XMFLOAT3(ro.scale.x, ro.scale.y, ro.scale.z);

		
		world = XMMatrixScalingFromVector(XMLoadFloat3(&scale))  *
			(XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotationPI)) * XMMatrixTranslationFromVector(XMLoadFloat3(&position)));
		XMStoreFloat4x4(&oc.World, XMMatrixTranspose(world));
		
		XMMATRIX invRotation = XMMatrixTranspose(XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotationPI)));
		XMMATRIX invScale = XMMatrixScaling(1.0f / ro.scale.x, 1.0f / ro.scale.y, 1.0f / ro.scale.z);
		XMStoreFloat4x4(&oc.InvWorld, invScale * XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotationPI)));

		//XMMATRIX invRotation = XMMatrixTranspose(XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotationPI)));
		//XMMATRIX invScale = XMMatrixScaling(1.0f / ro.scale.x, 1.0f / ro.scale.y, 1.0f / ro.scale.z);
		//XMStoreFloat4x4(&oc.InvWorld, XMMatrixTranspose(invRotation * invScale ));

		oc.Roughness = ro.roughness;
		oc.Metallic = ro.metallic;
		oc.Color = ro.color;


		BYTE data[sizeof(ObjectConstants)];
		memcpy(&data, &oc, sizeof(ObjectConstants));

		DX12Renderer::GetInstance().SetObjectCB(ro.renderObjectID, data);
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
