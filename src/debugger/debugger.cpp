#include "mi/MainSystem/MainSystem.h"
#include "application.h"

#include <exception>

#ifdef MI_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include "miGUI.h"
#include "miGUILoader.h"

int main(int argc, char* argv[])
{
	MG_LIB_HANDLE gui_lib = mgLoad();
	if (!gui_lib)
	{
		MessageBoxA(0, "Can't load migui.dll", "Error", MB_OK);
		return 0;
	}

	const char* videoDriverType = "gs.d3d11.dll"; // for example read name from .ini
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
	if (app->OnCreate(videoDriverTypeStr.data()))
	{
		app->MainLoop();
	}

	delete app;

	mgUnload(gui_lib);

	return 0;
}