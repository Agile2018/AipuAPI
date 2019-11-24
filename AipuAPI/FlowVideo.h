#ifndef FLOWVIDEO_H
#define FLOWVIDEO_H

#define NUM_TRACKED_OBJECTS	5
#define COORDINATES_X_ALL_IMAGES 20
#define NUM_COORDINATES_X_IMAGE  4

#include <atomic>
#include <gst/gst.h>
#include <gst/app/app.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "ErrorFaceLib.h"

class FlowVideo
{
public:
	FlowVideo();
	~FlowVideo();

	void CaptureFlow(int optionFlow);

	void LoadConfiguration(string nameFile);	

	void SetMinEyeDistance(int value) {
		minEyeDistance = value;
	}

	void SetMaxEyeDistance(int value) {
		maxEyeDistance = value;
	}

	void SetFaceConfidenceThresh(int value) {
		faceConfidenceThresh = value;
	}

	void SetRefreshInterval(int value);

	void SetSequenceFps(int value);

	void SetIpCamera(string ip) {
		ipCamera = ip;
	}

	void SetDeviceVideo(string device) {
		deviceVideo = device;
	}

	void SetVideoScaleMethod(int value) {
		videoScaleMethod = value;
	}

	void SetWidthFrame(int value) {
		widthFrame = value;
	}

	void SetHeightFrame(int value) {
		heightFrame = value;
	}

	void SetFileVideo(string file) {
		fileVideo = file;
	}

	void SetNameWindow(string name) {
		nameWindow = name;
	}

	void SetFlagFlow(bool flag) {
		flagFlow = flag;
	}

	void SetClient(int value);

	void SetIsRegister(bool option);

	void ShowWindow(int option);

	void ReloadRecognitionFace();

	void SetConfigurationDatabase();

	void RecognitionFaceFiles(string file, int client);

	void SetIsFinishLoadFiles(bool value);
	bool GetIsFinishLoadFiles();
	void ResetLowScore();
	int GetCountLowScore();
	void ResetCountNotDetect();
	int GetCountNotDetect();
	void SetDeepTrack(string value);
	void ResetCountRepeatUser();
	int GetCountRepeatUser();
	Rx::subject<string> errorSubject;
	Rx::observable<string> observableError = errorSubject.get_observable();

	Rx::subject<string> userDetected;
	Rx::observable<string> observableUserJSON = userDetected.get_observable();

private:
	Rx::subscriber<string> shootError = errorSubject.get_subscriber();
	Rx::subscriber<string> shootUserJSON = userDetected.get_subscriber();
	int minEyeDistance = 20;          // minimal eye distance in input image
	int maxEyeDistance = 200;         // maximal eye distance in input image
	int faceConfidenceThresh = 450;         // face detection confidence threshold		
	string ipCamera;
	string fileVideo;
	string deviceVideo = "/dev/video0";
	string deepTrack = "true";
	int videoScaleMethod = 1; //3 1
	int widthFrame = 640;
	int heightFrame = 480;	
	string nameWindow = "frame";
	bool flagFlow = false;	

	const int SWP_NOSIZE = 0x1;
	const int SWP_NOMOVE = 0x2;
	const int SWP_NOACTIVATE = 0x10;
	const int wFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE;
	const int HWND_TOPMOST = -1;
	const int HWND_BOTTOM = 1;

	ErrorFaceLib* error = new ErrorFaceLib();
	
	void InitITracking();
	void TerminateITracking();
	gchar* DescriptionFlow(int optionFlow);
	void ObserverEvent();
	
};


#endif // !FLOWVIDEO_H

