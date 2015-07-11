#ifndef UARM_ROBOT_MODULE_H
#define	UARM_ROBOT_MODULE_H

struct MainAngles {
    double right;
    double left;
    double rot;
    MainAngles() : right(0), left(0), rot(0) {};
    MainAngles(double right, double left, double rot) : right(right), left(left), rot(rot) {};
};

class UarmRobot : public Robot {
    Serial *SP;
    colorPrintfRobotVA_t *colorPrintf_p;

    void colorPrintf(ConsoleColor colors, const char *mask, ...);
    MainAngles calculate_angles(double x, double y, double z);

    public:
        bool is_aviable;
        UarmRobot(Serial *SP) : is_aviable(true), SP(SP) {};
        void prepare(colorPrintfRobot_t *colorPrintf_p, colorPrintfRobotVA_t *colorPrintfVA_p);
        FunctionResult* executeFunction(system_value command_index, void **args);
        void axisControl(system_value axis_index, variable_value value) {};
        ~UarmRobot();
};
typedef std::vector<UarmRobot*> v_connections;
typedef v_connections::iterator v_connections_i;

class UarmRobotModule : public RobotModule {
    boost::mutex connections_mutex;

    v_connections aviable_connections;
    FunctionData **robot_functions;
    AxisData **robot_axis;

    colorPrintfModuleVA_t *colorPrintf_p;

    public:
        UarmRobotModule();
        const char *getUID();
        void prepare(colorPrintfModule_t *colorPrintf_p, colorPrintfModuleVA_t *colorPrintfVA_p);

        //compiler only
        FunctionData** getFunctions(unsigned int *count_functions);
        AxisData** getAxis(unsigned int *count_axis);
        void *writePC(unsigned int *buffer_length);

        //intepreter - devices
        int init();
        Robot* robotRequire();
        void robotFree(Robot *robot);
        void final();

        //intepreter - program & lib
        void readPC(void *buffer, unsigned int buffer_length) {};

        //intepreter - program
        int startProgram(int uniq_index);
        int endProgram(int uniq_index);

        //destructor
        void destroy();
        ~UarmRobotModule() {};

    protected:
        void colorPrintf(ConsoleColor colors, const char *mask, ...);
};

#endif	/* SERVO_ROBOT_MODULE_H */