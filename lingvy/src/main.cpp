#include <string>

#include "instance.h"

int main(int argc, char* argv[]) {
	Instance instance(argv[0]);

#ifdef _DEBUG
	instance.SetInputFile("inputfile.txt");
#else
	if (argc > 1) instance.SetInputFile(argv[1]);
#endif // _DEBUG

	return instance.Run();
}