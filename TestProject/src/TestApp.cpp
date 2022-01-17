#include <ChickenEngine.h>

class TestGameLayer : public ChickenEngine::GameLayer
{
public:
	virtual void LoadScene() override
	{
		LOG_TRACE("Enter Load Scene");
		// Set Camera start pos
		ChickenEngine::SceneManager::GetCamera().SetPosition({ 0.0,10.0,-30.0 });
		ChickenEngine::SceneManager::GetCamera().UpdateViewMatrix();


		//// Load Texture
		//std::string texPath = ChickenEngine::FileHelper::GetTexturePath("timg.jpg");
		//ChickenEngine::ResourceManager::LoadTexture(texPath, "tutou");
		//
		//// Load Objects

		//// 1 Cube
		//ChickenEngine::Mesh cube = ChickenEngine::MeshManager::GenerateBox();
		//cube.AddDiffuseTexture(ChickenEngine::ResourceManager::GetTexture("tutou"));
		//ChickenEngine::SceneManager::CreateRenderObject(cube, std::string("cube"), { 2.0,2.0, 2.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0,1.0 }, {0.0,0.0,0.0,0.0}, 0.15, 0.04);

		//// 2 Plane
		//ChickenEngine::Mesh plane = ChickenEngine::MeshManager::GeneratePlane();
		//ChickenEngine::SceneManager::CreateRenderObject(plane, std::string("plane"), { 0.0,-1.5,0.0 }, { 0.0,0.0,0.0 }, { 3.0,1.0,3.0 }, { 1.0,1.0,1.0,1.0 }, 0.15, 0.04);

		// Load model
		ChickenEngine::Model m(ChickenEngine::FileHelper::GetModelPath("sponza/sponza.obj"));
		std::shared_ptr<ChickenEngine::RenderObject> ro1 = ChickenEngine::SceneManager::CreateRenderObject(m, std::string("sponza"), { 0.0,0.0,0.0 }, { 0.0,180.0,0.0 }, { 0.1, 0.1, 0.1 }, { 0.0,0.0,0.0,0.0 }, 0.15, 0.04);
	}

	// not used yet
	virtual void Update() {}
	virtual void ImGuiRender() {}
	virtual void OnEvent(ChickenEngine::Event& event) {}
private:

};

class TestApp : public ChickenEngine::Application
{
public:
	TestApp()
	{
		gameLayer = new TestGameLayer();
	}

	~TestApp()
	{

	}
};

ChickenEngine::Application* ChickenEngine::CreateApplication()
{
	return new TestApp();
}
