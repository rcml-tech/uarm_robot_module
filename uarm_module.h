
#ifndef UARM_ROBOT_MODULE_H
#define	UARM_ROBOT_MODULE_H
#endif

#pragma once

#include "Stdafx.h"

#include <stdio.h>
#include "current\module.h"
#include "current\robot_module.h"
#include <list>

#include <vcclr.h>

#include <Windows.h>

class CUarmRobot;
typedef std::list<CUarmRobot*> CAvalibleRobotsList;


class CUarmRobotModule : public RobotModule
{

public:
	const static unsigned int COUNT_FUNCTIONS = 12;
	const static unsigned int COUNT_AXIS = 6;
	const static unsigned int LOCKED_IDX = 5;

protected:
	CRITICAL_SECTION m_Cs;

	colorPrintfModuleVA_t* m_colorPrintf_p;


	ModuleInfo m_ModuleInfo;

	FunctionData** m_robot_functions;
	AxisData** m_robot_axis;

	CAvalibleRobotsList m_avalible_robots;

	bool m_Debug;

	CUarmRobotModule();

	void CreateFuntions();
	void CreateAxis();

	CUarmRobot* FindRobot(Robot* robot = NULL, bool avalible = true);

public:
	static CUarmRobotModule* CreateModule();

	// init
	virtual const struct ModuleInfo& getModuleInfo() override;
	virtual void prepare(colorPrintfModule_t* colorPrintf_p,
		colorPrintfModuleVA_t* colorPrintfVA_p) override;

	// compiler only
	virtual FunctionData** getFunctions(unsigned int* count_functions) override;
	virtual AxisData** getAxis(unsigned int* count_axis) override;
	virtual void* writePC(unsigned int* buffer_length) override;

	// intepreter - devices
	virtual int init() override;
	virtual Robot* robotRequire() override;
	virtual void robotFree(Robot* robot) override;
	virtual void final() override;

	// intepreter - program & lib
	virtual void readPC(void *buffer, unsigned int buffer_length) override
	{

	}

	// intepreter - program
	virtual int startProgram(int uniq_index) override
	{
		return 0;
	}

	virtual int endProgram(int uniq_index) override
	{
		return 0;
	}

	// destructor
	virtual void destroy() override;

};

using namespace UFACTORY::CC;

class CUarmRobot : public Robot
{
private:
	bool m_isAvalible;
	bool m_locked;
	bool m_Debug;
	colorPrintfRobotVA_t* m_colorPrintf_p;

	std::string m_port_name;

protected:
	
	gcroot<UArm^> m_Uarm;

	CUarmRobot(std::string port_name, bool isDebug);
	CUarmRobot(std::string port_name, bool isDebug, int delay);

	FunctionResult* RiseException()
	{
		return new FunctionResult(FunctionResult::EXCEPTION);
	}

public:
	void setAvalible(bool Value)
	{
		m_isAvalible = Value;
	}

	void OnRobotFree()
	{
		m_Uarm->detachAll();
	}

	bool IsAvalible()
	{
		return m_isAvalible;
	}

	static CUarmRobot* CreateRobot(std::string port_name, bool isDebug);

	virtual void prepare(colorPrintfRobot_t* colorPrintf_p, colorPrintfRobotVA_t* colorPrintfVA_p) override;

	virtual FunctionResult* executeFunction(CommandMode mode, system_value command_index,
		void** args) override;

	virtual void axisControl(system_value axis_index, variable_value value) override;
};
