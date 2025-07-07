#include <stdio.h>

void WriteVersion()
{
#ifndef __WIN32__
	FILE* fp = fopen("VERSION.txt", "w");

	if (fp)
	{
		fprintf(fp, "__GAME_VERSION__: %s\n", __GAME_VERSION__);
		fprintf(fp, "%s@%s:%s\n", "ZGFtcw==", __HOSTNAME__, __PWD__);
		fclose(fp);
	}
#endif
}
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
