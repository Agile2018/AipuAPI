//Thanks ReneNyffenegger
//https://github.com/ReneNyffenegger/cpp-base64/blob/master/base64.cpp

#ifndef Base64_h
#define Base64_h

#include <iostream>

using namespace std;

class Base64
{
public:
	Base64();
	~Base64();
	string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);

private:
	const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";
	static inline bool is_base64(unsigned char c) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	}
};


#endif // !Base64_h

