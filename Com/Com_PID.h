#ifndef __COM_PID_H__
#define __COM_PID_H__

#include "stm32f10x.h"

int Com_PID_Balance(float Kp, float Kd, float expectedAngle, float actualAngle,
                         float angularVelocity);

int Com_PID_Velocity(float Kp, float Ki, float expectedVelocity, float measureVelocity,
                     int cmdDisplacement);

int Com_PID_Turn(float Kp, float angularVelocity);

#endif /* __COM_PID_H__ */
