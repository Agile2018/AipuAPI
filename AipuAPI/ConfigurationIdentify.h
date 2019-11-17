#ifndef ConfigurationIdentify_h
#define ConfigurationIdentify_h

#include "Configuration.h"
#include "ErrorFaceLib.h"

class ConfigurationIdentify : public Configuration
{
public:
	ConfigurationIdentify();
	~ConfigurationIdentify();
	void ParseJSONToObject();
	void ParseMapToJSON();
	string ParseMapToJSONForIdentify();

	void SetMinEyeDistance(int minDistance) {
		minEyeDistance = minDistance;
	}
	void SetMaxEyeDistance(int maxDistance) {
		maxEyeDistance = maxDistance;
	}

	void SetIdentificationSpeed(int speed) {
		identificationSpeed = speed;
	}

	void SetFaceDetectionForced(int forced) {
		faceDetectionForced = forced;
	}

	int GetIdentificationSpeed() {
		return identificationSpeed;
	}

	int GetFaceDetectionForced() {
		return faceDetectionForced;
	}

	int GetMinEyeDistance() {
		return minEyeDistance;
	}

	int GetMaxEyeDistance() {
		return maxEyeDistance;
	}

	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
private:
	const int MIN_EYE_DISTANCE = 25;
	const int MAX_EYE_DISTANCE = 200;
	const int IDENTIFICATION_SPEED = 5;
	const int FACE_DETECTION_FORCED = 2;
	const string PARAMS = "Params";
	const string IDENTIFICATIONSPEED = "A_IdentificationSpeed";
	const string FACEDETECTIONFORCED = "A_FaceDetectionForced";
	const string CONFIGURATION = "configuration";
	const string IDENTIFY_CONFIGURATION = "identify_configuration";
	const string AMINEYE = "A_MinEyeDist";
	const string AMAXEYE = "A_MaxEyeDist";

	int minEyeDistance = MIN_EYE_DISTANCE;
	int maxEyeDistance = MAX_EYE_DISTANCE;
	int identificationSpeed = IDENTIFICATION_SPEED;
	int faceDetectionForced = FACE_DETECTION_FORCED;

	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	ErrorFaceLib* error = new ErrorFaceLib();
	void SetValueJSONToConfiguration();
	void ObserverError();
};



#endif	