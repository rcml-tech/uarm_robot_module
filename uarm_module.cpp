#include "Stdafx.h"

#include "uarm_module.h"
#include <string>

#include "Utils.h"

using namespace System;

PREFIX_FUNC_DLL RobotModule* getRobotModuleObject()
{
	CUarmRobotModule* module = CUarmRobotModule::CreateModule();
	return (RobotModule*)module;
}

//UarmRobotModule
//////////////////////////
CUarmRobotModule::CUarmRobotModule() :
m_robot_functions(NULL),
m_robot_axis(NULL)
{
	InitializeCriticalSection(&m_Cs);
	CreateFuntions();
	CreateAxis();

	m_ModuleInfo.uid = "UIID";
	m_ModuleInfo.digest = "digest";
	m_ModuleInfo.mode = ModuleInfo::PROD;
	m_ModuleInfo.version = 1;
}

CUarmRobotModule* CUarmRobotModule::CreateModule()
{
	return new CUarmRobotModule();
}

void CUarmRobotModule::CreateFuntions()
{
	m_robot_functions = new FunctionData* [COUNT_FUNCTIONS];
	unsigned int function_id = 0;

	{
		FunctionData::ParamTypes *pt = new FunctionData::ParamTypes[3];
		pt[0] = FunctionData::ParamTypes::FLOAT;
		pt[1] = FunctionData::ParamTypes::FLOAT;
		pt[2] = FunctionData::ParamTypes::FLOAT;

		//0
		m_robot_functions[function_id] = new FunctionData(function_id + 1, 3, pt, "move_to");
		++function_id;
	}

	//1
	m_robot_functions[function_id] = new FunctionData(function_id + 1, 0, NULL, "pump_on");
	++function_id;

	//2
	m_robot_functions[function_id] = new FunctionData(function_id + 1, 0, NULL, "pump_off");
	++function_id;

	//3
	m_robot_functions[function_id] = new FunctionData(function_id + 1, 0, NULL, "find_x");
	++function_id;

	//4
	m_robot_functions[function_id] = new FunctionData(function_id + 1, 0, NULL, "find_y");
	++function_id;

	//5
	m_robot_functions[function_id] = new FunctionData(function_id + 1, 0, NULL, "find_z");
	++function_id;

	{
		FunctionData::ParamTypes *pt = new FunctionData::ParamTypes[3];
		pt[0] = FunctionData::ParamTypes::FLOAT;
		pt[1] = FunctionData::ParamTypes::FLOAT;
		pt[2] = FunctionData::ParamTypes::FLOAT;

		//6
		m_robot_functions[function_id] = new FunctionData(function_id + 1, 3, pt, "move_to_at_once");
		++function_id;
	}

	{
		FunctionData::ParamTypes *pt = new FunctionData::ParamTypes[3];
		pt[0] = FunctionData::ParamTypes::FLOAT;
		pt[1] = FunctionData::ParamTypes::FLOAT;
		pt[2] = FunctionData::ParamTypes::FLOAT;

		//7
		m_robot_functions[function_id] = new FunctionData(function_id + 1, 3, pt, "move");
		++function_id;
	}

	{
		FunctionData::ParamTypes *pt = new FunctionData::ParamTypes[4];
		pt[0] = FunctionData::ParamTypes::FLOAT;
		pt[1] = FunctionData::ParamTypes::FLOAT;
		pt[2] = FunctionData::ParamTypes::FLOAT;
		pt[3] = FunctionData::ParamTypes::FLOAT;

		//8
		m_robot_functions[function_id] = new FunctionData(function_id + 1, 4, pt, "move_to_1");
		++function_id;
	}

	{
		FunctionData::ParamTypes *pt = new FunctionData::ParamTypes[6];
		pt[0] = FunctionData::ParamTypes::FLOAT;
		pt[1] = FunctionData::ParamTypes::FLOAT;
		pt[2] = FunctionData::ParamTypes::FLOAT;
		pt[3] = FunctionData::ParamTypes::FLOAT;
		pt[4] = FunctionData::ParamTypes::FLOAT;
		pt[5] = FunctionData::ParamTypes::FLOAT;

		//9
		m_robot_functions[function_id] = new FunctionData(function_id + 1, 6, pt, "move_to_2");
		++function_id;
	}

	{
		FunctionData::ParamTypes *pt = new FunctionData::ParamTypes[4];
		pt[0] = FunctionData::ParamTypes::FLOAT;
		pt[1] = FunctionData::ParamTypes::FLOAT;
		pt[2] = FunctionData::ParamTypes::FLOAT;
		pt[3] = FunctionData::ParamTypes::FLOAT;

		//10
		m_robot_functions[function_id] = new FunctionData(function_id + 1, 4, pt, "write_angles");
		++function_id;
	}

	{
		FunctionData::ParamTypes *pt = new FunctionData::ParamTypes[1];
		pt[0] = FunctionData::ParamTypes::FLOAT;

		//11
		m_robot_functions[function_id] = new FunctionData(function_id + 1, 1, pt, "rotate_servo4");
	}
}

