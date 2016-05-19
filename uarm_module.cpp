#include <stdio.h>
#include <vector>
#include <string>
#include <list>
#include <vcclr.h>
#include <Windows.h>
#include <vector>

#include "UarmError.h"
#include "Utils.h"
#include "build_number.h"
#include "uarm_module.h"
namespace UARM{
  using namespace std;
	PREFIX_FUNC_DLL unsigned short getRobotModuleApiVersion() { return MODULE_API_VERSION; };
	PREFIX_FUNC_DLL RobotModule* getRobotModuleObject() {
		CUarmRobotModule* module = CUarmRobotModule::CreateModule();
		return (RobotModule*)module;
	}
	
	#define IID "RCT.Uarm_robot_module_v100"

	#define SERVO_1 0
	#define SERVO_2 1
	#define SERVO_3 2
  #define SERVO_4 3

  #define X 4
  #define Y 5
	#define Z 6

  #define PUMP_AXIS 7
  #define LINEAR_AXIS_GROUP 4

  unsigned int CUarmRobotModule::COUNT_AXIS = 9;

  // UarmRobotModule
  //////////////////////////
  CUarmRobotModule::CUarmRobotModule()
  	: m_colorPrintf_p(NULL), m_robot_functions(NULL), m_robot_axis(NULL), m_Debug(false), is_prepare_failed(false){
  	InitializeCriticalSection(&m_Cs);
  	CreateFuntions();

  	m_ModuleInfo.uid = IID;
  	m_ModuleInfo.digest = NULL;
  	m_ModuleInfo.mode = ModuleInfo::PROD;
  	m_ModuleInfo.version = BUILD_NUMBER;
  }

  CUarmRobotModule* CUarmRobotModule::CreateModule() {
  	return new CUarmRobotModule();
  }

  void CUarmRobotModule::CreateFuntions() {
  	m_robot_functions = new FunctionData*[COUNT_FUNCTIONS];
  	unsigned int function_id = 0;

  	{
  		FunctionData::ParamTypes* pt = new FunctionData::ParamTypes[3];
  		pt[0] = FunctionData::ParamTypes::FLOAT;
  		pt[1] = FunctionData::ParamTypes::FLOAT;
  		pt[2] = FunctionData::ParamTypes::FLOAT;

  		// 0
  		m_robot_functions[function_id] =
  			new FunctionData(function_id + 1, 3, pt, "move_to");
  		++function_id;
  	}

  	// 1
  	m_robot_functions[function_id] =
  		new FunctionData(function_id + 1, 0, NULL, "pump_on");
  	++function_id;

  	// 2
  	m_robot_functions[function_id] =
  		new FunctionData(function_id + 1, 0, NULL, "pump_off");
  	++function_id;

  	// 3
  	m_robot_functions[function_id] =
  		new FunctionData(function_id + 1, 0, NULL, "find_x");
  	++function_id;

  	// 4
  	m_robot_functions[function_id] =
  		new FunctionData(function_id + 1, 0, NULL, "find_y");
  	++function_id;

  	// 5
  	m_robot_functions[function_id] =
  		new FunctionData(function_id + 1, 0, NULL, "find_z");
  	++function_id;

  	{
  		FunctionData::ParamTypes* pt = new FunctionData::ParamTypes[3];
  		pt[0] = FunctionData::ParamTypes::FLOAT;
  		pt[1] = FunctionData::ParamTypes::FLOAT;
  		pt[2] = FunctionData::ParamTypes::FLOAT;

  		// 6
  		m_robot_functions[function_id] =
  			new FunctionData(function_id + 1, 3, pt, "move_to_at_once");
  		++function_id;
  	}

    {
  	  FunctionData::ParamTypes* pt = new FunctionData::ParamTypes[3];
  	  pt[0] = FunctionData::ParamTypes::FLOAT;
  	  pt[1] = FunctionData::ParamTypes::FLOAT;
  	  pt[2] = FunctionData::ParamTypes::FLOAT;

  	  // 7
  	  m_robot_functions[function_id] =
  		  new FunctionData(function_id + 1, 3, pt, "move");
  	  ++function_id;
    }

    {
  	  FunctionData::ParamTypes* pt = new FunctionData::ParamTypes[4];
  	  pt[0] = FunctionData::ParamTypes::FLOAT;
  	  pt[1] = FunctionData::ParamTypes::FLOAT;
  	  pt[2] = FunctionData::ParamTypes::FLOAT;
  	  pt[3] = FunctionData::ParamTypes::FLOAT;

  	  // 8
  	  m_robot_functions[function_id] =
  		  new FunctionData(function_id + 1, 4, pt, "move_to_1");
  	  ++function_id;
    }

    {
  	  FunctionData::ParamTypes* pt = new FunctionData::ParamTypes[6];
  	  pt[0] = FunctionData::ParamTypes::FLOAT;
  	  pt[1] = FunctionData::ParamTypes::FLOAT;
  	  pt[2] = FunctionData::ParamTypes::FLOAT;
  	  pt[3] = FunctionData::ParamTypes::FLOAT;
  	  pt[4] = FunctionData::ParamTypes::FLOAT;
  	  pt[5] = FunctionData::ParamTypes::FLOAT;

  	  // 9
  	  m_robot_functions[function_id] =
  		  new FunctionData(function_id + 1, 6, pt, "move_to_2");
  	  ++function_id;
    }

    {
  	  FunctionData::ParamTypes* pt = new FunctionData::ParamTypes[4];
  	  pt[0] = FunctionData::ParamTypes::FLOAT;
  	  pt[1] = FunctionData::ParamTypes::FLOAT;
  	  pt[2] = FunctionData::ParamTypes::FLOAT;
  	  pt[3] = FunctionData::ParamTypes::FLOAT;

  	  // 10
  	  m_robot_functions[function_id] =
  		  new FunctionData(function_id + 1, 4, pt, "write_angles");
  	  ++function_id;
    }

    {
  	  FunctionData::ParamTypes* pt = new FunctionData::ParamTypes[1];
  	  pt[0] = FunctionData::ParamTypes::FLOAT;

  	  // 11
  	  m_robot_functions[function_id] =
  		  new FunctionData(function_id + 1, 1, pt, "rotate_servo4");
  	  ++function_id;
    }

    {
  	  FunctionData::ParamTypes* pt = new FunctionData::ParamTypes[1];
  	  pt[0] = FunctionData::ParamTypes::FLOAT;

  	  // 12
  	  m_robot_functions[function_id] =
  		  new FunctionData(function_id + 1, 1, pt, "read_angle_with_offset");
  	  ++function_id;
    }

    {
  	  FunctionData::ParamTypes* pt = new FunctionData::ParamTypes[1];
  	  pt[0] = FunctionData::ParamTypes::FLOAT;

  	  // 13
  	  m_robot_functions[function_id] =
  		  new FunctionData(function_id + 1, 1, pt, "read_angle");
    }
  }

