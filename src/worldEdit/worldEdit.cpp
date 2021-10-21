#include "mixer.lib.h"
#include "mixer.gui.h"
#include "application.h"

#include <exception>

int main(int argc, char* argv[])
{
	const char* videoDriverType = "mixer.vd.opengl33.dll"; // for example read name from .ini
	miStringA videoDriverTypeStr = videoDriverType;
	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i], "-vid") == 0)
		{
			++i;
			if (i < argc)
			{
				videoDriverTypeStr = argv[i];
			}
		}
	}

	Application* app = new Application;
	try
	{
		app->OnCreate(videoDriverTypeStr.data());
		app->MainLoop();
	}
	catch (const std::exception& e)
	{
		miLogWriteError("Exception: %s\n", e.what());
	}

	delete app;
	return 0;
}