void CUarmRobotModule::CreateAxis()
{
	m_robot_axis = new AxisData*[COUNT_AXIS];

	unsigned int axis_id = 0;

	m_robot_axis[axis_id] = new AxisData(axis_id, 180, 0, "servo1");
	axis_id++;

	m_robot_axis[axis_id] = new AxisData(axis_id, 180, 0, "servo2");
	axis_id++;

	m_robot_axis[axis_id] = new AxisData(axis_id, 180, 0, "servo3");
	axis_id++;

	m_robot_axis[axis_id] = new AxisData(axis_id, 180, 0, "servo4");
	axis_id++;
	
	m_robot_axis[axis_id] = new AxisData(axis_id, 1, 0, "pump");
	axis_id++;

	m_robot_axis[axis_id] = new AxisData(axis_id, 180, 0, "locked");
	axis_id++;
}

const struct ModuleInfo& CUarmRobotModule::getModuleInfo()
{
	return m_ModuleInfo;
}

void CUarmRobotModule::prepare(colorPrintfModule_t* colorPrintf_p,
	colorPrintfModuleVA_t* colorPrintfVA_p)
{
	m_colorPrintf_p = colorPrintfVA_p;
}

FunctionData** CUarmRobotModule::getFunctions(unsigned int* count_functions)
{
	*count_functions = COUNT_FUNCTIONS;
	return  m_robot_functions;
}

AxisData** CUarmRobotModule::getAxis(unsigned int* count_axis)
{
	*count_axis = COUNT_AXIS;
	return m_robot_axis;
}

void* CUarmRobotModule::writePC(unsigned int* buffer_length)
{
	*buffer_length = 0;
	return NULL;
}