  const struct ModuleInfo& CUarmRobotModule::getModuleInfo() {
  	return m_ModuleInfo;
  }

  #define GET_INT_INI_VALUE(dest, key)                         \
    try{                                                       \
      dest = Utils.getIniValueInt(axis_names[i].c_str(), key); \
    } catch (UarmError *e) {                                   \
      _error->append(e);                                       \
    }

  void CUarmRobotModule::prepare(colorPrintfModule_t* colorPrintf_p,
  	colorPrintfModuleVA_t* colorPrintfVA_p) {
  	m_colorPrintf_p = colorPrintfVA_p;
  	CUtils Utils("config.ini");

  	if (Utils.getIniError() > 0) {
  		printf("Can't load '%s' file!\n", Utils.getConfigPath().c_str());
  		is_prepare_failed = true;
  		return;
  	}

  	try {
  		int count_robots = Utils.getIniValueInt("main", "count_robots");
  		m_Debug = Utils.getIniValueInt("main", "is_debug") != 0;

  		vector<string> axis_names;
  		vector<ServoData> servo_data;
  		axis_names.push_back("servo1");
  		axis_names.push_back("servo2");
  		axis_names.push_back("servo3");
  		axis_names.push_back("servo4");
      axis_names.push_back("X");
      axis_names.push_back("Y");
      axis_names.push_back("Z");

  		int max = 0;
  		int min = 0;
  		int low = 0;
  		int top = 0;
  		bool increment = true;
  		double start_position = 0;
  		UarmError *axis_errors = new UarmError();
  		for (int i = 0; i< axis_names.size(); ++i) {
  			UarmError *_error = new UarmError();

  			GET_INT_INI_VALUE(min, "min")
  				GET_INT_INI_VALUE(max, "max")
  				if (min < -180 || min > 180) {
  					char buff[1024];
  					sprintf_s(buff, "Wrong min limit '%d', should be in borders [-180,180]", min);
  					_error->append(new UarmError(buff));
  				}
  			if (max < -180 || max > 180) {
  				char buff[1024];
  				sprintf_s(buff, "Wrong max limit '%d', should be in borders [-180,180]", max);
  				_error->append(new UarmError(buff));
  			}
  			if (max < min) {
  				_error->append(new UarmError("min bigger than max"));
  			}
  			GET_INT_INI_VALUE(increment, "increment")
  				if (increment){
  					GET_INT_INI_VALUE(low, "low_control_value")
  						GET_INT_INI_VALUE(top, "top_control_value")
  						if (top < low) {
  							_error->append(new UarmError("low_control_value bigger than top_control_value"));
  						}
  				}
  			GET_INT_INI_VALUE(start_position, "start_position")

  				if (!_error->isEmpty()){
  					char buff[1024];
  					sprintf_s(buff, "Axis '%s' error: %s", axis_names[i].c_str(), _error->emit().c_str());
  					axis_errors->append(new UarmError(buff));
  					continue;
  				}
  				else {
  					delete _error;
  				}

  				if (increment) {
  					axis_settings.push_back(AxisMinMax(low, top, axis_names[i]));
  				}
  				else {
  					axis_settings.push_back(AxisMinMax(min, max, axis_names[i]));
  				}
  				servo_data.push_back(ServoData(i, min, max, start_position, increment));
  		}

  		if (!axis_errors->isEmpty()){
  			throw axis_errors;
  		}
  		else{
  			delete axis_errors;
  		}
  		axis_settings.push_back(AxisMinMax(0, 1, "pump"));
  		axis_settings.push_back(AxisMinMax(0, 1, "locked"));

  		m_robot_axis = new AxisData*[COUNT_AXIS];

  		for (int axis_id = 0; axis_id < axis_settings.size(); ++axis_id) {
  			m_robot_axis[axis_id]
  				= new AxisData(axis_id, axis_settings[axis_id]._max, axis_settings[axis_id]._min, axis_settings[axis_id].name.c_str());
  		}

  		for (int i(0); i < count_robots; i++) {
  			string robot_section = "robot_port_" + to_string(i);
  			string robot_port = Utils.getIniValueStr("main", robot_section);

  			try {
  				CUarmRobot* Robot = CUarmRobot::CreateRobot(robot_port, servo_data, m_Debug);
  				m_avalible_robots.push_back(Robot);
  			}
  			catch (System::Exception ^ e) {
  				String ^ Str = e->ToString();
  				string ErrStr("cant't create Robot (idx:");
  				ErrStr += to_string(i);
  				ErrStr += " port:";
  				ErrStr += robot_port;
  				ErrStr += ")";
  				ErrStr += "\n";
  				ErrStr += "\n";

  				ErrStr += CUtils::Str_To_StdStr(Str);

  				printf(ErrStr.c_str());
  			}
  		}
  	}
  	catch (UarmError *e) {
  		is_prepare_failed = true;
  		COUNT_AXIS = 0;
  		printf("Axis Errors:\n %s ", e->emit().c_str());
      delete e;
  	}
  }

