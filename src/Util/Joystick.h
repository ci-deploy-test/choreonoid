/*!
  @file
  @author Shin'ichiro Nakaoka
*/

#ifndef CNOID_UTIL_JOYSTICK_H
#define CNOID_UTIL_JOYSTICK_H

#include "Signal.h"
#include "exportdecl.h"

namespace cnoid {

class JoystickImpl;

class CNOID_EXPORT Joystick
{
public:
    Joystick();
    Joystick(const char* device);
    virtual ~Joystick();

    int fileDescriptor() const;

    bool isReady() const;
    const char* errorMessage() const;

    enum AxisID {
        L_STICK_H_AXIS,
        L_STICK_V_AXIS,
        R_STICK_H_AXIS,
        R_STICK_V_AXIS,
        DIRECTIONAL_PAD_H_AXIS,
        DIRECTIONAL_PAD_V_AXIS,
        L_TRIGGER_AXIS,
        R_TRIGGER_AXIS,
        NUM_STD_AXES
    };

    enum ButtonID {
        A_BUTTON,
        B_BUTTON,
        X_BUTTON,
        Y_BUTTON,
        L_BUTTON,
        R_BUTTON,
        SELECT_BUTTON,
        START_BUTTON,
        L_STICK_BUTTON,
        R_STICK_BUTTON,
        LOGO_BUTTON,
        NUM_STD_BUTTONS
    };

    int numAxes() const;
    void setAxisEnabled(int axis, bool on);
    int numButtons() const;
    double getDefaultPosition(int axis) const;

    bool readCurrentState();
    double getPosition(int axis) const;
    double getPosition(int axis, double threshold) const;
    bool getButtonState(int button) const;

#ifdef __linux__
    bool getButtonDown(int button) const;
    bool getButtonUp(int button) const;
    bool getButtonHold(int button, int duration/*(msec)*/) const;
    bool getButtonHoldOn(int button, int duration/*(msec)*/) const;
#endif
    
    bool isActive() const;
    SignalProxy<void(int id, bool isPressed)> sigButton();
    SignalProxy<void(int id, double position)> sigAxis();

private:
    JoystickImpl* impl;
    friend class JoystickImpl;
};

}

#endif
