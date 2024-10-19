#include "Viewer.hpp"

namespace
{
	constexpr size_t kWidth	 = 1920;
	constexpr size_t kHeight = 1080;
}

int main(int argc, char* argv[])
{
	Viewer viewer(kWidth, kHeight);
	viewer.view("Assets/Scene.yaml");
}
