
#include <stdio.h>
#include <stdlib.h>
#include <exception>
#include <vector>
#include <math.h>

#include <windows.h>

#include <boost/thread/thread.hpp>

#include "module.h"
#include "robot_module.h"

#include "SerialClass.h"
#include "SimpleIni.h"

#include "uarm_robot_module.h"

unsigned int COUNT_FUNCTIONS = 0;
const unsigned int COUNT_AXIS = 0;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

#define ADD_ROBOT_AXIS(AXIS_NAME, UPPER_VALUE, LOWER_VALUE) \
	robot_axis[axis_id] = new AxisData; \
	robot_axis[axis_id]->axis_index = axis_id + 1; \
	robot_axis[axis_id]->upper_value = UPPER_VALUE; \
	robot_axis[axis_id]->lower_value = LOWER_VALUE; \
	robot_axis[axis_id]->name = AXIS_NAME; \
	++axis_id;

/////////////////////////////////////////////////

inline double sqr(double value) {
    return value * value;
}

inline double RadToDeg(double radiants) {
    return radiants * 180 / 3.14159265359f;
}

inline int getIniValueInt(CSimpleIniA *ini, const char *section_name, const char *key_name) {
    const char *tmp = ini->GetValue(section_name, key_name, NULL);
    if (!tmp) {
        printf("Not specified value for \"%s\" in section \"%s\"!\n", key_name, section_name);
        throw std::exception();
    }
    return strtol(tmp, NULL, 0);
}
inline const char* getIniValueChar(CSimpleIniA *ini, const char *section_name, const char *key_name) {
    const char *result = ini->GetValue(section_name, key_name, NULL);
    if (!result) {
        printf("Not specified value for \"%s\" in section \"%s\"!\n", key_name, section_name);
        throw std::exception();
    }
    return result;
}

UarmRobotModule::UarmRobotModule() {
    robot_functions = new FunctionData*[COUNT_FUNCTIONS];
    unsigned int function_id = 0;

    robot_functions[function_id] = new FunctionData(function_id + 1, 0, NULL, "calibration");
    ++function_id;

    FunctionData::ParamTypes *pt = new FunctionData::ParamTypes[4];
    pt[0] = FunctionData::ParamTypes::FLOAT;
    pt[1] = FunctionData::ParamTypes::FLOAT;
    pt[2] = FunctionData::ParamTypes::FLOAT;
    pt[3] = FunctionData::ParamTypes::FLOAT;
    robot_functions[function_id] = new FunctionData(function_id + 1, 4, pt, "move");
    ++function_id;

    robot_functions[function_id] = new FunctionData(function_id + 1, 0, NULL, "gripper_catch");
    ++function_id;
    robot_functions[function_id] = new FunctionData(function_id + 1, 0, NULL, "gripper_release");
    ++function_id;

    pt = new FunctionData::ParamTypes[3];
    pt[0] = FunctionData::ParamTypes::FLOAT;
    pt[1] = FunctionData::ParamTypes::FLOAT;
    pt[2] = FunctionData::ParamTypes::FLOAT;
    robot_functions[function_id] = new FunctionData(function_id + 1, 3, pt, "calculate_main_angles");
    ++function_id;

    COUNT_FUNCTIONS = function_id;
}

const char *UarmRobotModule::getUID() {
    return "Servo robot module 1.00";
}

void UarmRobotModule::prepare(colorPrintfModule_t *colorPrintf_p, colorPrintfModuleVA_t *colorPrintfVA_p) {
    this->colorPrintf_p = colorPrintfVA_p;
}

FunctionData** UarmRobotModule::getFunctions(unsigned int *count_functions) {
    (*count_functions) = COUNT_FUNCTIONS;
    return robot_functions;
}

AxisData** UarmRobotModule::getAxis(unsigned int *count_axis) {
    (*count_axis) = COUNT_AXIS;
    return NULL;
}

void *UarmRobotModule::writePC(unsigned int *buffer_length) {
    (*buffer_length) = 0;
    return NULL;
}

