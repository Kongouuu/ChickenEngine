#include <ChickenEngine.h>

class TestApp : public ChickenEngine::Application
{
public:
	TestApp()
	{

	}

	~TestApp()
	{

	}
};

ChickenEngine::Application* ChickenEngine::CreateApplication()
{
	return new TestApp();
}
