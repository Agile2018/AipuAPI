#ifndef ConfigurationIFace_h
#define ConfigurationIFace_h

#include "Configuration.h"
#include "ErrorFaceLib.h"

class ConfigurationIFace : public Configuration
{
public:
	ConfigurationIFace();
	~ConfigurationIFace();
	void ParseJSONToObject();
	void ParseMapToJSON();

	void SetMinEyeDistance(int minDistance) {
		minEyeDistance = minDistance;
	}
	void SetMaxEyeDistance(int maxDistance) {
		maxEyeDistance = maxDistance;
	}

	void SetMaxDetect(int maxFacesDetect) {
		maxDetect = maxFacesDetect;
	}

	void SetPrecision(int precision) {
		accuracy = precision;
	}

	void SetModeDetect(int mode) {
		modeDetect = mode;
	}	

	void SetExtractionMode(int mode) {
		extractionMode = mode;
	}

	int GetMinEyeDistance() {
		return minEyeDistance;
	}

	int GetMaxEyeDistance() {
		return maxEyeDistance;
	}

	int GetMaxDetect() {
		return maxDetect;
	}

	int GetPrecision() {
		return accuracy;
	}

	int GetModeDetect() {
		return modeDetect;
	}
		
	int GetExtractionMode() {
		return extractionMode;
	}

	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
private:
	const int MAX_DETECT = 1;
	const int MIN_EYE_DISTANCE = 25;
	const int MAX_EYE_DISTANCE = 200;
	const int ACCURACY = 600;
	const int MODE_DETECT = 1;		
	const int EXTRACTION_MODE = 1;

	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	ErrorFaceLib* error = new ErrorFaceLib();
	int minEyeDistance = MIN_EYE_DISTANCE;
	int maxEyeDistance = MAX_EYE_DISTANCE;
	int maxDetect = MAX_DETECT;
	int accuracy = ACCURACY;
	int modeDetect = MODE_DETECT;
	int extractionMode = EXTRACTION_MODE;

	const string PARAMS = "Params";
	const string MAXFACES = "maxfaces";
	const string MINEYE = "mineye";
	const string MAXEYE = "maxeye";
	const string PRECISION = "accuracy";
	const string MODEDETECT = "modedetect";	
	const string EXTRACTIONMODE = "extractionmode";
	const string CONFIGURATION = "configuration";
	const string DETECT_CONFIGURATION = "detect_configuration";

	void SetValueJSONToConfiguration();
	void ObserverError();
};


#endif // !ConfigurationIFace_h

