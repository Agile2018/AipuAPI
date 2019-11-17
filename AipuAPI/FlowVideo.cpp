#include "FlowVideo.h"
#include "Backrest.h"
#include "GBuffer.h"
#include "ThreadPool.h"
#include <random>
#include <windows.h>
#include <winuser.h>

std::atomic<cv::Mat*> atomicFrame;
std::atomic<GBuffer*> atomicBuffer;
void* objectHandler;
void* faceHandlerTracking;
void* objects[NUM_TRACKED_OBJECTS];
float imageCoordinatesFollowed[COORDINATES_X_ALL_IMAGES];
Backrest* backRest = new Backrest();
int client = 1;
int sequenceFps = 30;                   // fps of video	
int timeDeltaMs = 1000 / sequenceFps;
int refreshInterval = 2000;
int countFrameTracking = 0;
bool flagFirstDetect = false;
bool flagTracking = false;
ThreadPool* pool = new ThreadPool(2);

FlowVideo::FlowVideo()
{
	
}

FlowVideo::~FlowVideo()
{
	pool->shutdown();
	delete backRest;
}

void FlowVideo::ObserverEvent() {
	auto observerErrorManagement = backRest->observableError.map([](Either* either) {
		return either;
	});


	auto subscriptionErrorManagement = observerErrorManagement.subscribe([this](Either* either) {
		string messageError = to_string(either->GetCode()) + ": " + either->GetLabel();
		shootError.on_next(messageError);
	});

	auto observerDatabase = backRest->observableUserJSON.map([](string jsonUser) {
		return jsonUser;
	});

	auto subscriptionDatabase = observerDatabase.subscribe([this](string jsonUser) {
		
		shootUserJSON.on_next(jsonUser);
		
	});
}

void FlowVideo::SetClient(int value) {
	client = value;
	
}

void FlowVideo::SetIsRegister(bool option) {
	backRest->SetIsRegister(option);
}

void FlowVideo::ReloadRecognitionFace() {
	backRest->ReloadRecognitionFace();
}

void FlowVideo::SetConfigurationDatabase() {
	backRest->SetConfigurationDatabase();
}

void FlowVideo::SetRefreshInterval(int value) {
	refreshInterval = value;
}

void FlowVideo::SetSequenceFps(int value) {
	sequenceFps = value;
	timeDeltaMs = 1000 / sequenceFps;
}

void AdvanceVideoStream() {
	int sizeVideoStream = refreshInterval / timeDeltaMs;
	int positionVideoStream = (countFrameTracking / sizeVideoStream) + 1;
	int positionFrameMaxVideoStream = sizeVideoStream * positionVideoStream;

	if (countFrameTracking < positionFrameMaxVideoStream)
	{
		countFrameTracking = positionFrameMaxVideoStream;
	}
}

void ClearCoordinatesImage(int indexTracked) {
	int index = indexTracked * NUM_COORDINATES_X_IMAGE;
	imageCoordinatesFollowed[index] = 0;
	imageCoordinatesFollowed[index + 1] = 0;
	imageCoordinatesFollowed[index + 2] = 0;
	imageCoordinatesFollowed[index + 3] = 0;
}

void BuildCoordinatesImage(float x, float y, float width, float height, int indexTracked) {
	int index = indexTracked * NUM_COORDINATES_X_IMAGE;
	imageCoordinatesFollowed[index] = x;
	imageCoordinatesFollowed[index + 1] = y;
	imageCoordinatesFollowed[index + 2] = width;
	imageCoordinatesFollowed[index + 3] = height;
}

void DrawRectangles(cv::Mat workMat) {
	for (int i = 0; i < COORDINATES_X_ALL_IMAGES; i += 4)
	{
		if (imageCoordinatesFollowed[i] != 0) {
			cv::Rect rect((int)imageCoordinatesFollowed[i], (int)imageCoordinatesFollowed[i + 1],
				(int)imageCoordinatesFollowed[i + 2], (int)imageCoordinatesFollowed[i + 3]);
			cv::rectangle(workMat, rect, cv::Scalar(0, 255, 0), 2);
		}
	}

}

void FlowVideo::LoadConfiguration(string nameFile) {
	pool->init();
	ObserverEvent();
	backRest->LoadConfiguration(nameFile);
	//gst_init(NULL, NULL);
}