  FunctionData** CUarmRobotModule::getFunctions(unsigned int* count_functions) {
  	*count_functions = COUNT_FUNCTIONS;
  	return m_robot_functions;
  }

  AxisData** CUarmRobotModule::getAxis(unsigned int* count_axis) {
  	*count_axis = COUNT_AXIS;
  	if (!m_robot_axis){
  		*count_axis = 0;
  	}
  	return m_robot_axis;
  }

  void* CUarmRobotModule::writePC(unsigned int* buffer_length) {
  	*buffer_length = 0;
  	return NULL;
  }

  void CUarmRobotModule::destroy() {
  	DeleteCriticalSection(&m_Cs);
  	for (unsigned int j = 0; j < COUNT_FUNCTIONS; ++j) {
  		if (m_robot_functions[j]->count_params) {
  			delete[] m_robot_functions[j]->params;
  		}
  		delete m_robot_functions[j];
  	}
  	for (int j = 0; j < COUNT_AXIS; ++j) {
  		delete m_robot_axis[j];
  	}
  	delete[] m_robot_functions;
  	delete[] m_robot_axis;
  	delete this;
  }

  CUarmRobot* CUarmRobot::CreateRobot(string port_name,
  	vector<ServoData> servo_data, bool isDebug) {
  	CUarmRobot* Robot = new CUarmRobot(port_name, servo_data, isDebug);
  	return Robot;
  }

  ServoData::ServoData(int number, int _min, int _max,
  	double start_position, bool increment)
  	: servo_number(number),
  	_min(_min),
  	_max(_max),
  	start_position(0),
  	increment(increment),
  	current_position(start_position){};

