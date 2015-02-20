#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <process.h>

#include "../module_headers/module.h"
#include "../module_headers/robot_module.h"

#include "../../SerialClass/SerialClass.h"
#include "SimpleIni.h"

#include "uarm_robot_module.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

/* GLOBALS CONFIG */
const int COUNT_FUNCTIONS = 3;
const int COUNT_AXIS = 0;

#define DEFINE_ALL_FUNCTIONS \
	ADD_ROBOT_FUNCTION("move", 4, true)				/*stretch, height, rotation, handRot*/\
	ADD_ROBOT_FUNCTION("gripper_catch", 0, false)	/**/\
	ADD_ROBOT_FUNCTION("gripper_release", 0, false)	/**/


#define DEFINE_ALL_AXIS \
	/*ADD_ROBOT_AXIS("locked", 1, 0)\
	ADD_ROBOT_AXIS("straight", 2, 0)\
	ADD_ROBOT_AXIS("rotation", 2, 0)*/

/////////////////////////////////////////////////

inline int getIniValueInt(CSimpleIniA *ini, const char *section_name, const char *key_name) {
	const char *tmp = ini->GetValue(section_name, key_name, NULL);
	if (!tmp) {
		printf("Not specified value for \"%s\" in section \"%s\"!\n", key_name, section_name);
		throw;
	}
	return strtol(tmp, NULL, 0);
}
inline const char* getIniValueChar(CSimpleIniA *ini, const char *section_name, const char *key_name) {
	const char *result = ini->GetValue(section_name, key_name, NULL);
	if (!result) {
		printf("Not specified value for \"%s\" in section \"%s\"!\n", key_name, section_name);
		throw;
	}
	return result;
}

UarmRobotModule::UarmRobotModule() {
	{
		robot_functions = new FunctionData*[COUNT_FUNCTIONS];
		regval function_id = 0;
		DEFINE_ALL_FUNCTIONS
	}
	/*{
		robot_axis = new AxisData*[COUNT_AXIS];
		regval axis_id = 0;
		DEFINE_ALL_AXIS
	}*/
}

const char *UarmRobotModule::getUID() {
	return "Servo robot module 1.00";
}

int UarmRobotModule::init() {
	WCHAR DllPath[MAX_PATH] = {0};
	GetModuleFileNameW((HINSTANCE)&__ImageBase, DllPath, _countof(DllPath));

	WCHAR *tmp = wcsrchr(DllPath, L'\\');
	WCHAR ConfigPath[MAX_PATH] = {0};
	
	size_t path_len = tmp - DllPath;

	wcsncpy(ConfigPath, DllPath, path_len);
	wcscat(ConfigPath, L"\\config.ini");

	CSimpleIniA ini;
	ini.SetMultiKey(true);

	if (ini.LoadFile(ConfigPath) < 0) {
		printf("Can't load '%s' file!\n", ConfigPath);
		return 1;
	}

	try{
		int count_robots = getIniValueInt(&ini, "main", "count_robots");
	
		for (int i = 1; i <= count_robots; ++i) {
			std::string str("robot_port_");
			str += std::to_string(i);
			const char *port_name = getIniValueChar(&ini, "main", str.c_str());

			int len_p = 5 + strlen(port_name) + 1;
			char *p = new char[len_p]; 
			strcpy(p, "\\\\.\\");
			strcat(p, port_name);

			Serial* SP = new Serial(p);
			delete p;

			if (!SP->IsConnected()) {
				printf("Can't connected to the serial port %s!", port_name);
				continue;
			}

			UarmRobot *uarm_robot = new UarmRobot(SP);
			printf("DLL: connected to %s robot %p\n", port_name, uarm_robot);
			aviable_connections.push_back(uarm_robot);

#ifdef _DEBUG
			uarm_robot->startCheckingSerial();
#endif
		}
	} catch(...) {
		return 1;
	}

	return 0;
}

FunctionData** UarmRobotModule::getFunctions(int *count_functions) {
	(*count_functions) = COUNT_FUNCTIONS;
	return robot_functions;
}

AxisData** UarmRobotModule::getAxis(int *count_axis) {
	(*count_axis) = COUNT_AXIS;
	//return robot_axis;
	return NULL;
}

Robot* UarmRobotModule::robotRequire() {
	for (v_connections_i i = aviable_connections.begin(); i != aviable_connections.end(); ++i) {
		if ((*i)->is_aviable) {
			(*i)->is_aviable = false;
			return (*i);
		}
	}
	return NULL;
}