int UarmRobotModule::init() {
    WCHAR DllPath[MAX_PATH] = {0};
    GetModuleFileNameW((HINSTANCE)&__ImageBase, DllPath, (DWORD) MAX_PATH);

    WCHAR *tmp = wcsrchr(DllPath, L'\\');
    WCHAR wConfigPath[MAX_PATH] = {0};

    size_t path_len = tmp - DllPath;

    wcsncpy(wConfigPath, DllPath, path_len);
    wcscat(wConfigPath, L"\\config.ini");

    char ConfigPath[MAX_PATH] = {0};
    wcstombs(ConfigPath,wConfigPath,sizeof(ConfigPath));

    CSimpleIniA ini;
    ini.SetMultiKey(true);

    if (ini.LoadFile(ConfigPath) < 0) {
        printf("Can't load '%s' file!\n", ConfigPath);
        return 1;
    }

    try{
        int count_robots = getIniValueInt(&ini, "main", "count_robots");

        for (int i = 1; i <= count_robots; ++i) {
            char section_name[50];
            sprintf(section_name, "robot_port_%d", i);

            const char *port_name = getIniValueChar(&ini, "main", section_name);

            int len_p = 5 + strlen(port_name) + 1;
            char *p = new char[len_p];
            strcpy(p, "\\\\.\\");
            strcat(p, port_name);

            Serial* SP = new Serial(p);
            delete p;

            if (!SP->IsConnected()) {
                colorPrintf(ConsoleColor(ConsoleColor::red), "Can't connected to the serial port %s!\n", port_name);
                delete SP;
                continue;
            }

            UarmRobot *uarm_robot = new UarmRobot(SP);
            colorPrintf(ConsoleColor(ConsoleColor::green), "DLL: connected to %s\n", port_name);
            aviable_connections.push_back(uarm_robot);
        }
    } catch(...) {
        return 1;
    }

    return 0;
}

Robot* UarmRobotModule::robotRequire() {
    connections_mutex.lock();
    for (v_connections_i i = aviable_connections.begin(); i != aviable_connections.end(); ++i) {
        if ((*i)->is_aviable) {
            (*i)->is_aviable = false;
            connections_mutex.unlock();
            return (*i);
        }
    }
    connections_mutex.unlock();
    return NULL;
}

void UarmRobotModule::robotFree(Robot *robot) {
    UarmRobot *uarm_robot = reinterpret_cast<UarmRobot*>(robot);

    connections_mutex.lock();
    for (v_connections_i i = aviable_connections.begin(); i != aviable_connections.end(); ++i) {
        if ((*i) == uarm_robot) {
            uarm_robot->is_aviable = true;
            break;
        }
    }
    connections_mutex.unlock();
}

void UarmRobotModule::final() {
    for (v_connections_i i = aviable_connections.begin(); i != aviable_connections.end(); ++i) {
        delete (*i);
    }
    aviable_connections.clear();
}

int UarmRobotModule::startProgram(int uniq_index) {
    return 0;
}

