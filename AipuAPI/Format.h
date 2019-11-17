#ifndef Format_h
#define Format_h

#include <string>
#include <cstdarg>
#include <memory>

using namespace std;

class Format
{
public:
	Format();
	~Format();
	string FormatString(const char* format, ...);

private:

};


#endif // !Format_h

