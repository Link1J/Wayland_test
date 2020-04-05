#include "Wayland.h"

Wayland::Window window;

int main()
{
	Wayland::Window::SetupStatics();

	window.Init(320, 200, 0, 0);

	while(window.IsOpen())
	{
		window.Poll();
	}
}