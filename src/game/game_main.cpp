#include <string.h>

#include "framework/mgf.h"

#ifdef MG_DEBUG
#pragma comment(lib, "mgfd.lib")
#else
#pragma comment(lib, "mgf.lib")
#endif

int main_game();
int main_modelEditor();

int main(int argc, char* argv[])
{
	int mode = 1;
	for(int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i], "-me") == 0)
		{
			mode = 1;
		}
	}

	switch (mode)
	{
	default:
	case 0:
		return main_game();
	case 1:
		return main_modelEditor();
	}
}