unsigned char* LoadImageOfMemory(vector<unsigned char> buffer,
	int *width, int *height) {
	int lenght, errorCode;
	const char* imgData = reinterpret_cast<const char*> (&buffer[0]);
	if (imgData == NULL) {
		return NULL;
	}

	errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), width,
		height, &lenght, NULL);
	if (errorCode != IFACE_OK) {
		//error->CheckError(errorCode, error->medium);
		return NULL;
	}

	unsigned char* rawImage = new unsigned char[lenght];
	errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), width,
		height, &lenght, rawImage);
	if (errorCode != IFACE_OK) {
		//error->CheckError(errorCode, error->medium);
		return NULL;
	}

	return rawImage;

}

void FaceTracking(char* data, int size) {	
	int width, height, errorCode, countDesolation = 0;
	unsigned char* ucharData = reinterpret_cast<unsigned char*> (data);
	std::vector<uchar> vectorData(ucharData, ucharData + size);
	unsigned char* rawImageData = LoadImageOfMemory(vectorData, &width, &height);
	
	if (rawImageData != NULL) {

		errorCode = IFACE_TrackObjects(objectHandler, rawImageData,
			width, height, countFrameTracking*timeDeltaMs, NUM_TRACKED_OBJECTS, objects);

		//error->CheckError(errorCode, error->medium);
		//cout << "Error TrackObject: " << errorCode << endl;
		for (int trackedObjectIndex = 0; trackedObjectIndex < NUM_TRACKED_OBJECTS;
			trackedObjectIndex++)
		{

			float bbX, bbY, bbWidth, bbHeight;
			IFACE_TrackedObjectState trackedState;

			errorCode = IFACE_GetObjectState(objects[trackedObjectIndex],
				objectHandler, &trackedState);

			//error->CheckError(errorCode, error->medium);

			if (trackedState == IFACE_TRACKED_OBJECT_STATE_CLEAN) {

				ClearCoordinatesImage(trackedObjectIndex);
				countDesolation++;
				if (countDesolation == NUM_TRACKED_OBJECTS && !flagFirstDetect)
				{
					AdvanceVideoStream();
				}
				cout << "STATE_CLEAN" << endl;
				continue;
			}
			switch (trackedState)
			{
			case IFACE_TRACKED_OBJECT_STATE_TRACKED:
				flagFirstDetect = true;
				errorCode = IFACE_GetObjectBoundingBox(objects[trackedObjectIndex],
					objectHandler, &bbX, &bbY, &bbWidth, &bbHeight);

				//error->CheckError(errorCode, error->medium);
				BuildCoordinatesImage(bbX, bbY, bbWidth, bbHeight, trackedObjectIndex);
				printf("   face id is tracked. Its bounding box :(%f, %f), (%f, %f), Face score : , Object score : \n", bbX, bbY, bbWidth, bbHeight);
				break;
			case IFACE_TRACKED_OBJECT_STATE_COVERED:
				errorCode = IFACE_GetObjectBoundingBox(objects[trackedObjectIndex],
					objectHandler, &bbX, &bbY, &bbWidth, &bbHeight);
				//error->CheckError(errorCode, error->medium);
				BuildCoordinatesImage(bbX, bbY, bbWidth, bbHeight, trackedObjectIndex);
				break;
			case IFACE_TRACKED_OBJECT_STATE_SUSPEND:
				ClearCoordinatesImage(trackedObjectIndex);
				printf("STATE SUSPEND INDEX: %d\n", trackedObjectIndex);

				break;
			case IFACE_TRACKED_OBJECT_STATE_LOST:
				void *newObj;
				errorCode = IFACE_CreateObject(&newObj);
				//error->CheckError(errorCode, error->medium);
				objects[trackedObjectIndex] = newObj;
				ClearCoordinatesImage(trackedObjectIndex);
				flagFirstDetect = false;
				printf("STATE LOST INDEX: %d\n", trackedObjectIndex);

				break;
			case IFACE_TRACKED_OBJECT_STATE_CLEAN:
				printf("STATE CLEAN INDEX: %d\n", trackedObjectIndex);

				break;
			}

		}
		countFrameTracking++;
		delete[] rawImageData;
	}
	flagTracking = false;
}

void BackProcessImage(char* data, int size, int client) {
	backRest->ProcessImageInBack(data, size, client);
}


GstFlowReturn NewPreroll(GstAppSink* /*appsink*/, gpointer /*data*/)
{
	return GST_FLOW_OK;
}

