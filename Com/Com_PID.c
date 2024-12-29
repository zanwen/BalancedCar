#include "Com_PID.h"

/**
 * @brief PD控制-直立环（主控制环）：消除倾角误差（期望为立正，倾角为0）
 * 对倾角误差进行P控制，倾角越大，速度越大，使其立正（控制现在）
 * D控制：根据误差变化率来提前调整输出（与变化率对抗），避免惯性振荡和P控制过冲（预测未来）
 *
 * @param Kp 比例系数
 * @param Kd 微分系数
 * @param expectedAngle 期望的角度
 * @param measureAngle 测量的角度
 * @param angularVelocity 角速度
 * @return int PWM占空比[0, 7200-1]
 */
int Com_PID_Balance(float Kp, float Kd, float expectedAngle, float measureAngle,
                    float angularVelocity) {
    static float error = 0.0f;
    error              = measureAngle - expectedAngle;
    return Kp * error + Kd * angularVelocity;
}

/**
 * @brief PI控制-速度环（辅助环）：消除速度误差（期望达到直立后速度为0）
 * 主控制环存在稳态误差的问题，例如保持一定的倾角做匀速运动，无法静止
 * P控制：在有速度误差（非静止）时，增加辅助速度，使小车尽快立正从而达到静止（立正后直立环输出趋近于0，速度也就趋近于0）
 * I控制：达到稳态误差后，通过累积误差来进一步增加速度（）
 *
 * @param Kp 比例系数P
 * @param Ki 积分系数I
 * @param expectedVelocity 期望的速度
 * @param measureVelocity 测量的速度
 * @param cmdDisplacement 蓝牙命令小车移动的位移
 * @return int PWM占空比[0, 7200-1]
 */
int Com_PID_Velocity(float Kp, float Ki, float expectedVelocity, float measureVelocity,
                     int cmdDisplacement) {
    static float integralError = 0.0f;
    static float lastError     = 0.0f;
    static float error         = 0.0f;
    error                      = measureVelocity - expectedVelocity;
    // 一阶低通滤波器，过滤高频噪声，且以直立环为主导进行辅助
    error     = lastError * 0.8 + error * 0.2;
    lastError = error;
    // 积分累加和限幅
    integralError += error;
    // 速度的积分是位移，将蓝牙命令的位移x从该积分中减去，则小车重新运动x才能抵消该积分误差
    integralError -= cmdDisplacement;
    if (integralError > 10000) {
        integralError = 10000;
    } else if (integralError < -10000) {
        integralError = -10000;
    }
    return Kp * error + Ki * integralError;
}

/**
 * @brief P控制-转向环
 * 是小车保持执行，在Z轴出现角速度偏差时进行调整
 *
 * @param Kp 比例系数P
 * @param angularVelocity 测量的偏航角变化率
 * @return int PWM占空比[0, 7200-1]
 */
int Com_PID_Turn(float Kp, float angularVelocity) { return Kp * angularVelocity; }
