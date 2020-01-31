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

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

class Module: public yarp::os::RFModule
{
protected:

    // Parameters
    std::string moduleName;

    // Ports
    yarp::os::Port commandPort;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelBgr> > imagePort;

    yarp::dev::PolyDriver robotDevice;
    // Interfaces can only be used as a pointer, they cannot be instatiated
    yarp::dev::IEncoders        *iencoders;

    int nJnts;
    cv::Mat image;

public:

    // Function called once at the startup.
    // Do all init configuration here
    bool configure(yarp::os::ResourceFinder &rf)
    {
        // let's get the module name
        moduleName = rf.check("name",yarp::os::Value("yarp-basics")).asString();

        yInfo() << "Configuring the module with the following parameters: \n";
        yInfo() << "name is" << moduleName << "\n";

        ResourceFinder r;
        r.setDefaultContext("tutorial_yarp-basics");
        std::string filePath = r.findFileByName("programmersLife.bmp");

        image = cv::imread(filePath.c_str(), CV_LOAD_IMAGE_COLOR);
        if(!image.data)
        {
            yError() << "Error";
            return false;
        }

        // open all ports
        string portName = "/" + moduleName;
        bool ret = commandPort.open(portName);
        if(!ret)
        {
            yError() << "Cannot open port " << portName;
            return false;
        }

        // opening port to publish image
        imagePort.open("/imagePort:o");

        // open the device driver to access the robot
        yarp::os::Property option;
        option.put("device", "remote_controlboard");
        option.put("remote", "/icubSim/head"); // where we connect to
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

    double getPeriod()
    {
        return 0.5; // module periodicity (seconds)
    }

    bool updateModule()
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
            else if(val.asString() == "enc")
            {
                int jnt = 0;
                if (b.size()>1)
                    jnt = b.get(1).asInt();
                double enc = 0;

                if (jnt >= nJnts)
                {
                    yError() << "Head has" << nJnts << "joints";
                    b.clear();
                    b.addString("Head has " + to_string(nJnts) + " joints");
                }
                else
                {
                    yInfo() << "Reading joint" <<jnt;

                    // Read encoder values:
                    if(iencoders)
                        iencoders->getEncoder(jnt,&enc);
                    b.clear();
                    b.addString("Joint " + to_string(jnt) + " position is:");
                    b.addDouble(enc);
                }

                commandPort.write(b);
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

    // Catch the CTRL+C and unlock any resources which may
    // hang on closure
    bool interruptModule()
    {
        // wake up port from read.
        commandPort.interrupt();
        return true;
    }

    bool close()
    {
        yInfo()<<"closing module";
        commandPort.close();
        return true;
    }

};

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
//     rf.setDefaultContext("tutorial_yarp-basic");
//     rf.setVerbose(true);

    Module module;
    module.runModule(rf);

    yInfo()<<"Main returning...";
    return EXIT_SUCCESS;
}

