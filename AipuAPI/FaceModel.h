#ifndef FaceModel_h
#define FaceModel_h

#define BATCH_SIZE	5
#define BATCH_TOTAL_SIZE 30
#define EMPTY_FACE	0
#define TRACKED_OBJECTS	5
#define SIZE_COORDINATES 20
#define NUMBER_COORDINATES_IMAGES  4

#include "ConfigurationIFace.h"
#include "ErrorFaceLib.h"
#include "Molded.h"

class FaceModel
{
public:
	FaceModel();
	~FaceModel();
	void SetNameFileImage(string name) {
		nameFileImage = name;
	}

	void SetNameDirectory(string name) {
		nameDirectory = name;
	}

	void SetSequenceFps(int fps) {
		sequenceFps = fps;
		timeDeltaMs = 1000 / sequenceFps;
	}

	void ResfreshBetweenFrame(int refresh) {
		refreshInterval = refresh;
	}

	void ResetLowScore() {
		countLowScore = 0;
	}

	int GetCountLowScore() {
		return countLowScore;
	}

	void ResetCountNotDetect() {
		countNotDetect = 0;
	}

	int GetCountNotDetect() {
		return countNotDetect;
	}

	void SetIsFinishLoadFiles(bool value) {
		isFinishLoadFiles = value;
	}

	bool GetIsFinishLoadFiles() {
		return isFinishLoadFiles;
	}

	void Terminate();
	void TerminateTracking();
	int ModelByBatch(std::vector<std::vector<unsigned char>> bufferOfImagesBatch,
		std::vector<int> clients);
	int ModelOneToOne(vector<unsigned char> buffer, int client);
	void RecognitionFaceFiles(string file, int client);
	void FastOnlyDetect(vector<unsigned char> buffer);
	void Tracking(); //vector<unsigned char> buffer
	void InitTracking(); 
	void InitHandle();
	void TerminateHandle();
	void ProcessFaceTracking(void* faceTracking, int client);
	ConfigurationIFace* configuration = new ConfigurationIFace();
	Rx::subject<Molded*> templateImage;
	Rx::observable<Molded*> observableTemplate = templateImage.get_observable();
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
	Rx::subject<float*> subjectCoordinates;
	Rx::observable<float*> observableCoordinates = subjectCoordinates.get_observable();

private:
	const string nameFileCropImage = "imgCrop.png";
	ErrorFaceLib* error = new ErrorFaceLib();
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	Rx::subscriber<float*> shootCoordinates = subjectCoordinates.get_subscriber();
	Rx::subscriber<Molded*> templateOut = templateImage.get_subscriber();	
	std::vector<Molded*> modelsDetected;
	float imageCoordinatesFollowed[SIZE_COORDINATES];
	bool isFinishLoadFiles = true;
	string nameFileImage;
	string nameDirectory;
	int sequenceFps = 30;
	int timeDeltaMs = 1000 / 25; //sequenceFps
	int refreshInterval = 200; // time interval (in ms) between frames where refresh of face detect will be done	
	void* faceHandlerTracking;
	void* objectHandler;
	void* objects[TRACKED_OBJECTS];
	void* faceHandlerGlobal;
	int countFrameTracking = 0;
	int countLowScore = 0;
	int countNotDetect = 0;
	void FaceCropImage(void* face, Molded* model);
	int DetectByBatch(void* facesDetected[BATCH_TOTAL_SIZE],
		std::vector<std::vector<unsigned char>> bufferOfImagesBatch, std::vector<int> clients);
	void GetBatchModels(int countFacesDetected, void* facesDetected[BATCH_TOTAL_SIZE]);
	void CreateTemplate(void* face, Molded* model, int client);
	int GetOneModel(unsigned char* rawImage, int width, int height, int client);
	void FastDetect(unsigned char* rawImage, int width, int height);
	void ObserverError();
	void GetFaceCropRectangle(void* face);
	unsigned char* LoadImage(vector<unsigned char> buffer, int *width, int *height);
	unsigned char* LoadFileImage(string image, int *width, int *height, int *length);
	void BuildCoordinates(float x, float y, float width, float height, int indexTracked);
	void ClearCoordinatesImage(int indexTracked);
	std::string IntToStr(int num);
};


#endif // !FaceModel_h