  int CUarmRobotModule::init() {
  	return is_prepare_failed ? 1 : 0;
  }

  CUarmRobot* CUarmRobotModule::FindRobot(Robot* robot /*= NULL*/,
  	bool avalible /*= true*/) {
  	CUarmRobot* res(NULL);
  	try {
  		EnterCriticalSection(&m_Cs);
  		for (CAvalibleRobotsList::iterator i(m_avalible_robots.begin());
  			i != m_avalible_robots.end(); ++i) {
  			bool found_avalible(!robot && (*i)->IsAvalible() == avalible);
  			bool found_robot(robot && (*i) == robot);

  			if (found_avalible || found_robot) {
  				res = *i;
  				break;
  			}
  		}
  		LeaveCriticalSection(&m_Cs);
  	}
  	catch (...) {
  		LeaveCriticalSection(&m_Cs);
  	}

  	return res;
  }

  Robot* CUarmRobotModule::robotRequire() { return FindRobot(NULL); }

  void CUarmRobotModule::robotFree(Robot* robot) {
  	CUarmRobot* foundRobot(FindRobot(robot));
  	if (foundRobot) {
  		foundRobot->setAvalible(true);
  	}
  }

  void CUarmRobotModule::final() {
  	try {
  		EnterCriticalSection(&m_Cs);
  		for (CAvalibleRobotsList::iterator i(m_avalible_robots.begin());
  			i != m_avalible_robots.end(); ++i) {
  			(*i)->OnRobotFree();
  			delete (*i);
  		}
  		LeaveCriticalSection(&m_Cs);
  	}
  	catch (...) {
  		LeaveCriticalSection(&m_Cs);
  	}
  }

  // CUarmRobot
  /////////////////////////////////////////
  CUarmRobot::CUarmRobot(string port_name, vector<ServoData> servo_data, bool isDebug)
  	: m_colorPrintf_p(NULL),
  	m_isAvalible(true),
  	m_port_name(port_name),
  	servo_data(servo_data),
  	m_Debug(isDebug),
  	m_locked(true) {
  	String ^ Str = gcnew String(port_name.c_str());
  	m_Uarm = gcnew UArm(Str);
  	m_Uarm->setDebug(m_Debug);
  }

  CUarmRobot::CUarmRobot(string port_name, vector<ServoData> servo_data, bool isDebug, int delay)
  	: m_colorPrintf_p(NULL),
  	m_isAvalible(true),
  	m_port_name(port_name),
  	servo_data(servo_data),
  	m_Debug(isDebug),
  	m_locked(true) {
  	String ^ Str = gcnew String(port_name.c_str());
  	m_Uarm = gcnew UArm(Str, isDebug, delay);
  }

  void CUarmRobot::prepare(colorPrintfRobot_t* colorPrintf_p,
  	colorPrintfRobotVA_t* colorPrintfVA_p) {
  	m_colorPrintf_p = colorPrintfVA_p;
  	m_Uarm->attachAll();
  	//   Записываем начальное положение серв
    servo_data[X].current_position = m_Uarm->findX();
    servo_data[Y].current_position = m_Uarm->findY();
    servo_data[Z].current_position = m_Uarm->findZ();
  }

