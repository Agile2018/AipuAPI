#include "Either.h"

Either::Either()
{
}

Either::~Either()
{
}

void Either::Clear() {
	code = 0;
	label = "";
}

int Either::GetCode() {
	return code;
}

string Either::GetLabel() {
	return label;
}

void Either::SetCode(int codeInput) {
	code = codeInput;
}

void Either::SetLabel(string labelInput) {
	label = labelInput;
}
