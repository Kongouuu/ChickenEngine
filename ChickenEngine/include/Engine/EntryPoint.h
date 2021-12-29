#pragma once

extern ChickenEngine::Application* ChickenEngine::CreateApplication();

int main(int argc, char** argv)
{
	ChickenEngine::Log::Init();

	auto App = ChickenEngine::CreateApplication();
	App->Init();
	App->Run();
	delete App;

	return 0;
}

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
//    PSTR cmdLine, int showCmd)
//{
//	ChickenEngine::Log::Init();
//
//	LOG_INFO("test");
//	LOG_WARN("2");
//	auto App = ChickenEngine::CreateApplication(hInstance);
//	App->Run();
//	delete App; 
//	return 0;
//}
//
