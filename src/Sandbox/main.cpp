#include "pch.h"

#include <windows.h>
#include "main.h"

#include "Tests.h"
#include "Benchmarks/Vector4Benchmark.hpp"

#define NO_VECTORIZE_FUNC attribute((optimize("no-tree-vectorize,no-tree-slp-vectorize"))) 
int WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	Console::InitConsol();
	
	Vector4Benchmark vecBenchmark;
	
	vecBenchmark.Run();
	vecBenchmark.DisplayResults();

	int a;
	std::cin >> a;
	
	Console::DeleteConsol();
	return 0;
}
