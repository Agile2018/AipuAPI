#include "GBuffer.h"
GBuffer::GBuffer(char* data, int size)
{
	frameBuffer.assign(data, data + size);
}

GBuffer::~GBuffer()
{
	frameBuffer.clear();
}