GstFlowReturn NewSample(GstAppSink *appsink, gpointer /*data*/)
{
	static int framecount = 0;	

	GstSample *sample = gst_app_sink_pull_sample(appsink);
	GstCaps *caps = gst_sample_get_caps(sample);
	GstBuffer *buffer = gst_sample_get_buffer(sample);
	GstStructure *structure = gst_caps_get_structure(caps, 0);
	const int width = g_value_get_int(gst_structure_get_value(structure, "width"));
	const int height = g_value_get_int(gst_structure_get_value(structure, "height"));

	// Show caps on first frame
	if (!framecount) {		
		g_print("caps: %s\n", gst_caps_to_string(caps));
	}
	framecount++;

	GstMapInfo map;
	gst_buffer_map(buffer, &map, GST_MAP_READ);
	int size = width * height * 1;
	

	if (!flagTracking)
	{
		flagTracking = true;
		pool->submit(FaceTracking, (char*)map.data, (int)map.size);
		
		
		//std::async(std::launch::async, FaceTracking, (char*)map.data, (int)map.size);
		/*std::thread tti(FaceTracking, (char*)map.data, (int)map.size);
		tti.detach(); */
	}

	
	std::thread tbi(BackProcessImage, (char*)map.data, (int)map.size, client);
	tbi.detach();
	
	//pool->submit(BackProcessImage, (char*)map.data, (int)map.size, client);
		


	GBuffer* prevFrameBuffer = atomicBuffer.exchange(new GBuffer((char*)map.data, size));
	if (prevFrameBuffer)
	{
		delete prevFrameBuffer;
	}

	

	//cv::Mat* prevFrame;
	//prevFrame = atomicFrame.exchange(new cv::Mat(cv::Size(width, height),
	//	CV_8UC1, (char*)map.data, cv::Mat::AUTO_STEP)); //CV_16U CV_8UC3

	//g_print("size: %d width: %d height: %d \n", map.size, width, height);
	//prevFrame = atomicFrame.exchange(new cv::Mat(cv::Size(width, height),
	//	CV_8UC3, (char*)map.data, cv::Mat::AUTO_STEP)); //CV_16U CV_8UC3 height + height / 2

	//prevFrame = atomicFrame.exchange(new cv::Mat(cv::Size(img.cols, img.rows),
	//	CV_8UC3, (void*)img.data, cv::Mat::AUTO_STEP)); //CV_16U CV_8UC3

	//prevFrame = atomicFrame.exchange(&img); //char*
	/*if (prevFrame) {	
		
		delete prevFrame;
	}*/

	

	gst_buffer_unmap(buffer, &map);
	gst_sample_unref(sample);	
	return GST_FLOW_OK;
}

static gboolean MessageCallback(GstBus *bus, GstMessage *message, gpointer data)
{

	switch (GST_MESSAGE_TYPE(message)) {
	case GST_MESSAGE_ERROR: {
		GError *err;
		gchar *debug;

		gst_message_parse_error(message, &err, &debug);
		g_print("Error: %s\n", err->message);
		g_error_free(err);
		g_free(debug);
		break;
	}
	case GST_MESSAGE_EOS:
		/* end-of-stream */			
		break;
	default:
		/* unhandled message */
		break;
	}

	return true;
}


std::string IntToStr(int num)
{	
	std::ostringstream s;
	s << num;
	return s.str();
}

void FlowVideo::RecognitionFaceFiles(string file, int client) {
	backRest->RecognitionFaceFiles(file, client);
}

void FlowVideo::SetIsFinishLoadFiles(bool value) {
	backRest->SetIsFinishLoadFiles(value);
}

bool FlowVideo::GetIsFinishLoadFiles() {
	return backRest->GetIsFinishLoadFiles();
}

