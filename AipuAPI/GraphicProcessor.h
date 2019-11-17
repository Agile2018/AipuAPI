#ifndef GraphicProcessor_h
#define GraphicProcessor_h

#include <iostream>
#include "Format.h"
#include <cuda_runtime.h>
#include <helper_cuda.h>


using namespace std;

class GraphicProcessor
{
public:
	GraphicProcessor();
	~GraphicProcessor();
	string GetDescription() {
		return description;
	}
	bool ThereIsGraphicProcessor();
private:
	string description;
	Format* format = new Format();
};


#endif // !GraphicProcessor_h

