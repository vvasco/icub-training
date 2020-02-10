#HSLIDE

### Yarp basics

#HSLIDE
#### Intro
@fa[arrow-down]

#VSLIDE
@snap[north]
### What is YARP
@snapend

@snap[west]
@ul[list-spaced-bullets text-08](false)
- YARP is a <span style="color:#e49436">middleware</span>:

   - set of libraries, protocols, GUIs and tools
   - to develop of high level application for robots
   - focus on modularity, code re-usage, flexibility and hw/sw abstraction.
@ulend
@snapend


#VSLIDE
@snap[north]
### How YARP communicates
@snapend

@snap[west]
@ul[list-spaced-bullets text-08](false)
- YARP ports:

   - communication entry point
- many clients can connect to a port
- a single connection can use:

  - different <span style="color:#e49436">protocols</span> and custom <span style="color:#e49436">carriers</span>
- run on different operating systems
@ulend
@snapend

@snap[east span-45]
![IMAGE](tutorial_yarp-basics/images/images-icub-training.png)
@snapend

#VSLIDE

@snap[north]
### Read-write example
@snapend

@snap[west]
@ul[list-spaced-bullets text-08](false)
- `yarp server` :

   - database of information about yarp network

- `yarp read /read` :

   - creates a read port
- `yarp write /write /read` :

   - creates a write port connected to the read port
@ulend
@snapend

@snap[east span-45]
![IMAGE](tutorial_yarp-basics/images/yarp-server.png)
@snapend

@snap[south span-50]
![IMAGE](tutorial_yarp-basics/images/write_read.png)
@snapend


#HSLIDE

#### Yarp from code

#HSLIDE

#### Data types
@fa[arrow-down]

#VSLIDE

@snap[north]
<span style="color:#e49436">`yarp::os::Value`</span>
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- container to store a single instance of different basic data types:
@ulend
@snapend

```c++
class yarp::os::Value : public Portable
{
   Value(int x);                // Create an integer data.
   Value(double x);             // Create a floating point data.
   Value(string &str);          // Create a string data.
   Value(void *data, int len);  // Create a binary data.

   bool isInt();
   bool isDouble();
   bool isString();
   bool isBlob();

   int asInt();            // Get integer value.
   double asDouble();      // Get floating point value.
   string asString();      // Get string value.
   char* asBlob();         // Get binary data value.
};
```

#VSLIDE

@snap[north]
<span style="color:#e49436">`yarp::os::Property`</span>
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- dictionary type of data:
@ulend
@snapend

```c++
    Property prop;
    prop.clear();

    prop.put(“myInt”, Value(5));
    prop.put(“myString”, Value(“Hello World”));
    prop.put(“myPi”, 3.14);

    Property &myGroup = prop.addGroup(“group1”);
    group1.put(“g1”, Value(2.5));
    group1.put(“g2”, Value(“We have cookies”));


    prop.check(“myInt”);
    Value myInt = prop.find(“myInt”);
    double myPi = prop.find(“myPi”).asDouble();
    Bottle &group = prop.findGroup(“myGroup”)
```

#VSLIDE

@snap[north]
<span style="color:#e49436">`yarp::os::Bottle`</span>
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- most flexible type of data:
@ulend
@snapend

```c++
    Bottle bot;
    void clear();

    bot.addInt(5);
    bot.addString("hello");

    Bottle& b1 = addList();
    b1.addDouble(10.2);

    Property &prop = bot.addDict();
    prop.put(“message”, “Help me”);

    Value &v0 = bot.get(0);
    Value &v1 = bot.get(1);
```

#VSLIDE

@snap[north]
<span style="color:#e49436">`yarp::sig::ImageOf<PixelType>`</span>
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- container for image type:
@ulend
@snapend

```c++
    ImageOf<PixelRgb> yarpImage;
    yarpImage.resize(300,200);
    PixelRgb  rgb;
    rgb = yarpImage.pixel(10, 20);
```

#HSLIDE

#### Ports
@fa[arrow-down]

#VSLIDE

@snap[north]
<span style="color:#e49436"> `yarp::os::Port` </span>
@snapend

@snap[west]
@ul[list-spaced-bullets text-06](false)
- Synchronized communication sender/receivers.
- Ports identified by their name:

   - Names must be unique
   - Names must start with ‘/’ character
   - No ‘@’ character allowed
@ulend
@snapend

@snap[east span-45]
```c++
    yarp::os::Port myPort;
    myPort.open(“/port”);

    Bottle b;
    port.read(b);
    int n = b.get(0).asInt();
    n++;
    b.clear();
    b.addInt(n);
    myPort.write(b);

    myPort.close();
```
@snapend

#VSLIDE

@snap[north]
<span style="color:#e49436"> `yarp::os::BufferedPort` </span>
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- decouples time between sender and receivers:
@ulend
@snapend

@snap[text-left text-07 span-100]
```c++
    BufferedPort<Bottle> p;       // Create a port.
    p.open("/in");                // Give it a name on the network.
    while (true)
    {
      Bottle *b = p.read();       // Read/wait for until data arrives
      // Do something with data in *b
    }

    BufferedPort<Bottle> p;       // Create a port.
    p.open("/out");               // Give it a name on the network.
    while (true)
    {
      Bottle& b = p.prepare();    // Get a place to store things
      // write inside b
      p.write();
    }
```
@snapend

#VSLIDE

@snap[north]
#### Buffering policy
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- by default <span style="color:#e49436">`BufferedPort`</span> drops old messages.
@ulend
@snapend