void FlowVideo::InitITracking() {
	int errorCode;

	errorCode = IFACE_CreateFaceHandler(&faceHandlerTracking);
	error->CheckError(errorCode, error->medium);
	errorCode = IFACE_CreateObjectHandler(&objectHandler, faceHandlerTracking);
	error->CheckError(errorCode, error->medium);

	for (int i = 0; i < NUM_TRACKED_OBJECTS; i++)
	{
		errorCode = IFACE_CreateObject(&objects[i]);
		error->CheckError(errorCode, error->medium);
	}

	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_FACE_DISCOVERY_FREQUENCE_MS,
		IntToStr(refreshInterval).c_str());
	error->CheckError(errorCode, error->medium);
	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_MIN_EYE_DISTANCE,
		IntToStr(minEyeDistance).c_str());
	error->CheckError(errorCode, error->medium);
	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_MAX_EYE_DISTANCE,
		IntToStr(maxEyeDistance).c_str());
	error->CheckError(errorCode, error->medium);
	errorCode = IFACE_SetParam(faceHandlerTracking,
		IFACE_PARAMETER_FACEDET_CONFIDENCE_THRESHOLD,
		IntToStr(faceConfidenceThresh).c_str()); //
	error->CheckError(errorCode, error->medium);

	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_TRACKING_MODE,
		IFACE_TRACK_TRACKING_MODE_OBJECT_TRACKING);
	error->CheckError(errorCode, error->medium);

	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_SPEED_ACCURACY_MODE,
		IFACE_TRACK_SPEED_ACCURACY_MODE_FAST);
	error->CheckError(errorCode, error->medium);

	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION,
		IFACE_TRACK_MOTION_OPTIMIZATION_HISTORY_LONG_FAST);
	error->CheckError(errorCode, error->medium);

	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_DEEP_TRACK,
		"true"); // IFACE_TRACK_DEEP_TRACK_DEFAULT
	error->CheckError(errorCode, error->medium);
}

void FlowVideo::CaptureFlow(int optionFlow) {
	/*int argc = 1;
	
	char appName[] = "AipuAPI.dll";    
	char *arg[] = { appName, NULL };
	char **argv[] = { arg, NULL };*/
		

	InitITracking();
	
	gchar *descr = DescriptionFlow(optionFlow);
	
	/*gst_init(&argc, argv);*/
	gst_init(NULL, NULL);
	
	GError *gError = nullptr;
	GstElement *pipeline = gst_parse_launch(descr, &gError);
	
	if (gError) {
		g_print("could not construct pipeline: %s\n", gError->message);
		g_error_free(gError);
		exit(-1);
	}

	GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");

	gst_app_sink_set_emit_signals((GstAppSink*)sink, true);
	gst_app_sink_set_drop((GstAppSink*)sink, true);
	gst_app_sink_set_max_buffers((GstAppSink*)sink, 1);
	GstAppSinkCallbacks callbacks = { nullptr, NewPreroll, NewSample };
	gst_app_sink_set_callbacks(GST_APP_SINK(sink), &callbacks, nullptr, nullptr);
	GstBus *bus;
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, MessageCallback, nullptr);
	gst_object_unref(bus);

	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);

	cv::namedWindow(nameWindow.c_str(), cv::WINDOW_NORMAL); //cv::WINDOW_GUI_EXPANDED

	while (!flagFlow) {
		g_main_iteration(false);
		
		GBuffer* frameBuffer = atomicBuffer.load();
		if (frameBuffer)
		{
			//clock_t timeStart1 = clock();
			cv::Mat img = cv::imdecode(frameBuffer->GetBuffer(), cv::IMREAD_COLOR);
			/*clock_t duration1 = clock() - timeStart1;
			int durationMs1 = int(1000 * ((float)duration1) / CLOCKS_PER_SEC);
			printf("   imdecode BUFFER  time: %d \n", durationMs1);*/
			
			if (img.data != NULL)
			{
				DrawRectangles(img);
				cv::imshow(nameWindow.c_str(), img);
				cv::waitKey(1000/sequenceFps);
				/*if (cv::waitKey(1000/sequenceFps) == 27) {
					flagFlow = true;
				}*/
				
			}
			
		}
		



		//cv::Mat* ptrFrameDraw = atomicMatDraw.load();
		//cv::Mat frameDraw;
		//cv::Mat* ptrFrameDraw = atomicFrame.load();
		//if (ptrFrameDraw) {
		//	//cv::Mat track = atomicFrame.load()[0].clone();
		//	
		//	/*ProcessTracking(track, &countFrameTracking, &flagFirstDetect);
		//	countFrameTracking++;*/
		//	/*if (!flagTracking)
		//	{
		//		flagTracking = true;
		//		std::thread tti(&FlowVideo::ProcessTracking, this, track,
		//			&countFrameTracking, &flagFirstDetect);
		//		tti.detach();
		//		countFrameTracking++;
		//	}*/
		//	
		//	//frameDraw = atomicMatDraw.load()[0].clone();
		//	/*cv::Mat* ptrMatDraw = atomicMatDraw.load();
		//	if (ptrMatDraw) {
		//		cv::Mat trackDraw = atomicMatDraw.load()[0].clone();
		//		DrawRectangles(trackDraw);

		//		cv::imshow(nameWindow.c_str(), trackDraw);
		//	}*/
		//	//cv::Mat trackDraw = atomicMatDraw.load()[0].clone();
		//	/*if (!track.empty())
		//	{
		//		DrawRectangles(track);
		//		cv::imshow(nameWindow.c_str(), track);
		//	}*/
		//	
		//	
		//	//cout << "Inside Show rows:" << atomicFrame.load()[0].rows << "  cols:" << atomicFrame.load()[0].cols << endl;
		//	
		//	clock_t timeStart1 = clock();
		//	//cv::Mat frameRGB(atomicFrame.load()[0].rows, atomicFrame.load()[0].cols, CV_8UC4);
		//	cv::Mat e;
		//	atomicFrame.load()[0].convertTo(e, CV_8UC3);
		//	//cv::cvtColor(atomicFrame.load()[0], e, cv::COLOR_GRAY2RGB); //xRGB COLOR_YUV2BGRA_I420		
		//	clock_t duration1 = clock() - timeStart1;
		//	int durationMs1 = int(1000 * ((float)duration1) / CLOCKS_PER_SEC);
		//	printf("   CHANGE COLOR  time: %d \n", durationMs1);

		//	
		//	//cout << "Outside Show" << endl;
		//	/*cv::Mat m = atomicFrame.load()[0];*/
		//	//cv::imwrite("5555.png", frameRGB);
		//	cv::imshow(nameWindow.c_str(), e);
		//	if (cv::waitKey(25) == 27) {
		//		flagFlow = true;
		//	}
		//		
		//	
		//	

		//	
		//	//track.release();
		//}
		
	}
	cv::destroyWindow(nameWindow.c_str());
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(pipeline));
	std::atomic_init(&atomicBuffer, 0);
	
	TerminateITracking();

}


