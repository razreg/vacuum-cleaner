#include "AlgorithmRegistrar.h"

using namespace std;

Logger AlgorithmRegistrar::logger = Logger("Registrar");
AlgorithmRegistrar AlgorithmRegistrar::instance;

// filename is the name of the file without the extension (only stem)
int AlgorithmRegistrar::loadAlgorithm(const string& path, const string& filename) {
	size_t size = instance.size();

	void* dlib = dlopen(path.c_str(), RTLD_NOW);
	if (dlib == NULL) {
		if (logger.debugEnabled()) logger.debug("Failed to load file. Details: " + string(dlerror()));
		/* TODO errors.push_back(dir_iter->path().filename().string() +
			": file cannot be loaded or is not a valid.so");*/
		return FILE_CANNOT_BE_LOADED;
	}
	dlibs.push_back(dlib);

	if (instance.size() == size) {
		return NO_ALGORITHM_REGISTERED; // no algorithm registered
	}
	instance.setNameForLastAlgorithm(filename);
	return ALGORITHM_REGISTERED_SUCCESSFULY;
}