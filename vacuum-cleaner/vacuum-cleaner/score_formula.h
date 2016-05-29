#include <map>
#include <string>

using namespace std;

extern "C" int calc_score(const map<string, int>& score_params);

int getValue(const map<string, int>& score_params, string paramName, bool& valid);