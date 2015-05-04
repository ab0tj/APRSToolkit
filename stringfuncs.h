#ifndef __NS_STD
#define __NS_STD
using namespace std;
#endif

#ifndef __STRINGFUNCS_INC
#define __STRINGFUNCS_INC
// INCLUDES
#include <string>
#include <cstdlib>

// FUNCTIONS
void find_and_replace(string&, const string&, const string&);
string get_call(string);
unsigned char get_ssid(string);
#endif