/** @file pid.h
 * @brief
 * @author Tiger3018
 * @date 2022-01-17
 * @li MIT License
 */
#pragma once
#ifndef __PID_H__
#define __PID_H__

#ifdef __cplusplus
extern "C" {
#endif

// include

// definition

// struct
/** kp, ki, kd, frequency, ki_limit, k_limit
 * @see class PartControl
 */
typedef struct PidCoeff
{
  double kp, ki, kd;        /** coefficient of pid */
  double frequency;         /** reserved for future use */
  double ki_limit, k_limit; /** 0 means no limit, used with @see CONSTRAIN_ABS */
} PidCoeff;
/** feed, cmd, error[3], tick[3], p_out, i_out, d_out, out
 * @see class PartControl
 */
typedef struct PidInfo
{
  double feed, cmd, error[3], tick[3], p_out, i_out, d_out, out;
} PidInfo;

#ifdef __cplusplus
}
#endif

#endif /* __PID_H__ */