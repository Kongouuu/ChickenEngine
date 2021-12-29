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
				renderer.SetDiffuseTexture(m.renderItemID, m.diffuseMap.id);
				renderer.SetSpecularTexture(m.renderItemID, m.specularMap.id);
				renderer.SetNormalTexture(m.renderItemID, m.normalMap.id);
				renderer.SetHeightTexture(m.renderItemID, m.heightMap.id);
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
		pc.InvRenderTargetSize = XMFLOAT2(1.0f / width, 1.0f / height);
		pc.NearZ = 1.0f;
		pc.FarZ = 1000.0f;

		// light data
		pc.DirLight = instance().mDirLight.data;
		instance().mLightCamera.LookAt(camera.GetPosition3f(), pc.DirLight.Direction, XMFLOAT3(0.0, 1.0, 0.0));
		instance().mLightCamera.UpdateViewMatrix();
		XMMATRIX lView = instance().mLightCamera.GetView();
		XMMATRIX lProj = instance().mLightCamera.GetProj();
		XMMATRIX shadowTransform = XMMatrixMultiply(lView, lProj);
		XMStoreFloat4x4(&pc.ShadowTransform, XMMatrixTranspose(shadowTransform));
		BYTE data[sizeof(pc)];
		memcpy(&data, &pc, sizeof(pc));
		std::vector<BYTE> dataVector(data, data + sizeof(pc));
		return dataVector;
	}

	void SceneManager::UpdateLightCamera()
	{
		auto& sm = instance();
		float farZ = sm.mCamera.GetFarZ();
		XMFLOAT3 dir= sm.GetDirLightDirection();
		XMVECTOR camPos = sm.mCamera.GetPosition();
		XMVECTOR lightCamPos = XMVectorAdd(camPos, XMLoadFloat3(&XMFLOAT3(-dir.x * farZ, -dir.y * farZ, -dir.z * farZ)));

	}

	void SceneManager::UpdateSceneData(int width, int height)
	{
		UpdateDirLightDirection();
		UpdateLightCamera();
		std::vector<BYTE> data = GetSceneData(width , height);
		DX12Renderer::GetInstance().SetPassSceneData(data.data());
	}

	void SceneManager::SetRenderObjectCB(RenderObject& ro)
	{
		/* 因为模型单位不一样会变成普通方块的十倍大，所以先把所有基础的Mesh大小也拉高十倍 */
		/* 然后再这里统一把所有mesh缩小10倍 */
		/* 因为scale变成十分之一，因此position也会同样缩水，这里要乘以10达到平衡 */
		XMMATRIX world;
		ObjectConstants oc;

		// translation
		XMFLOAT3 position = XMFLOAT3(ro.position.x * 10.f, ro.position.y * 10.f, ro.position.z * 10.f);
		// rotation
		XMFLOAT3 rotationPI = ro.rotation;
		rotationPI.x *= (DirectX::XM_PI / 180.0f);
		rotationPI.y *= (DirectX::XM_PI / 180.0f);
		rotationPI.z *= (DirectX::XM_PI / 180.0f);
		// scale
		XMFLOAT3 scale = XMFLOAT3(ro.scale.x / 10.f, ro.scale.y / 10.f, ro.scale.z / 10.f);

		world = XMMatrixTranslationFromVector(XMLoadFloat3(&position)) *
			(XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotationPI)) * XMMatrixScalingFromVector(XMLoadFloat3(&scale)));
		XMStoreFloat4x4(&oc.World, XMMatrixTranspose(world));
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