void CUarmRobotModule::destroy()
{
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

CUarmRobot* CUarmRobot::CreateRobot(std::string port_name, bool isDebug)
{
	CUarmRobot* Robot = new CUarmRobot(port_name, isDebug);
	return Robot;
}

int CUarmRobotModule::init()
{
	CUtils Utils("config.ini");

	if (Utils.getIniError() > 0)
	{
		printf("Can't load '%s' file!\n", Utils.getConfigPath());
		return 1;
	}
	try
	{
		int count_robots = Utils.getIniValueInt("main", "count_robots");
		m_Debug = Utils.getIniValueInt("main", "is_debug") != 0;

		for (int i(0); i < count_robots; i++)
		{
			std::string robot_section = "robot_port_" + std::to_string(i);
			std::string robot_port = Utils.getIniValueStr("main", robot_section);
			
			try
			{
				CUarmRobot* Robot = CUarmRobot::CreateRobot(robot_port, m_Debug);
				m_avalible_robots.push_back(Robot);
			}
			catch (System::Exception^ e)
			{
				String^ Str = e->ToString();
				std::string ErrStr("cant't create Robot (idx:");
				ErrStr += std::to_string(i);
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
	catch (...)
	{
		return 1;
	}


	return 0;
}

CUarmRobot* CUarmRobotModule::FindRobot(Robot* robot /*= NULL*/, bool avalible /*= true*/)
{
	CUarmRobot* res(NULL);
	try
	{
		EnterCriticalSection(&m_Cs);
		for (CAvalibleRobotsList::iterator i(m_avalible_robots.begin()); i != m_avalible_robots.end(); i++)
		{
			bool found_avalible(!robot && (*i)->IsAvalible() == avalible);
			bool found_robot(robot && (*i) == robot);

			if (found_avalible || found_robot)
			{
				res = *i;
				break;
			}

		}
		LeaveCriticalSection(&m_Cs);

	}
	catch (...)
	{
		LeaveCriticalSection(&m_Cs);
	}

	return res;
}

Robot* CUarmRobotModule::robotRequire()
{
	return FindRobot(NULL);
}

void CUarmRobotModule::robotFree(Robot* robot)
{
	CUarmRobot* foundRobot(FindRobot(robot));
	if (foundRobot)
	{
		foundRobot->setAvalible(true);
	}
}

void CUarmRobotModule::final()
{
	try
	{
		EnterCriticalSection(&m_Cs);
		for (CAvalibleRobotsList::iterator i(m_avalible_robots.begin()); i != m_avalible_robots.end(); i++)
		{
			(*i)->OnRobotFree();
			delete (*i);
		}
		LeaveCriticalSection(&m_Cs);
	}
	catch (...)
	{
		LeaveCriticalSection(&m_Cs);
	}
}

//CUarmRobot
/////////////////////////////////////////
CUarmRobot::CUarmRobot(std::string port_name, bool isDebug) :
m_isAvalible(true),
m_port_name(port_name),
m_Debug(isDebug),
m_locked(false)
{
	String^ Str = gcnew String(port_name.c_str());
	m_Uarm = gcnew UArm(Str);
	m_Uarm->setDebug(m_Debug);
}

CUarmRobot::CUarmRobot(std::string port_name, bool isDebug, int delay) :
m_isAvalible(true),
m_port_name(port_name),
m_Debug(isDebug),
m_locked(false)
{
	String^ Str = gcnew String(port_name.c_str());
	m_Uarm = gcnew UArm(Str, isDebug, delay);
}

void CUarmRobot::prepare(colorPrintfRobot_t* colorPrintf_p,
	colorPrintfRobotVA_t* colorPrintfVA_p)
{
	m_colorPrintf_p = colorPrintfVA_p;
	m_Uarm->attachAll();
}

FunctionResult* CUarmRobot::executeFunction(CommandMode mode, system_value command_index, void** args)
{

	if ((command_index < 1) || (command_index >(int) CUarmRobotModule::COUNT_FUNCTIONS))
		return NULL;

	switch (command_index)
	{
	case 1:
	{
		double x = *(double*)args[0];
		double y = *(double*)args[1];
		double z = *(double*)args[2];
		try
		{
			m_Uarm->MoveTo(x, y, z);
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);

			return RiseException();
		}

		break;
	}
	case 2:
	{
		try
		{
			m_Uarm->PumpON();
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);

			return RiseException();
		}
		break;
	}
	case 3:
	{
		try
		{
			m_Uarm->PumpOFF();
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);

			return RiseException();
		}
		break;
	}

	case 4:
	{
		try
		{
			FunctionResult* Res = new FunctionResult(FunctionResult::VALUE, m_Uarm->findX());
			return Res;
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);

			return RiseException();
		}
	}
	case 5:
	{
		try
		{
			FunctionResult* Res = new FunctionResult(FunctionResult::VALUE, m_Uarm->findY());
			return Res;
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);

			return RiseException();
		}
		
	}
	case 6:
	{
		try
		{
			FunctionResult* Res = new FunctionResult(FunctionResult::VALUE, m_Uarm->findZ());
			return Res;
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);

			return RiseException();
		}
		
	}

	case 7:
	{
		double x = *(double*)args[0];
		double y = *(double*)args[1];
		double z = *(double*)args[2];
		
		try
		{
			m_Uarm->MoveToAtOnce(x, y, z);
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);

			return RiseException();
		}
		break;
	}

	case 8:
	{
		double x = *(double*)args[0];
		double y = *(double*)args[1];
		double z = *(double*)args[2];

		try
		{
			m_Uarm->Move(x, y, z);
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);

			return RiseException();
		}
		break;
	}

	case 9:
	{
		double x = *(double*)args[0];
		double y = *(double*)args[1];
		double z = *(double*)args[2];
		double t = *(double*)args[3];

		try
		{
			m_Uarm->MoveTo(x, y, z, (int)t);
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);

			return RiseException();
		}
		break;

	}

	case 10:
	{
		double x = *(double*)args[0];
		double y = *(double*)args[1];
		double z = *(double*)args[2];
		double t = *(double*)args[3];
		double r = *(double*)args[4];
		double th4 = *(double*)args[5];

		try
		{
			if (!CUtils::CheckAnlgeVal(th4))
				return RiseException();

			m_Uarm->MoveTo(x, y, z, (int)t, (int)r, (int)th4);
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);

			return RiseException();
		}
		
		break;
	}

	case 11:
	{
		double th1 = *(double*)args[0];
		double th2 = *(double*)args[1];
		double th3 = *(double*)args[2];
		double th4 = *(double*)args[3];
		try
		{
			if (!CUtils::CheckAnlgeVal(th1) || !CUtils::CheckAnlgeVal(th2) || 
				!CUtils::CheckAnlgeVal(th3) || !CUtils::CheckAnlgeVal(th4))
				return RiseException();

			m_Uarm->writeAngles(th1, th2, th3, th4);
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);

			return RiseException();
		}
		break;
	}
	case 12:
	{
		double th4 = *(double*)args[0];

		try
		{
			if (!CUtils::CheckAnlgeVal(th4))
				return RiseException();

			m_Uarm->writeTheta_4((int)th4);
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);

			return RiseException();
		}
		break;
	}

	}

	return NULL;
}

void CUarmRobot::axisControl(system_value axis_index, variable_value value)
{
	if (axis_index != CUarmRobotModule::LOCKED_IDX && m_locked)
		return;

	variable_value th1(0), th2(0), th3(0), th4(0);

	switch (axis_index)
	{
	case 0: th1 = value;
		break;

	case 1: th2 = value;
		break;

	case 2: th3 = value;
		break;

	case 3: th4 = value;
		break;

	case 4:
	{
		try
		{
			if (value == 1)
				m_Uarm->PumpON();
			else
				m_Uarm->PumpOFF();
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);
		}
		catch (...)
		{

		}

		break;
	}

	case 5: m_locked = (value != 0.0f);
		break;

	default:
		break;
	}

	
	if (axis_index < 4)
	{
		try
		{
			m_Uarm->writeAngles(th1, th2, th3, th4);
		}
		catch (Exception^ E)
		{
			if (m_Debug)
				CUtils::printError(E);
		}
		catch (...)
		{

		}
	}

}