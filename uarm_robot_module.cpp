
#include <stdio.h>
#include <stdlib.h>
#include <exception>
#include <vector>
#include <math.h>
#include <string>
#include <string.h>

#include <windows.h>

#include <boost/thread/thread.hpp>

#include "module.h"
#include "robot_module.h"

#include "SerialClass.h"
#include "SimpleIni.h"

#include "Error.h"

#include "uarm_robot_module.h"

const unsigned int COUNT_FUNCTIONS = 6;
const unsigned int COUNT_AXIS = 0;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

#define ARM_LOWER_LEN       148.0f
#define ARM_UPPER_LEN       160.0f
#define GRIPPER_HEIGHT      60.0f
#define GRIPPER_STRAIGHT    41.0f
#define BASE_HEIGHT         90.0f
#define BASE_STRAIGHT       20.0f

#define D150A_SERVO_MIN_PUL 535.0f
#define D150A_SERVO_MAX_PUL 2415.0f
#define D009A_SERVO_MIN_PUL 600.0f
#define D009A_SERVO_MAX_PUL 2550.0f
#define SERVO_MAX           605.0f
#define SERVO_MIN           80.0f

#define ANGLE_ROT_MAX       90.0f
#define ANGLE_ROT_MIN       -90.0f

#define ANGLE_SERVO_MAX     180.0f
#define ANGLE_SERVO_MIN     0.0f

#define ANGLE_LEFT_MIN_MAP  24.628047f
#define ANGLE_LEFT_MAX_MAP  50.962675f

#define ANGLE_RIGHT_MIN_MAP  83.108139f
#define ANGLE_RIGHT_MAX_MAP  132.698940f

#define ADD_ROBOT_AXIS(AXIS_NAME, UPPER_VALUE, LOWER_VALUE) \
	robot_axis[axis_id] = new AxisData; \
	robot_axis[axis_id]->axis_index = axis_id + 1; \
	robot_axis[axis_id]->upper_value = UPPER_VALUE; \
	robot_axis[axis_id]->lower_value = LOWER_VALUE; \
	robot_axis[axis_id]->name = AXIS_NAME; \
	++axis_id;

/////////////////////////////////////////////////

void checkMinMax(variable_value value, variable_value min, variable_value max) {
    if (
        (value < min)
        || (value > max)
    ) {
        throw new Error("Check failed for value %f between %f and %f", value, min, max);
    }
}

void check009A_ServoMinMax(variable_value value, const char* servo_name) {
    try {
        checkMinMax(value, D009A_SERVO_MIN_PUL, D009A_SERVO_MAX_PUL);
    } catch (Error *e) {
        throw new Error(e, "Check failed for 009A servo: %s!", servo_name);
    }
}

void check150A_ServoMinMax(variable_value value, const char* servo_name) {
    try {
        checkMinMax(value, D150A_SERVO_MIN_PUL, D150A_SERVO_MAX_PUL);
    } catch (Error *e) {
        throw new Error(e, "Check failed for 150A servo: %s!", servo_name);
    }
}

double map(double value, double min_x, double max_x, double min_y, double max_y) {
    return (((value - min_x) / (max_x - min_x)) * (max_y - min_y)) + min_y;
}

std::string variableToString(variable_value value) {
    int i_value = (int) value;

    char text[40];
    sprintf(text, "%d", i_value);

    std::string result = "";
    result.append(text);
    return result;
}

double sqr(double value) {
    return value * value;
}

double RadToDeg(double radiants) {
    return radiants * 180 / 3.14159265359f;
}

