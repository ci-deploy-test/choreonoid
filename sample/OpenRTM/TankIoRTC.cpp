/**
   @author Shin'ichiro Nakaoka
*/

#include <cnoid/BodyIoRTC>
#include <cnoid/Light>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>

using namespace std;
using namespace cnoid;

namespace {

class TankIoRTC : public BodyIoRTC
{
public:
    TankIoRTC(RTC::Manager* manager);
    ~TankIoRTC();

    virtual RTC::ReturnCode_t onInitialize(Body* body) override;
    virtual bool initializeSimulation(ControllerItemIO* io) override;
    virtual void inputFromSimulator() override;
    virtual void outputToSimulator() override;

    BodyPtr ioBody;
    Link* cannonY;
    Link* cannonP;
    Link* crawlerL;
    Link* crawlerR;
    
    // DataInPort declaration
    RTC::TimedDoubleSeq torques;
    RTC::InPort<RTC::TimedDoubleSeq> torquesIn;

    RTC::TimedDoubleSeq velocities;
    RTC::InPort<RTC::TimedDoubleSeq> velocitiesIn;

    RTC::TimedBooleanSeq lightSwitch;
    RTC::InPort<RTC::TimedBooleanSeq> lightSwitchIn;
    LightPtr light;
    
    // DataOutPort declaration
    RTC::TimedDoubleSeq angles;
    RTC::OutPort<RTC::TimedDoubleSeq> anglesOut;
};

const char* spec[] =
{
    "implementation_id", "TankIoRTC",
    "type_name",         "TankIoRTC",
    "description",       "Tank I/O",
    "version",           "1.0",
    "vendor",            "AIST",
    "category",          "Generic",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

}

TankIoRTC::TankIoRTC(RTC::Manager* manager)
    : BodyIoRTC(manager),
      torquesIn("u", torques),
      velocitiesIn("dq", velocities),
      lightSwitchIn("light", lightSwitch),
      anglesOut("q", angles)
{

}


TankIoRTC::~TankIoRTC()
{

}


RTC::ReturnCode_t TankIoRTC::onInitialize(Body* body)
{
    // Set InPort buffers
    addInPort("u", torquesIn);
    addInPort("dq", velocitiesIn);
    addInPort("light", lightSwitchIn);
    
    // Set OutPort buffer
    addOutPort("q", anglesOut);
    angles.data.length(2);

    return RTC::RTC_OK;
}


bool TankIoRTC::initializeSimulation(ControllerItemIO* io)
{
    ioBody = io->body();
    cannonY = ioBody->link("CANNON_Y");
    cannonP = ioBody->link("CANNON_P");
    crawlerL = ioBody->link("CRAWLER_TRACK_L");
    crawlerR = ioBody->link("CRAWLER_TRACK_R");
    light = ioBody->findDevice<Light>("MainLight");
    return true;
}


void TankIoRTC::inputFromSimulator()
{
    angles.data[0] = cannonY->q();
    angles.data[1] = cannonP->q();
    anglesOut.write();
}


void TankIoRTC::outputToSimulator()
{
    if(torquesIn.isNew()){
        torquesIn.read();
        if(torques.data.length() >= 2){
            cannonY->u() = torques.data[0];
            cannonP->u() = torques.data[1];
        }
    }
    if(velocitiesIn.isNew()){
        velocitiesIn.read();
        if(velocities.data.length() >= 2){
            crawlerL->dq() = velocities.data[0];
            crawlerR->dq() = velocities.data[1];
        }
    }
    if(light && lightSwitchIn.isNew()){
        lightSwitchIn.read();
        light->on(lightSwitch.data[0]);
        light->notifyStateChange();
    }
}


extern "C"
{
    DLL_EXPORT void TankIoRTCInit(RTC::Manager* manager)
    {
        coil::Properties profile(spec);
        manager->registerFactory(
            profile, RTC::Create<TankIoRTC>, RTC::Delete<TankIoRTC>);
    }
};