void FlowVideo::TerminateITracking() {
	int errorCode;

	for (int i = 0; i < NUM_TRACKED_OBJECTS; i++)
	{
		try
		{
			if (objects[i] != NULL)
			{
				errorCode = IFACE_ReleaseEntity(objects[i]);
				error->CheckError(errorCode, error->less);
			}

		}
		catch (const std::exception& e)
		{
			cout << e.what() << endl;
		}

	}

	errorCode = IFACE_ReleaseEntity(objectHandler);
	error->CheckError(errorCode, error->less);

	errorCode = IFACE_ReleaseEntity(faceHandlerTracking);
	error->CheckError(errorCode, error->less);

	for (int i = 0; i < NUM_TRACKED_OBJECTS; i++) {
		ClearCoordinatesImage(i);
	}
}

gchar* FlowVideo::DescriptionFlow(int optionFlow) {
	gchar *descr = nullptr;

	switch (optionFlow) {
	case 1: // IP CAMERA
		
		descr = g_strdup_printf(
			"rtspsrc location=%s latency=0 "
			"! application/x-rtp, payload=96 ! rtph264depay ! h264parse ! avdec_h264 "
			"! decodebin ! videoconvert ! videoscale method=%d "
			"! video/x-raw,width=(int)%d,height=(int)%d,format=(string)I420 "
			"! jpegenc quality=100 "
			"! appsink name=sink emit-signals=true sync=false max-buffers=0 drop=true",
			ipCamera.c_str(), videoScaleMethod, widthFrame, heightFrame
		);
		break;
	case 2: // FILE		
		descr = g_strdup_printf(
			"filesrc location=%s "
			"! decodebin ! videoconvert ! videoscale method=%d "
			"! videobalance contrast=1 brightness=0 saturation=1 "
			"! video/x-raw, width=(int)%d, height=(int)%d, format=(string)I420  "	
			"! jpegenc quality=100 "
			"! appsink name=sink emit-signals=true sync=true max-buffers=0 drop=true",
			fileVideo.c_str(), videoScaleMethod, widthFrame, heightFrame
		);
		break;
	case 3: // CAMERA  
		break;
	}
	return descr;

}

void FlowVideo::ShowWindow(int option) {
	std::wstring stemp = std::wstring(nameWindow.begin(), nameWindow.end());
	LPCWSTR sw = stemp.c_str();
	HWND win_handle = FindWindow(0, sw);
	switch (option)
	{
	case 1:
		SetWindowPos(win_handle, HWND_BOTTOM, 0, 0, 0, 0, wFlags);
		break;
	case -1:
		SetWindowPos(win_handle, HWND_TOPMOST, 0, 0, 0, 0, wFlags);
		break;	
	}
	

}

