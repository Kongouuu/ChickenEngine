#include <ChickenEngine.h>

class TestLayer : public ChickenEngine::Layer
{
public:
	TestLayer() : Layer("test") {}

	void OnUpdate() override
	{
		//LOG_INFO("Test layer update");
	}

	void OnEvent(ChickenEngine::Event& e) override
	{
		//LOG_INFO("Test layer event: {0}", e.ToString());
	}

};
class TestApp : public ChickenEngine::Application
{
public:
	TestApp()
	{
		PushLayer(std::make_shared<TestLayer>());
	}

	~TestApp()
	{

	}
};

ChickenEngine::Application* ChickenEngine::CreateApplication()
{
	return new TestApp();
}
