#ifndef Innovatrics_h
#define Innovatrics_h

#include "ErrorFaceLib.h"
#include "GraphicProcessor.h"
#include "AFaceAPI.h"
//#include "idkit.h"

class Innovatrics
{
public:
	Innovatrics();
	~Innovatrics();
	void InitLibrary();
	void InitLibraryIdentify();
	void SetIsGraphicProcessor(bool processor) {
		isGraphicProcessor = processor;
	}
	void SetParamsLibrary();
	void Terminate();

	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();

private:
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	ErrorFaceLib* error = new ErrorFaceLib();
	bool InitParamsGraphicProcessor();
	bool isGraphicProcessor = false;
	void ObserverError();
	GraphicProcessor* graphicProcessor = new GraphicProcessor();
	void VerifyProcessorGraphic();
};


#endif // !Innovatrics_h

