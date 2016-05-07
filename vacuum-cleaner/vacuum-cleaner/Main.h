#ifndef __MAIN__H_
#define __MAIN__H_

#include <dlfcn.h>
#include <cctype>

#include "Simulator.h"
#include "House.h"
#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "AlgorithmRegistrar.h"

using namespace std;
namespace fs = boost::filesystem;

// exit codes
const int SUCCESS = 0;
const int INTERNAL_FAILURE = 1;
const int INVALID_ARGUMENTS = 2;
const int INVALID_CONFIGURATION = 3;
const int INVALID_SCORE_FORMULA = 4;
const int INVALID_HOUSES = 5;
const int INVALID_ALGORITHMS = 6;

const string SCORE_FORMULA_PLACEHOLDER = "&%$+#@(*-6~78)i!dfa4//=";
const char* SCORE_FORMULA_METHOD_NAME = "calc_score";

Logger logger = Logger("Main");

string getCurrentWorkingDirectory();

bool parseArgs(int argc, char** argv, string& configPath, string& housesPath, string& algorithmsPath,
	string &scoreFormula, size_t& threads);

void trimString(string& str) {
	size_t first = str.find_first_not_of(' ');
	size_t lastS = str.find_last_not_of(' ');
	size_t lastR = str.find_last_not_of('\r');
	size_t lastN = str.find_last_not_of('\n');
	size_t last = min(lastS, min(lastR, lastN));
	str = str.substr(first, (last - first + 1));
}

bool isDirectory(fs::path& dir) {
	return fs::exists(dir) && fs::is_directory(dir);
}

void printStringVector(vector<string>& vec);

class MainHelper {

	static const string usage;
	static AlgorithmRegistrar& registrar;

	void* libHandle = NULL;
	vector<fs::path> housePathVector;
	map<string, int> configMap;
	ScoreFormula scoreFormula = NULL;
	size_t threads;

	vector<string> algorithmErrors;

	bool isConfigMapValid();

	void populateConfigMap(ifstream& configFileStream);

public:

	~MainHelper() {
		if (libHandle != NULL) {
			dlclose(libHandle);
		}
	};

	bool loadScoreFormula(const string& scoreFormulaPath);

	bool loadConfiguration(const string& configFileDir);

	bool loadAlgorithms(const string& algorithmsPath);

	bool getHousePaths(const string& housesPath);

	void setThreads(int threads) {
		this->threads = threads;
	};

	int setConfiguration(string& configPath);

	int setScoreFormula(string& scoreFormulaPath);

	int setAlgorithms(string& algorithmsPath);

	int setHousePaths(string& housesPath);

	int runSimulator();

	static string getUsage() {
		return usage;
	};
};

#endif //__MAIN__H_