  FunctionResult* CUarmRobot::executeFunction(CommandMode mode,
  	system_value command_index,
  	void** args) {
  	if ((command_index < 1) ||
  		(command_index >(int)CUarmRobotModule::COUNT_FUNCTIONS))
  		return NULL;

  	switch (command_index) {
  	case 1: {
  		double x = *(double*)args[0];
  		double y = *(double*)args[1];
  		double z = *(double*)args[2];
  		try {
  			m_Uarm->MoveTo(x, y, z);
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}

  		break;
  	}
  	case 2: {
  		try {
  			m_Uarm->PumpON();
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}
  		break;
  	}
  	case 3: {
  		try {
  			m_Uarm->PumpOFF();
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}
  		break;
  	}

  	case 4: {
  		try {
  			FunctionResult* Res =
  				new FunctionResult(FunctionResult::VALUE, m_Uarm->findX());
  			return Res;
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}
  	}
  	case 5: {
  		try {
  			FunctionResult* Res =
  				new FunctionResult(FunctionResult::VALUE, m_Uarm->findY());
  			return Res;
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}
  	}
  	case 6: {
  		try {
  			FunctionResult* Res =
  				new FunctionResult(FunctionResult::VALUE, m_Uarm->findZ());
  			return Res;
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}
  	}

  	case 7: {
  		double x = *(double*)args[0];
  		double y = *(double*)args[1];
  		double z = *(double*)args[2];

  		try {
  			m_Uarm->MoveToAtOnce(x, y, z);
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}
  		break;
  	}

  	case 8: {
  		double x = *(double*)args[0];
  		double y = *(double*)args[1];
  		double z = *(double*)args[2];

  		try {
  			m_Uarm->Move(x, y, z);
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}
  		break;
  	}

  	case 9: {
  		double x = *(double*)args[0];
  		double y = *(double*)args[1];
  		double z = *(double*)args[2];
  		double t = *(double*)args[3];

  		try {
  			m_Uarm->MoveTo(x, y, z, (int)t);
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}
  		break;
  	}

  	case 10: {
  		double x = *(double*)args[0];
  		double y = *(double*)args[1];
  		double z = *(double*)args[2];
  		double t = *(double*)args[3];
  		double r = *(double*)args[4];
  		double th4 = *(double*)args[5];

  		try {
  			if (!CUtils::CheckAnlgeVal(th4)) return RiseException();

  			m_Uarm->MoveTo(x, y, z, (int)t, (int)r, (int)th4);
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}

  		break;
  	}

  	case 11: {
  		double th1 = *(double*)args[0];
  		double th2 = *(double*)args[1];
  		double th3 = *(double*)args[2];
  		double th4 = *(double*)args[3];
  		try {
  			if (!CUtils::CheckAnlgeVal(th1) || !CUtils::CheckAnlgeVal(th2) ||
  				!CUtils::CheckAnlgeVal(th3) || !CUtils::CheckAnlgeVal(th4))
  				return RiseException();

  			m_Uarm->writeAngles(th1, th2, th3, th4);
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}
  		break;
  	}
  	case 12: {
  		double th4 = *(double*)args[0];

  		try {
  			if (!CUtils::CheckAnlgeVal(th4)) return RiseException();

  			m_Uarm->writeTheta_4((int)th4);
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}
  		break;
  	}
  	case 13: {
  		try {
  			double servo_num = *(double*)args[0];

  			if (!CUtils::CheckServoIdx(servo_num))
  				return RiseException();

  			FunctionResult* Res =
  				new FunctionResult(FunctionResult::VALUE, m_Uarm->ReadAngleWithOffset((int)servo_num));
  			return Res;
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}
  	}
  	case 14: {
  		try {
  			double servo_num = *(double*)args[0];

  			if (!CUtils::CheckServoIdx(servo_num))
  				return RiseException();

  			FunctionResult* Res =
  				new FunctionResult(FunctionResult::VALUE, m_Uarm->ReadAngle((int)servo_num));
  			return Res;
  		}
  		catch (Exception ^ E) {
  			if (m_Debug) CUtils::printError(E);

  			return RiseException();
  		}
  	}
  	}

  	return NULL;
  }

  void CUarmRobot::axisControl(system_value axis_index, variable_value value) {
    if (axis_index != CUarmRobotModule::LOCKED_IDX && m_locked) return;
    if (axis_index == CUarmRobotModule::LOCKED_IDX){
      m_locked = (!value != 0.0f);
    } else if (axis_index == PUMP_AXIS){
      try {
        if (value == 1)
          m_Uarm->PumpON();
        else
          m_Uarm->PumpOFF();
      }
      catch (Exception ^ E) {
        if (m_Debug) CUtils::printError(E);
      }
      catch (...) {
      }
    } else if (axis_index < PUMP_AXIS) {
      if (servo_data[axis_index].increment){
        system_value test_value = servo_data[axis_index].current_position + value;
        if (test_value < servo_data[axis_index]._min ||
            test_value > servo_data[axis_index]._max) {
          printf("Axis '%d' limit error\n", axis_index);
          return;
        }
        servo_data[axis_index].current_position = test_value;
      } else {
        servo_data[axis_index].current_position = value;
      }
      try {
        if (axis_index < LINEAR_AXIS_GROUP) {
          double th1 = servo_data[SERVO_1].current_position;
          double th2 = servo_data[SERVO_2].current_position;
          double th3 = servo_data[SERVO_3].current_position;
          double th4 = servo_data[SERVO_4].current_position;
          m_Uarm->writeAngles(th1, th2, th3, th4);
        } else {
          double current_x = servo_data[X].current_position;
          double current_y = servo_data[Y].current_position;
          double current_z = servo_data[Z].current_position;
          m_Uarm->Move(current_x, current_y, current_z);
        }
      }
      catch (Exception ^ E) {
        if (m_Debug) CUtils::printError(E);
      }
      catch (...) {
      }
    }
  }
};