void UarmRobotModule::robotFree(Robot *robot) {
	UarmRobot *uarm_robot = reinterpret_cast<UarmRobot*>(robot);

	for (v_connections_i i = aviable_connections.begin(); i != aviable_connections.end(); ++i) {
		if ((*i) == uarm_robot) {
			uarm_robot->is_aviable = true;
			return;
		}
	}
}

void UarmRobotModule::final() {
	for (v_connections_i i = aviable_connections.begin(); i != aviable_connections.end(); ++i) {
		delete (*i);
	}
	aviable_connections.clear();
}

void UarmRobotModule::destroy() {
	for (int j = 0; j < COUNT_FUNCTIONS; ++j) {
		delete robot_functions[j];
	}
	for (int j = 0; j < COUNT_AXIS; ++j) {
		delete robot_axis[j];
	}
	delete[] robot_functions;
	//delete[] robot_axis;
	delete this;
}

FunctionResult* UarmRobot::executeFunction(regval command_index, regval *args) {
	unsigned int len;
	switch(command_index) {
		case 1: {
			len = 10;
			break;
		}
		case 2:
		case 3: {
			len = 3;
			break;
		}
		default:
			return NULL;
	}

	unsigned char *buffer;
	buffer = new unsigned char[len];
	buffer[0] = 0xFF;

	switch(command_index) {
		case 1: {
			buffer[1] = 0xEE;

			short int tmp = *args;

			buffer[2] = (tmp >> 8) & 0x00FF;
			buffer[3] = tmp & 0x00FF;

			tmp = *(args + 1);
			buffer[4] = (tmp >> 8) & 0x00FF;
			buffer[5] = tmp & 0x00FF;

			tmp = *(args + 2);
			buffer[6] = (tmp >> 8) & 0x00FF;
			buffer[7] = tmp & 0x00FF;

			tmp = *(args + 3);
			buffer[8] = (tmp >> 8) & 0x00FF;
			buffer[9] = tmp & 0x00FF;
			break;
		}
		case 2: {
			buffer[1] = 0xDD;
			buffer[2] = 1;
			break;
		}
		case 3: {
			buffer[1] = 0xDD;
			buffer[2] = 0;
			break;
		}
		default:
			return NULL;
	}

#ifdef _DEBUG
	EnterCriticalSection(&cheking_mutex);
#endif
	bool have_error = !SP->WriteData(buffer, len);
#ifdef _DEBUG
	LeaveCriticalSection(&cheking_mutex);
#endif

	delete[] buffer;

	if (have_error) {
		return new FunctionResult(0);
	}
	return new FunctionResult(1, 0);
}

void UarmRobot::axisControl(regval axis_index, regval value) {}

#ifdef _DEBUG
unsigned int WINAPI procCheckingSerial(void *arg) {
	UarmRobot *uarm_robot = static_cast<UarmRobot*>(arg);
	uarm_robot->checkSerial();
	return 0;
}

void UarmRobot::startCheckingSerial() {
	checkingSerial = true;
	InitializeCriticalSection(&cheking_mutex);
	thread_handle = (HANDLE) _beginthreadex(NULL, 0, procCheckingSerial, this, 0, &tid);
}

void UarmRobot::checkSerial() {
	const int dataLength = 256;
	char incomingData[dataLength] = "";
	int readResult = 0;

	EnterCriticalSection(&cheking_mutex);
	while(SP->IsConnected() && checkingSerial) {
		readResult = SP->ReadData(incomingData, dataLength);
		LeaveCriticalSection(&cheking_mutex);

		if (readResult != -1) {
			printf("Bytes read: %i\n", readResult);
			incomingData[readResult] = 0;
			printf("%s\n", incomingData);
		}
		Sleep(500);

		EnterCriticalSection(&cheking_mutex);
	}
	LeaveCriticalSection(&cheking_mutex);
}
#endif

UarmRobot::~UarmRobot() {
#ifdef _DEBUG
	EnterCriticalSection(&cheking_mutex);
	checkingSerial = false;
	LeaveCriticalSection(&cheking_mutex);

	WaitForSingleObject(thread_handle, INFINITE);
	CloseHandle(thread_handle);
	DeleteCriticalSection(&cheking_mutex);
#endif

	SP->~Serial();
}

__declspec(dllexport) RobotModule* getRobotModuleObject() {
	return new UarmRobotModule();
}
