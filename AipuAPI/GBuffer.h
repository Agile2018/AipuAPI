#ifndef GBUFFER_H
#define GBUFFER_H

#include <vector>

class GBuffer
{
public:
	GBuffer(char* data, int size);
	~GBuffer();
	std::vector<unsigned char> GetBuffer() {
		return frameBuffer;
	}

private:
	std::vector<unsigned char> frameBuffer;
};


#endif // !GBUFFER_H

