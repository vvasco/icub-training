/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Alberto Cardellino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <map>
#include <string>

#include <yarp/os/LogStream.h>
#include <yarp/os/Port.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/BufferedPort.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>

#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>

#include <yarp/cv/Cv.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "src/yarpBasics_IDL.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

class Module: public yarp::os::RFModule, public yarpBasics_IDL
{
    // Parameters
    std::string moduleName;
    std::string robotName;

    // Ports
    yarp::os::Port commandPort;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelBgr> > imagePort;
    yarp::os::RpcServer rpcPort;

    yarp::dev::PolyDriver robotDevice;
    // Interfaces can only be used as a pointer, they cannot be instatiated
    yarp::dev::IEncoders        *iencoders;

    int nJnts;
    cv::Mat image;

    /********************************************************/
    bool attach(yarp::os::RpcServer &source) override
    {
        return this->yarp().attachAsServer(source);
    }

    /********************************************************/
    // Interface implementation of idl service
    double get_enc(const int jnt) override
    {
        double enc = 0;
        if (jnt >= nJnts)
        {
            yError() << "Head has" << nJnts << "joints";
        }
        else
        {
            yInfo() << "Reading joint" <<jnt;

            // Read encoder values:
            if(iencoders)
                iencoders->getEncoder(jnt,&enc);
        }
        return enc;
    }

    /********************************************************/
    // Function called once at the startup.
    // Do all init configuration here
    bool configure(yarp::os::ResourceFinder &rf) override
    {
        // let's get the module name
        moduleName = rf.check("name",yarp::os::Value("yarp-basics")).asString();
        robotName = rf.check("robot",yarp::os::Value("icubSim")).asString();

        yInfo() << "Configuring the module with the following parameters: \n";
        yInfo() << "name is" << moduleName;
        yInfo() << "robot is" << robotName << "\n";

        std::string filePath = rf.findFileByName("programmersLife.bmp");

        image = cv::imread(filePath.c_str(), CV_LOAD_IMAGE_COLOR);
        if(!image.data)
        {
            yError() << "Cannor open image";
            return false;
        }
        yInfo() << "Found image in" << filePath;

        // open all ports
        std::string portName = "/" + moduleName;
        bool ret = commandPort.open(portName);
        if(!ret)
        {
            yError() << "Cannot open port " << portName;
            return false;
        }

        // opening port to publish image
        imagePort.open("/" + moduleName + "/image:o");

        // open and attach rpc port
        rpcPort.open("/" + moduleName + "/rpc");
        attach(rpcPort);

        // open the device driver to access the robot
        yarp::os::Property option;
        option.put("device", "remote_controlboard");
        option.put("remote", "/" + robotName + "/head"); // where we connect to
        option.put("local",  "/" + moduleName + "/client"); // local name port

        if (!robotDevice.open(option))
        {
            yError() << "Unable to open the device driver";
            return false;
        }

        // get access to some robot interface
        robotDevice.view(iencoders);

        // retrieve number of axes
        iencoders->getAxes(&nJnts);

        return true;
    }

    /********************************************************/
    double getPeriod() override
    {
        return 0.5; // module periodicity (seconds)
    }

    /********************************************************/
    bool updateModule() override
    {
        // watchdog message
        yInfo()<< moduleName << "running happily...";

        // Read input from user
        Bottle b;
        commandPort.read(b);

        Value val = b.get(0);

        // If it is an integer, just add +1 and send it back (huge computation!!)
        if(val.isInt())
        {
            int c = b.get(0).asInt();
            b.clear();
            b.addString("Answer is");
            b.addInt(++c);
            commandPort.write(b);
        }

        // If it is a string check for known commands
        if(val.isString())
        {
            if(val.asString() == "quit")
            {
                yInfo() << "Received quit command";
                return false;
            }
            else if (val.asString() == "send_image")
            {
                // Publish the image
                yarp::sig::ImageOf<PixelBgr> &img = imagePort.prepare();
                img = yarp::cv::fromCvMat<PixelBgr>(image);
                imagePort.write();
            }
            else
                yError() << "Received unknown command " << val.toString();
        }
        return true;
    }

    /********************************************************/
    // Catch the CTRL+C and unlock any resources which may
    // hang on closure
    bool interruptModule() override
    {
        // wake up port from read.
        commandPort.interrupt();
        imagePort.interrupt();
        rpcPort.interrupt();
        return true;
    }

    /********************************************************/
    bool close() override
    {
        yInfo()<<"closing module";
        commandPort.close();
        imagePort.close();
        rpcPort.close();
        return true;
    }

};

/********************************************************/
int main(int argc, char * argv[])
{
    Network yarp;

    if(!yarp.checkNetwork())
    {
        yError() << "Cannot find yarp server";
        return EXIT_FAILURE;
    }

    ResourceFinder rf;
    rf.configure(argc, argv);
    rf.setDefaultContext("tutorial_yarp-basics");
    rf.setVerbose(true);

    Module module;
    module.runModule(rf);

    yInfo()<<"Main returning...";
    return EXIT_SUCCESS;
}

