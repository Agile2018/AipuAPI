#ifndef Either_H
#define Either_H
#include <iostream>

using namespace std;

class Either
{
public:
	Either();
	~Either();
	void SetCode(int codeInput);
	void SetLabel(string labelInput);
	int GetCode();
	string GetLabel();
	void Clear();

private:
	int code;
	string label;

};

#endif // !Either_H