int UarmRobotModule::endProgram(int uniq_index) {
    return 0;
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

void UarmRobotModule::colorPrintf(ConsoleColor colors, const char *mask, ...) {
    va_list args;
    va_start(args, mask);
    (*colorPrintf_p)(this, colors, mask, args);
    va_end(args);
}

void UarmRobot::colorPrintf(ConsoleColor colors, const char *mask, ...) {
    va_list args;
    va_start(args, mask);
    (*colorPrintf_p)(this, NULL, colors, mask, args);
    va_end(args);
}

MainAngles UarmRobot::calculate_angles(double x, double y, double z) {
    const double gripper_height = 60.0f;
    const double gripper_straight = 41.0f;

    const double base_height = 90.0f;
    const double base_straight = 20.0f;

    const double arm_upper_len = 160.0f;
    const double arm_lower_len = 148.0f;


    double rot_angle = RadToDeg(atan((y)/(x)));

    double straigh_pos = sqrt(sqr(x) + sqr(y));

    double arm_imaginary_straight = straigh_pos - gripper_straight - base_straight;
    double arm_imaginary_height = z - gripper_height - base_height;
    double arm_imaginary_len_sqr = sqr(arm_imaginary_straight) + sqr(arm_imaginary_height);
    double arm_imaginary_len = sqrt(arm_imaginary_len_sqr);

    double R_angle = RadToDeg(
        acos(
            (
                sqr(arm_upper_len) + sqr(arm_lower_len) - arm_imaginary_len_sqr
            ) / (
                2 * arm_upper_len * arm_lower_len
            )
        )
    );

    double L_angle_triangle =
        acos(
            (
                arm_imaginary_len_sqr + sqr(arm_lower_len) - sqr(arm_upper_len)
            ) / (
                2 * arm_lower_len * arm_imaginary_len
            )
        );

    /*double L_angle = RadToDeg(
        atan(z / arm_imaginary_straight) + L_angle_triangle
    );*/

    return MainAngles(R_angle, L_angle_triangle, rot_angle);
}

void UarmRobot::prepare(colorPrintfRobot_t *colorPrintf_p, colorPrintfRobotVA_t *colorPrintfVA_p) {
    this->colorPrintf_p = colorPrintfVA_p;
}

FunctionResult* UarmRobot::executeFunction(system_value command_index, void **args) {
    if ((command_index < 1) || (command_index > (int) COUNT_FUNCTIONS)) {
        return NULL;
    }

    char *buffer = NULL;
    unsigned int len = 0;
    switch (command_index) {
        case 1: { //calibration
            len = 3;
            buffer = new char[len];
            strcpy(buffer, "T;");
            break;
        }
        case 2: { //move
            variable_value *x_pos = (variable_value *) args[0];
            variable_value *y_pos = (variable_value *) args[1];
            variable_value *z_pos = (variable_value *) args[2];
            variable_value *angle_z = (variable_value *) args[4];

            calculate_angles(*x_pos, *y_pos, *z_pos);

            break;
        }
        case 3: { //gripperCatch
            len = 3;
            buffer = new char[len];
            buffer[0] = 'G';
            buffer[1] = '1';
            buffer[2] = ';';
            break;
        }
        case 4: { //gripperRelease
            len = 3;
            buffer = new char[len];
            buffer[0] = 'G';
            buffer[1] = '0';
            buffer[2] = ';';
            break;
        }
        case 5: { //calculate_main_angles
            variable_value *x_pos = (variable_value *) args[0];
            variable_value *y_pos = (variable_value *) args[1];
            variable_value *z_pos = (variable_value *) args[2];

            MainAngles ma = calculate_angles(*x_pos, *y_pos, *z_pos);
            colorPrintf(ConsoleColor(ConsoleColor::green), "left angle = %f rads.\n", ma.left);
            colorPrintf(ConsoleColor(ConsoleColor::green), "right angle = %f rads.\n", ma.right);
            colorPrintf(ConsoleColor(ConsoleColor::green), "rot angle = %f rads.\n", ma.rot);
            return new FunctionResult(1);
        }
    }

    bool have_error = !SP->WriteData((unsigned char *)buffer, len);
    delete[] buffer;
    if (have_error) {
        return new FunctionResult(0);
    }

    if (command_index == 1) {
        const int dataLength = 1024;
        char incomingData[dataLength] = "";
        int readResult = 0;

        while(SP->IsConnected()) {
            readResult = SP->ReadData(incomingData, dataLength);

            if (readResult != -1) {
                colorPrintf(ConsoleColor(ConsoleColor::aqua), "Bytes read: %i\n", readResult);
                incomingData[readResult] = 0;
                colorPrintf(ConsoleColor(ConsoleColor::green), "%s\n", incomingData);

                if (strchr(incomingData, ';')) {
                    break;
                }
            }
            Sleep(100);
        }
    }

    return new FunctionResult(1);
}

UarmRobot::~UarmRobot() {
    delete SP;
}

PREFIX_FUNC_DLL RobotModule* getRobotModuleObject() {
    return new UarmRobotModule();
}
