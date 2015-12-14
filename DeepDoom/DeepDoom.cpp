// DeepDoom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

int main()
{
	BOOL result = SetProcessDPIAware();

	if (result == FALSE)
	{
		std::cout << "Could not make the application DPI-aware, wrong window sizes could be returned.\n";
	}

    return 0;
}

