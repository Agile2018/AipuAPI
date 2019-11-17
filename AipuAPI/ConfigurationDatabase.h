#ifndef ConfigurationDatabase_h
#define ConfigurationDatabase_h

#include "Configuration.h"
#include "ErrorAipuLib.h"

class ConfigurationDatabase : public Configuration
{
public:
	ConfigurationDatabase();
	~ConfigurationDatabase();
	void ParseJSONToObject();
	void ParseMapToJSON();

	void SetNameDatabase(string name) {
		nameDatabase = name;
	}

	void SetConnectString(string connect) {
		connectString = connect;
	}

	string GetNameDatabase() {
		return nameDatabase;
	}

	string GetConnectString() {
		return connectString;
	}
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();

private:
	const string LABEL_ERROR_PARSING = "Error parsing the JSON string";
	ErrorAipuLib* error = new ErrorAipuLib();
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	const string NAME_DATABASE = "name";
	const string CONNECT_STRING = "connect";
	const string PARAMS = "Params";
	const string CONFIGURATION = "configuration";
	const string DATABASE_CONFIGURATION = "database_configuration";
	string nameDatabase;
	string connectString;
	void ObserverError();
	void SetValueJSONToConfiguration();
};


#endif // !ConfigurationDatabase_h

