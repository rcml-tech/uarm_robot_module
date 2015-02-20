#ifndef UARM_ROBOT_MODULE_H
#define	UARM_ROBOT_MODULE_H

class UarmRobot : public Robot {
	private:
#ifdef _DEBUG
		bool checkingSerial;
		unsigned int tid;
		HANDLE thread_handle;
		CRITICAL_SECTION cheking_mutex;
#endif
		Serial *SP;

    public: 
		bool is_aviable;
		UarmRobot::UarmRobot(Serial *SP) : is_aviable(true), SP(SP) {}
		FunctionResult* executeFunction(regval command_index, regval *args);
		void axisControl(regval axis_index, regval value);
#ifdef _DEBUG
		void startCheckingSerial();
		void checkSerial();
#endif
        ~UarmRobot();
};
typedef std::vector<UarmRobot*> v_connections;
typedef v_connections::iterator v_connections_i;

class UarmRobotModule : public RobotModule {
	v_connections aviable_connections;
	FunctionData **robot_functions;
	AxisData **robot_axis;

	public:
		UarmRobotModule();
		const char *getUID();
		int init();
		FunctionData** getFunctions(int *count_functions);
		AxisData** getAxis(int *count_axis);
		Robot* robotRequire();
		void robotFree(Robot *robot);
		void final();
		void destroy();
		~UarmRobotModule() {};
};

#define ADD_ROBOT_FUNCTION(FUNCTION_NAME, COUNT_PARAMS, GIVE_EXCEPTION) \
	robot_functions[function_id] = new FunctionData; \
	robot_functions[function_id]->command_index = function_id + 1; \
	robot_functions[function_id]->count_params = COUNT_PARAMS; \
	robot_functions[function_id]->give_exception = GIVE_EXCEPTION; \
	robot_functions[function_id]->name = FUNCTION_NAME; \
	++function_id;

#define ADD_ROBOT_AXIS(AXIS_NAME, UPPER_VALUE, LOWER_VALUE) \
	robot_axis[axis_id] = new AxisData; \
	robot_axis[axis_id]->axis_index = axis_id + 1; \
	robot_axis[axis_id]->upper_value = UPPER_VALUE; \
	robot_axis[axis_id]->lower_value = LOWER_VALUE; \
	robot_axis[axis_id]->name = AXIS_NAME; \
	++axis_id;

union WordToBytes {
    short int i;
	char c[2];
};

#endif	/* SERVO_ROBOT_MODULE_H */