int getIniValueInt(CSimpleIniA *ini, const char *section_name, const char *key_name) {
    const char *tmp = ini->GetValue(section_name, key_name, NULL);
    if (!tmp) {
        printf("Not specified value for \"%s\" in section \"%s\"!\n", key_name, section_name);
        throw std::exception();
    }
    return strtol(tmp, NULL, 0);
}
const char* getIniValueChar(CSimpleIniA *ini, const char *section_name, const char *key_name) {
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
    robot_functions[function_id] = new FunctionData(function_id + 1, 4, pt, "moveByPulse");
    ++function_id;

    pt = new FunctionData::ParamTypes[4];
    pt[0] = FunctionData::ParamTypes::FLOAT;
    pt[1] = FunctionData::ParamTypes::FLOAT;
    pt[2] = FunctionData::ParamTypes::FLOAT;
    pt[3] = FunctionData::ParamTypes::FLOAT;
    robot_functions[function_id] = new FunctionData(function_id + 1, 4, pt, "moveByCoords");
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

        is_demo = !!getIniValueInt(&ini, "main", "is_demo");

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

            if (!is_demo && !SP->IsConnected()) {
                colorPrintf(ConsoleColor(ConsoleColor::red), "Can't connected to the serial port %s!\n", port_name);
                delete SP;
                continue;
            }

            UarmRobot *uarm_robot = new UarmRobot(SP, is_demo);
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
    for (unsigned int j = 0; j < COUNT_FUNCTIONS; ++j) {
        if (robot_functions[j]->count_params) {
            delete[] robot_functions[j]->params;
        }
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

MainAngles *UarmRobot::calculate_angles(double x, double y, double z) {
    double rot_angle = RadToDeg(atan((y)/(x)));

    double straigh_pos = sqrt(sqr(x) + sqr(y));

    double arm_imaginary_straight = straigh_pos - GRIPPER_STRAIGHT - BASE_STRAIGHT;
    double arm_imaginary_height = z - GRIPPER_HEIGHT - BASE_HEIGHT;
    double arm_imaginary_len_sqr = sqr(arm_imaginary_straight) + sqr(arm_imaginary_height);
    double arm_imaginary_len = sqrt(arm_imaginary_len_sqr);

    double R_angle = RadToDeg(
        acos(
            (
                sqr(ARM_UPPER_LEN) + sqr(ARM_LOWER_LEN) - arm_imaginary_len_sqr
            ) / (
                2 * ARM_UPPER_LEN * ARM_LOWER_LEN
            )
        )
    );

    double L_angle_triangle = RadToDeg(
        acos(
            (
                arm_imaginary_len_sqr + sqr(ARM_LOWER_LEN) - sqr(ARM_UPPER_LEN)
            ) / (
                2 * ARM_LOWER_LEN * arm_imaginary_len
            )
        )
    );

    return new MainAngles(R_angle, L_angle_triangle, rot_angle);
}

void UarmRobot::prepare(colorPrintfRobot_t *colorPrintf_p, colorPrintfRobotVA_t *colorPrintfVA_p) {
    this->colorPrintf_p = colorPrintfVA_p;
}

FunctionResult* UarmRobot::executeFunction(system_value command_index, void **args) {
    if ((command_index < 1) || (command_index > (int) COUNT_FUNCTIONS)) {
        return NULL;
    }

    try {
        std::string str_buf = "";
        switch (command_index) {
            case 1: { //calibration
                str_buf = "T";
                break;
            }
            case 2: { //moveByPulse
                str_buf = "M";

                variable_value *servo_rot = (variable_value *) args[0];
                check150A_ServoMinMax(*servo_rot, "ROT");

                variable_value *servo_left = (variable_value *) args[1];
                check150A_ServoMinMax(*servo_left, "LEFT");

                variable_value *servo_right = (variable_value *) args[2];
                check150A_ServoMinMax(*servo_right, "RIGHT");

                variable_value *servo_hand = (variable_value *) args[3];
                check009A_ServoMinMax(*servo_hand, "HAND");

                str_buf += variableToString(*servo_rot);
                str_buf += ",15,";
                str_buf += variableToString(*servo_left);
                str_buf += ",15,";
                str_buf += variableToString(*servo_right);
                str_buf += ",15,";
                str_buf += variableToString(*servo_hand);
                str_buf += ",15";

                break;
            }
            case 3: { //moveByCoords
                variable_value *x_pos = (variable_value *) args[0];
                variable_value *y_pos = (variable_value *) args[1];
                variable_value *z_pos = (variable_value *) args[2];
                variable_value *angle_z = (variable_value *) args[3];

                MainAngles *ma = calculate_angles(*x_pos, *y_pos, *z_pos);

                try {
                    checkMinMax(ma->rot, ANGLE_ROT_MIN, ANGLE_ROT_MAX);
                } catch (Error *e) {
                    throw new Error(e, "Check failed for calculated angle ROT: %f", ma->rot);
                }
                variable_value servo_rot = round(map(ma->rot, ANGLE_ROT_MIN, ANGLE_ROT_MAX, D150A_SERVO_MIN_PUL, D150A_SERVO_MAX_PUL));

                try {
                    checkMinMax(ma->left, ANGLE_SERVO_MIN, ANGLE_SERVO_MAX);
                } catch (Error *e) {
                    throw new Error(e, "Check failed for calculated angle LEFT: %f", ma->left);
                }
                variable_value servo_left = round(map(ma->left, ANGLE_LEFT_MIN_MAP, ANGLE_LEFT_MAX_MAP, D150A_SERVO_MIN_PUL, D150A_SERVO_MAX_PUL));

                try {
                    checkMinMax(ma->right, ANGLE_SERVO_MIN, ANGLE_SERVO_MAX);
                } catch (Error *e) {
                    throw new Error(e, "Check failed for calculated angle RIGHT: %f", ma->right);
                }
                variable_value servo_right = round(map(ma->right, ANGLE_RIGHT_MIN_MAP, ANGLE_RIGHT_MAX_MAP, D150A_SERVO_MIN_PUL, D150A_SERVO_MAX_PUL));

                try {
                    checkMinMax(*angle_z, ANGLE_ROT_MIN, ANGLE_ROT_MAX);
                } catch (Error *e) {
                    throw new Error(e, "Check failed for angle HAND: %f", *angle_z);
                }
                variable_value servo_hand_rot = round(map(*angle_z, ANGLE_ROT_MIN, ANGLE_ROT_MAX, D009A_SERVO_MIN_PUL, D009A_SERVO_MAX_PUL));

                check150A_ServoMinMax(servo_rot, "ROT");
                check150A_ServoMinMax(servo_left, "LEFT");
                check150A_ServoMinMax(servo_right, "RIGHT");
                check009A_ServoMinMax(servo_hand_rot, "HAND");

                //special checks

                str_buf += variableToString(servo_rot);
                str_buf += ",15,";
                str_buf += variableToString(servo_left);
                str_buf += ",15,";
                str_buf += variableToString(servo_right);
                str_buf += ",15,";
                str_buf += variableToString(servo_hand_rot);
                str_buf += ",15";


                break;
            }
            case 4: { //gripperCatch
                str_buf = "G1";
                break;
            }
            case 5: { //gripperRelease
                str_buf = "G0";
                break;
            }
            case 6: { //calculate_main_angles
                variable_value *x_pos = (variable_value *) args[0];
                variable_value *y_pos = (variable_value *) args[1];
                variable_value *z_pos = (variable_value *) args[2];

                MainAngles *ma = calculate_angles(*x_pos, *y_pos, *z_pos);
                colorPrintf(ConsoleColor(ConsoleColor::green), "left angle = %f rads.\n", ma->left);
                colorPrintf(ConsoleColor(ConsoleColor::green), "right angle = %f rads.\n", ma->right);
                colorPrintf(ConsoleColor(ConsoleColor::green), "rot angle = %f rads.\n", ma->rot);
                delete ma;

                return new FunctionResult(1);
            }
        }

        if (!is_demo) {
            str_buf += ";";
            bool have_error = !SP->WriteData((unsigned char *) str_buf.c_str(), str_buf.length());
            if (have_error) {
                return new FunctionResult(0);
            }

            if (command_index == 1) {
                std::string output = "";
                const int dataLength = 1024;
                char incomingData[dataLength] = "";
                int readResult = 0;

                while(SP->IsConnected()) {
                    readResult = SP->ReadData(incomingData, dataLength);

                    if (readResult != -1) {
                        incomingData[readResult] = 0;

                        char *p = strchr(incomingData, '|');
                        if (p) {
                            output.append(incomingData, p - incomingData - 1);
                            colorPrintf(ConsoleColor(ConsoleColor::green), "\n\n%s\n\n", output.c_str());

                            output = "";
                            output.append(p + 1);
                        } else {
                            output.append(incomingData);
                        }

                        if (strchr(incomingData, ';')) {
                            break;
                        }
                    }
                    Sleep(100);
                }
            }
        }

        return new FunctionResult(1);
    } catch (Error *e) {
        Error *ge = new Error(e, "Error on execute function: %d", command_index);
        colorPrintf(ConsoleColor(ConsoleColor::red), "\n%s\n", ge->emit().c_str());
        delete ge;

        return new FunctionResult(0);
    }
}

UarmRobot::~UarmRobot() {
    delete SP;
}

PREFIX_FUNC_DLL RobotModule* getRobotModuleObject() {
    return new UarmRobotModule();
}
