#pragma once

// Build dll with above macro, build exe with below
// 在preprocessor里如果有添加，就是1，没有就是0
#ifdef _BUILD_DLL
#define CHICKEN_API __declspec(dllexport)
#else
#define CHICKEN_API __declspec(dllimport)
#endif

#define BIT(x) (1 << x)
#define BIND_FN(x) std::bind(&x, this, std::placeholders::_1)
