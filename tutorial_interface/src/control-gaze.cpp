// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
//
// A tutorial on how to use the Gaze Interface.
//
// Author: Ugo Pattacini - <ugo.pattacini@iit.it>

#include <cstdio>
#include <cmath>
#include <deque>

#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>

#include <yarp/dev/Drivers.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/GazeControl.h>
#include <yarp/dev/PolyDriver.h>

#include "src/controlGaze_IDL.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

class CtrlThread: public PeriodicThread, public controlGaze_IDL

{
protected:
    PolyDriver        clientGaze;
    IGazeControl     *igaze;
    PolyDriver        clientEncs;
    IEncoders        *iencs;

    BufferedPort<Bottle> portL;
    BufferedPort<Bottle> portR;
    BufferedPort<Bottle> portOut;
    BufferedPort<Bottle> portScope;
    RpcServer portRpc;

    ResourceFinder *rf;

    Vector home_pos;
    int startup_context_id;
    int nAxes;
    bool starting;

public:
    CtrlThread() : PeriodicThread(1.0), rf(nullptr) { }

    /********************************************************/
    bool attach(yarp::os::RpcServer &source)
    {
        return this->yarp().attachAsServer(source);
    }

    /********************************************************/
    bool stop_gaze() override
    {
        // back to home position
        yInfo()<<"Homing";
        starting = false;
        return igaze->lookAtFixationPointSync(home_pos);
    }

    /********************************************************/
    bool start_gaze() override
    {
        starting = true;
        return true;
    }

    void setRF(ResourceFinder &rf)
    {
        this->rf=&rf;
    }

    bool threadInit() override
    {
        string name=rf->check("name",Value("controller-gaze")).asString();
        string robot=rf->check("robot",Value("icubSim")).asString();

        // open a client interface to connect to the gaze server
        // we suppose that:
        // 1 - the iCub simulator is running;
        // 2 - the gaze server iKinGazeCtrl is running and
        //     launched with the following options: "--from configSim.ini"
        Property optGaze("(device gazecontrollerclient)");
        optGaze.put("remote","/iKinGazeCtrl");
        optGaze.put("local", "/" + name + "/gaze_client");
        if (!clientGaze.open(optGaze))
            return false;

        Property optEncs("(device remote_controlboard)");
        optEncs.put("remote", "/" + robot + "/head"); // where we connect to
        optEncs.put("local",  "/" + name + "/client"); // local name port
        if (!clientEncs.open(optEncs))
            return false;

        // access the interfaces
        clientGaze.view(igaze);
        clientEncs.view(iencs);

        // retrieve number of axes
        iencs->getAxes(&nAxes);

        // latch the controller context in order to preserve
        // it after closing the module
        // the context contains the tracking mode, the neck limits and so on.
        igaze->storeContext(&startup_context_id);

        // set trajectory time:
        igaze->setNeckTrajTime(0.8);
        igaze->setEyesTrajTime(0.4);

        // put the gaze in tracking mode, so that
        // when the torso moves, the gaze controller 
        // will compensate for it
        igaze->setTrackingMode(true);

        // print out some info about the controller
        Bottle info;
        igaze->getInfo(info);
        yInfo()<<info.toString();

        // get home position
        home_pos.resize(3);
        igaze->getFixationPoint(home_pos);

        // open ports
        portL.open("/" + name + "/target/left:i");
        portR.open("/" + name + "/target/right:i");
        portOut.open("/" + name + "/target:o");
        portScope.open("/" + name + "/scope:o");
        portRpc.open("/" + name + "/rpc");
        attach(portRpc);

        // params
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
            // poll data from YARP network
            // "false" means non-blocking read
            Bottle *pTargetL=portL.read(false);
            Bottle *pTargetR=portR.read(false);

            // update local copies if
            // something has arrived
            if (pTargetL!=nullptr && pTargetR!=nullptr)
            {
                if (pTargetL->size()>=2 && pTargetR->size()>=2)
                {
                    Vector pxl(2),pxr(2);
                    pxl[0]=pTargetL->get(0).asInt();
                    pxl[1]=pTargetL->get(1).asInt();

                    pxr[0]=pTargetR->get(0).asInt();
                    pxr[1]=pTargetR->get(1).asInt();

                    // look at the target (sync method: waits for reply)
                    int cam=0; // image plane (0: left, 1:right)
    //                igaze->lookAtMonoPixel(cam,pxl);
    //                igaze->lookAtMonoPixelWithVergence(cam,pxl,1.0);
                    igaze->lookAtStereoPixels(pxl,pxr);

                    // the fixation point is defined with respect to the root frame attached to the robot's waist:
                    // x-axis points backward
                    // y-axis points rightward
                    // z-axis points upward
                    Vector x;

                    // we get the current fixation point in the
                    // operational space
                    igaze->getFixationPoint(x);
                    yInfo()<<"Looking at"<<x.toString(3,3);

                    // get current joint encoders
                    vector<double> encs(nAxes);
                    iencs->getEncoders(encs.data());

                    // send information to yarpscope
                    Bottle &scope=portScope.prepare();
                    scope.clear();
                    scope.addInt(pxl[0]);
                    scope.addInt(pxr[0]);
                    scope.addInt(pxl[1]);
                    scope.addInt(pxr[1]);
                    scope.addDouble(encs[3]);
                    scope.addDouble(encs[4]);
                    portScope.write();

                    // write to output port
                    Bottle &out=portOut.prepare();
                    out.clear();
                    out.addList().read(x);
                    portOut.write();
                }
            }
        }
    }

    void threadRelease() override
    {
        // go back to home position
        stop_gaze();

        // we require an immediate stop
        // before closing the client for safety reason
        igaze->stopControl();

        // it's a good rule to restore the controller
        // context as it was before opening the module
        igaze->restoreContext(startup_context_id);

        clientGaze.close();
        clientEncs.close();

        // close ports
        portL.close();
        portR.close();
        portOut.close();
        portScope.close();
        portRpc.close();
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
    rf.setDefaultContext("tutorial_gaze-interface");
    rf.configure(argc,argv);

    CtrlModule mod;
    return mod.runModule(rf);
}