@snap[text-left text-07 span-100]
```c++
    BufferedPort<Bottle> p;
    p.open("/in");
    p.setStrict(true);    // received messages are queued and never dropped
    while (true)
    {
      Bottle *b = p.read();
    }

    BufferedPort<Bottle> p;
    p.open("/out");
    while (true)
    {
      Bottle& b = p.prepare();
      // Generate data.
      p.write(true); //wait for previous pending write to complete
    }
```
@snapend

#VSLIDE

@snap[north]
#### Getting a callback
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- call a method when data arrives:
@ulend
@snapend

```c++
   class DataPort : public BufferedPort<Bottle>
   {
       using BufferedPort<Bottle>::onRead;
       void onRead(Bottle& b) override
       {
           // process data in b
       }
   };
   ...
   DataPort p;
   p.useCallback();  // input should go to onRead() callback
   p.open("/in");

```

#VSLIDE

@snap[north]
<span style="color:#e49436">`yarp::os::ResourceFinder`</span>
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
 - to find config files and external resources:
@ulend
@snapend

@snap[text-left text-07 span-100]
``` c++
    class MyModule: public RFModule
    {
      public:
        bool configure(ResourceFinder &rf)  // module configuration
        { return true; }

        double getPeriod()               // define period in seconds
        { return 1.0; }

        bool updateModule() // code that will be executed every “getPeriod” seconds
        { return true; }

        bool interruptModule() // catch CTRL+C
        {  return true; }

        bool close() // at shutdown cleanup, close ports, delete memory
        { return true; }
    };

    MyModule module;
    ResourceFinder rf;
    rf.configure(argc, argv);
    module.runModule(rf);
```
@snapend

@[20-23]
@[4-5]
@[10-11]
@[13-14]
@[16-17]


#VSLIDE

@snap[north]
<span style="color:#e49436"> `yarp::os::RpcServer`</span>
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
 - waits for reply:
@ulend
@snapend

@snap[text-left text-07 span-100]
```c++
      class MyModule::RFModule
      {
         RpcServer rpc;
         ...
         bool configure(ResourceFinder &rf)
         {
            rpcPort.open(“/myModule/rpc”);
            attach(rpcPort);
            ...
         }
         ...

         bool respond(const Bottle& command, Bottle& reply)
         {
            cout<<"Got something, echo is on"<<endl;
            if (command.get(0).asString()=="quit")
               return false;
            else
               reply=command;
            return true;
         }
      }

```
@snapend

#VSLIDE

@snap[north]
<span style="color:#e49436"> `Thrift IDL` </span>
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- language to define RPC based-interface
- create `.thrift` file describing the __Thrift service__ to our module:
@ulend
@snapend

```c++
   #demo.thrift
   service Demo_IDL
   {
     i32 get_answer();
   }
```
@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- generate the source code adding in `CMakeLists.txt`:
@ulend
@snapend

```
   yarp_add_idl(IDL_GEN_FILES demo.thrift)

```

#VSLIDE

@snap[north]
<span style="color:#e49436"> `Thrift IDL` </span>
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- implement functions defined in the service interface:
@ulend
@snapend

@snap[text-left text-07 span-100]
```c++
   #include "Demo_IDL.h"

   class Module: public RFModule, public Demo_IDL
   {
      RpcServer rpc;
      ...

      bool attach(RpcServer &source)
      {
         return this->yarp().attachAsServer(source);
      }

      bool configure(ResourceFinder &rf)
      {
         rpcPort.open(“/myModule/rpc”);
         attach(rpcPort);
         ...
      }

      int32_t get_answer()
      {
        cout << "The answer is "<< answer << endl;
        return answer;
      }
```
@snapend

#HSLIDE

#### Interfaces
@fa[arrow-down]

#VSLIDE

@snap[north]
<span style="color:#e49436">`IPositionControl`</span>
@snapend


@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- class with pure virtual methods
- servers provide functionalities implementing required methods
- clients use functionalities by calling provided methods
- gives access to main position control commands
@ulend
@snapend

```c++
    IPositionControl::getAxes() = 0;
    IPositionControl::positionMove(...) = 0;
    IPositionControl::relativeMove(...) = 0;
    IPositionControl::checkMotionDone(...) = 0;
    IPositionControl::setRefSpeed(...) = 0;
    IPositionControl::setRefAcceleration(...) = 0;
    IPositionControl::getRefSpeed(...) = 0;
    IPositionControl::getRefAcceleration(...) = 0;
    IPositionControl::getTargetPosition(...) = 0;
    IPositionControl::stop(...) = 0;
```

#VSLIDE

@snap[north]
#### Getting Interfaces
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- devices opened through the special class <span style="color:#e49436">`PolyDriver`</span>
- keyword `device` tells YARP which device we want to open
- <span style="color:#e49436">`remote_controlboard`</span>: provides access to robot motor control
- required parameters:

  - remote port prefix: `remote`
  - local port name: `local`
@ulend
@snapend

```c++
      PolyDriver poly;
      Property config;
      config.put(“device”,“remote_controlboard”);
      config.put(“remote”, “/icub/head”);
      config.put(“local”, “/<myApplication>”);
      ...
      poly.open(config);
```

#VSLIDE

@snap[text-left text-07 span-100]
```c++
      IPositionControl *posControl = NULL;

      poly.view(posControl);                 // Get the interface

      int joints;
      posControl->getAxes(&joints);          // Get number of joints
      posControl->setRefSpeed(0, 5);         // set a speed of 5 degrees/s for joint 0
      posControl->positionMove(0, 30);       // move the joint 0 to +30 degrees

      bool done = false;
      do
      {
        checkMotionDone(&done);              // this function checks the movement completion
      }
      while(!done);

      posControl->positionMove(0,0);         // move joint back to position 0
```
@snapend

#HSLIDE
Now let's code :)
