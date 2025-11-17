#ifndef __PID_H
#define __PID_H

typedef struct {
    float kp;
    float ki;
    float kd;
    float integral;
    float last_error;
    float output;
} PIDController;

extern PIDController*pid;

void PID_Init(PIDController* pid, float kp, float ki, float kd);
float PID_Calculate(PIDController* pid, float target, float actual);

#endif
