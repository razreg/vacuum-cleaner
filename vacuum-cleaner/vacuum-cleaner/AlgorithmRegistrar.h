#ifndef __ALGORITHM_REGISTRAR__H_
#define __ALGORITHM_REGISTRAR__H_

#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "Common.h"

class AlgorithmRegistrar {

	class LibHandle {
		void* handle;
	public:
		LibHandle(void* handle) : handle(handle) {};
		~LibHandle() {
			if (handle != NULL) dlclose(handle);
		};
		LibHandle(const LibHandle&) = delete;
		LibHandle& operator=(const LibHandle&) = delete;
	};

	static Logger logger;

	list<function<unique_ptr<AbstractAlgorithm>()>> algorithmFactories;
	list<LibHandle> dlibs;
	list<string> algorithmNames;
	
	void registerAlgorithm(function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
		instance.algorithmFactories.push_back(algorithmFactory);
	};
	
	void setNameForLastAlgorithm(const string& algorithmName) {
		if (algorithmFactories.size() - 1 != algorithmNames.size()) {
			throw runtime_error("Corrupted algorithm list - the amounts of algorithm factories and algorithm names don't match");
		}
		algorithmNames.push_back(algorithmName);
	};

	static AlgorithmRegistrar instance;

	AlgorithmRegistrar() {}; // No one can instantiate from outside of this class (Singleton)

	~AlgorithmRegistrar() {
		algorithmFactories.clear(); // must happen before dlibs are destroyed.
	};

public:

	friend class AlgorithmRegistration;
	
	enum { ALGORITHM_REGISTERED_SUCCESSFULY = 0, FILE_CANNOT_BE_LOADED = -1, NO_ALGORITHM_REGISTERED = -2 };

	AlgorithmRegistrar(const AlgorithmRegistrar&) = delete;

	AlgorithmRegistrar& operator=(const AlgorithmRegistrar&) = delete;

	int loadAlgorithm(const string& path, const string& so_file_name_without_so_suffix);

	list<unique_ptr<AbstractAlgorithm>> getAlgorithms() const {
		list<unique_ptr<AbstractAlgorithm>> algorithms;
		for (auto algorithmFactoryFunc : algorithmFactories) {
			algorithms.push_back(algorithmFactoryFunc());
		}
		return algorithms;
	}

	const list<string>& getAlgorithmNames() const {
		return algorithmNames;
	}

	size_t size() const {
		return algorithmFactories.size();
	}

	static AlgorithmRegistrar& getInstance() {
		return instance;
	}

	void clear() {
		algorithmNames.clear();
		algorithmFactories.clear();
	};
};

#endif // __ALGORITHM_REGISTRAR__H_
