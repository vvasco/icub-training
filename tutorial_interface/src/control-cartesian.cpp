// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
//
// A tutorial on how to use the Gaze Interface.
//
// Author: Ugo Pattacini - <ugo.pattacini@iit.it>

#include <cstdio>
#include <cmath>

#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>
#include <yarp/math/Math.h>

#include <yarp/dev/Drivers.h>
#include <yarp/dev/CartesianControl.h>
#include <yarp/dev/PolyDriver.h>

#include "src/controlCartesian_IDL.h"

#define MAX_TORSO_PITCH     30.0    // [deg]

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;

class CtrlThread: public PeriodicThread, public controlCartesian_IDL
{
protected:
    PolyDriver         clientCart;
    ICartesianControl *icart;

    BufferedPort<Bottle> targetIn;
    BufferedPort<Bottle> portScope;
    RpcServer portRpc;

    ResourceFinder *rf;

    Vector xd;
    Vector od;

    Vector home_pos;
    Vector home_ori;
    int startup_context_id;
    bool starting;

public:
    CtrlThread() : PeriodicThread(1.0), rf(nullptr) { }

    /********************************************************/
    bool attach(yarp::os::RpcServer &source)
    {
        return this->yarp().attachAsServer(source);
    }

    /********************************************************/
    bool stop_cartesian() override
    {
        // back to home position
        yInfo()<<"Homing";
        starting = false;
        return icart->goToPoseSync(home_pos,home_ori);
    }

    /********************************************************/
    bool start_cartesian() override
    {
        starting = true;
        return true;
    }

    /********************************************************/
    bool enable_torso(const int32_t pitch, const int32_t roll, const int32_t yaw) override
    {
        Vector newDof, curDof;
        icart->getDOF(curDof);
        newDof=curDof;
        yInfo()<<"Current DoFs"<<newDof.toString();

        // enable the torso yaw and pitch
        // disable the torso roll
        newDof[0]=pitch; // torso pitch
        newDof[1]=roll; // torso roll
        newDof[2]=yaw; // torso yaw

        // send the request for dofs reconfiguration
        return icart->setDOF(newDof,curDof);
    }

    void setRF(ResourceFinder &rf)
    {
        this->rf=&rf;
    }

    bool threadInit() override
    {
        string name=rf->check("name",Value("controller-cartesian")).asString();
        string robot=rf->check("robot",Value("icubSim")).asString();

        // open a client interface to connect to the cartesian server of the simulator
        // we suppose that:
        //
        // 1 - the iCub simulator is running
        //     (launch: iCub_SIM)
        //
        // 2 - the cartesian server is running
        //     (launch: yarprobotinterface --context simCartesianControl)
        //
        // 3 - the cartesian solver for the left arm is running too
        //     (launch: iKinCartesianSolver --context simCartesianControl --part left_arm)
        //
        Property option("(device cartesiancontrollerclient)");
        option.put("remote","/" + robot + "/cartesianController/left_arm");
        option.put("local","/cartesian_client/left_arm");

        if (!clientCart.open(option))
            return false;

        // open the view
        clientCart.view(icart);

        // latch the controller context in order to preserve
        // it after closing the module
        // the context contains the dofs status, the tracking mode,
        // the resting positions, the limits and so on.
        icart->storeContext(&startup_context_id);

        // set trajectory time
        icart->setTrajTime(1.0);

        // get the torso dofs
        Vector newDof, curDof;
        icart->getDOF(curDof);
        newDof=curDof;
        yInfo()<<"Current DoFs"<<newDof.toString();

        // enable the torso yaw and pitch
        // disable the torso roll
        newDof[0]=0; // torso pitch
        newDof[1]=0; // torso roll
        newDof[2]=0; // torso yaw

        // send the request for dofs reconfiguration
        icart->setDOF(newDof,curDof);

        // impose some restriction on the torso pitch
        limitTorsoPitch();

        // print out some info about the controller
        Bottle info;
        icart->getInfo(info);
        yInfo()<<info.toString();

        // get home position
        home_pos.resize(3);
        home_ori.resize(4);
        icart->getPose(home_pos,home_ori);

        // open ports
        targetIn.open("/" + name + "/target:i");
        portScope.open("/" + name + "/scope:o");
        portRpc.open("/" + name + "/rpc");
        attach(portRpc);

        xd.resize(3);
        od.resize(4);

        starting = false;

        return true;
    }

    void afterStart(bool s) override
    {
        if (s)
            yInfo()<<"Thread started successfully";
        else
            yInfo()<<"Thread did not start";

    }

