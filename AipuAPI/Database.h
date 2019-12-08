#ifndef Database_h
#define Database_h

#include <iostream>
#include "MongoAccess.h"
#include "User.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/types.hpp>
#include "ErrorAipuLib.h"
#include "Base64.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <json\json.h>
#include "ConfigurationDatabase.h"
#include "File.h"

using namespace cv;
using namespace std;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::type;

class Database
{
public:
	Database();
	~Database();

	void Configure();
	void InsertNewUser(User* user);
	void FindUserByIdFace(int idFaceUser, vector<unsigned char> image,
		int rows, int cols, int client);
	void ResetIdUser() {
		lastUserId = -1;
		lastClient = -1;
	}
	ConfigurationDatabase* configuration = new ConfigurationDatabase();
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
	Rx::subject<string> userSubject;
	Rx::observable<string> observableUserJSON = userSubject.get_observable();

private:
	const int ERROR_DATABASE = 404;
	const string COLLECTION_USER = "user";
	const string COLLECTION_IMAGE = "image";
	const string FIELD_IMAGE_ID_USER = "id_user";
	const string FIELD_IMAGE_DATA = "data_64";
	const string FIELD_USER_ID_FACE = "id_face";
	const string FIELD_USER_ID = "_id";
	const string FIELD_USER_NAME = "name";
	const string FIELD_USER_LAST_NAME = "lastname";
	const string FIELD_USER_IDENTIFICATION = "identification";
	const string FIELD_USER_REGISTER = "register";
	const string FIELD_CLIENT = "client";
	const string FORMAT_IMAGE_64 = "data:image/jpeg;base64,";
	const string PARAMS = "params";
	const string DATASOURCE = "MongoDB";
	int lastUserId = 0;
	int lastClient = 0;
	int countRepeatOfSomeUser = 0;
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	Rx::subscriber<string> shootUserJSON = userSubject.get_subscriber();
	ErrorAipuLib* error = new ErrorAipuLib();
	File* file = new File();
	Base64* base64 = new Base64();
	void AddUser(User* user);
	//void AddRecordsUser(User* user);
	void AddImageUser(vector<unsigned char> image, int rows, int cols, int idUser);
	void ObserverError();
	void QueryUserByFace(int idFaceUser, int client);
	void BuildJSONUser(vector<std::string> values);
	void BuildNewUser(User* user);
	string QueryImageOfUser(int idFaceUser);
	string FileImageToStringBase64(vector<unsigned char> image, int rows, int cols);
	void DeleteFileTempCropImage(string filePath);
	void UpdateImageUser(int idFaceUser, vector<unsigned char> image, int rows, int cols);

};


#endif // !Database_h

