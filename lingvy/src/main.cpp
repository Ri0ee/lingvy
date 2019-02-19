#include <string>

#include "instance.h"

int main(int argc, char* argv[]) {
	Instance instance(argv[0]);
	if (argc > 1) instance.SetInputFile(argv[1]);
	return instance.Run();
}