    void run() override
    {
        if (starting)
        {
            Bottle *t=targetIn.read(false);
            if (t!=nullptr)
            {
                if (Bottle *pos=t->get(0).asList())
                {
                    if (pos->size()>=3)
                    {
                        // we read the target position from the gaze
                        xd[0]=pos->get(0).asDouble();
                        xd[1]=pos->get(1).asDouble();
                        xd[2]=pos->get(2).asDouble()-0.1;

                        // we keep the orientation of the arm constant:
                        // we want the middle finger to point forward (end-effector x-axis)
                        // with the palm turned down (end-effector y-axis points leftward);
                        // to achieve that it is enough to rotate the root frame of pi around z-axis
                        od[0]=0.0; od[1]=0.0; od[2]=1.0; od[3]=M_PI;

                        // go to the target :)
                        // (in streaming)
                        icart->goToPose(xd,od);

                        // some verbosity
                        Vector x,o;
                        printStatus(x,o);

                        // send information to yarpscope
                        Bottle &scope=portScope.prepare();
                        scope.clear();
                        scope.addDouble(xd[0]);
                        scope.addDouble(x[0]);
                        scope.addDouble(xd[1]);
                        scope.addDouble(x[1]);
                        scope.addDouble(xd[2]);
                        scope.addDouble(x[2]);
                        scope.addDouble(od[3]*(180.0/M_PI));
                        scope.addDouble(o[3]*(180.0/M_PI));
                        portScope.write();
                    }
                }
            }
        }
    }

    void threadRelease() override
    {
        // we go back to home position
        stop_cartesian();

        // we require an immediate stop
        // before closing the client for safety reason
        icart->stopControl();

        // it's a good rule to restore the controller
        // context as it was before opening the module
        icart->restoreContext(startup_context_id);

        clientCart.close();

        targetIn.close();
        portScope.close();
        portRpc.close();
    }

    void limitTorsoPitch()
    {
        int axis=0; // pitch joint
        double min, max;

        // sometimes it may be helpful to reduce
        // the range of variability of the joints;
        // for example here we don't want the torso
        // to lean out more than 30 degrees forward

        // we keep the lower limit
        icart->getLimits(axis,&min,&max);
        icart->setLimits(axis,min,MAX_TORSO_PITCH);
    }

    void printStatus(Vector &x, Vector &o)
    {
        Vector xdhat,odhat,qdhat;

        // we get the current arm pose in the
        // operational space
        icart->getPose(x,o);

        // we get the final destination of the arm
        // as found by the solver: it differs a bit
        // from the desired pose according to the tolerances
        icart->getDesired(xdhat,odhat,qdhat);

        double e_x=norm(xdhat-x);
        double e_o=norm(odhat-o);

        fprintf(stdout,"+++++++++\n");
        fprintf(stdout,"xd          [m] = %s\n",xd.toString().c_str());
        fprintf(stdout,"xdhat       [m] = %s\n",xdhat.toString().c_str());
        fprintf(stdout,"x           [m] = %s\n",x.toString().c_str());
        fprintf(stdout,"od        [rad] = %s\n",od.toString().c_str());
        fprintf(stdout,"odhat     [rad] = %s\n",odhat.toString().c_str());
        fprintf(stdout,"o         [rad] = %s\n",o.toString().c_str());
        fprintf(stdout,"norm(e_x)   [m] = %g\n",e_x);
        fprintf(stdout,"norm(e_o) [rad] = %g\n",e_o);
        fprintf(stdout,"---------\n\n");
    }
};


class CtrlModule: public RFModule
{
protected:
    CtrlThread thr;

public:
    bool configure(ResourceFinder &rf) override
    {
        // retrieve command line options in the form of "--period 0.01"
        double period=rf.check("period",Value(0.1)).asDouble();

        // set the thread period in [s]
        thr.setPeriod(period);

        // pass on the resource finder
        thr.setRF(rf);

        return thr.start();
    }

    bool close() override
    {
        thr.stop();
        return true;
    }

    double getPeriod() override
    {
        return 1.0;
    }

    bool updateModule() override
    {
        return true;
    }
};


int main(int argc, char *argv[])
{
    Network yarp;
    if (!yarp.checkNetwork())
    {
        yError()<<"YARP doesn't seem to be available";
        return EXIT_FAILURE;
    }

    ResourceFinder rf;
    rf.setDefaultContext("tutorial_cartesian-interface");
    rf.configure(argc,argv);

    CtrlModule mod;
    return mod.runModule(rf);
}



