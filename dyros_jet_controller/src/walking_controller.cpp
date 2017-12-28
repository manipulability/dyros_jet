#include "dyros_jet_controller/dyros_jet_model.h"
#include "dyros_jet_controller/walking_controller.h"

namespace dyros_jet_controller
{


void WalkingController::initWalkingPose(VectorQd& desired_q)
{
  int index = 0;
  target_q(index++) = 0;
  target_q(index++) = 0;

// R_arm
  target_q(index++) = -40*DEGREE;
  target_q(index++) = 95*DEGREE;
  target_q(index++) = 80*DEGREE;
  target_q(index++) = 110*DEGREE;
  target_q(index++) =  0*DEGREE;
  target_q(index++) = 70*DEGREE;
  target_q(index++) = 10*DEGREE;

// L_arm
  target_q(index++) =  40*DEGREE;
  target_q(index++) = -95*DEGREE;
  target_q(index++) = -80*DEGREE;
  target_q(index++) = -110*DEGREE;
  target_q(index++) =  0*DEGREE;
  target_q(index++) = -70*DEGREE;
  target_q(index++) = -10*DEGREE;
//R_leg
  target_q(index++) = 0*DEGREE;
  target_q(index++) = -2*DEGREE;
  target_q(index++) = 20*DEGREE;
  target_q(index++) = -40*DEGREE;
  target_q(index++) = 20*DEGREE;
  target_q(index++) = 2*DEGREE;
//L_leg
  target_q(index++) = 0*DEGREE;
  target_q(index++) = 2*DEGREE;
  target_q(index++) = -20*DEGREE;
  target_q(index++) = 40*DEGREE;
  target_q(index++) = -20*DEGREE;
  target_q(index++) = -2*DEGREE;
  target_q(ra_hand) = 40*DEGREE; // Joint number??



  for(unsigned int i=0; i<total_dof_; i++)
  {
    if(joint_enable_[i])
    {
      desired_q_(i) = DyrosMath::cubic(current_time_, 0.0, 5.0*Hz, start_q_(i), target_q_(i), 0, 0);
    }
  }
}

void WalkingController::compute(VectorQd& desired_q)
{

}


void WalkingController::setTarget(unsigned int joint_number, double target, double start_time, double end_time)
{
  if(joint_number >= total_dof_)
  {
    ROS_ERROR("WalkingController::setTarget - Out of range. Input = %u", joint_number);
    return ;
  }
  start_time_[joint_number] = start_time;
  end_time_[joint_number] = end_time;
  start_q_(joint_number) = current_q_(joint_number);
  target_q_(joint_number) = target;
}

void WalkingController::setTarget(unsigned int joint_number, double target, double duration)
{
  setTarget(joint_number, target, current_time_, current_time_ + duration);
}

void WalkingController::setEnable(unsigned int joint_number, bool enable)
{
  if (joint_number < total_dof_)
  {
    joint_enable_[joint_number] = enable;
  }
  else
  {
    ROS_ERROR("WalkingController::setEnable - Out of range. Input = %u", joint_number);
  }
}

void WalkingController::updateControlMask(unsigned int *mask)
{
  for (int i=0; i<total_dof_; i++)
  {
    if (joint_enable_[i])
    {
      if (mask[i] >= PRIORITY * 2)
      {
         setTarget(i,desired_q_(i),0);// Stop moving
      }
      mask[i] = (mask[i] | PRIORITY);
    }
    else
    {
      mask[i] = (mask[i] & ~PRIORITY);
    }
  }
}
void WalkingController::writeDesired(const unsigned int *mask, VectorQd& desired_q)
{
  for(unsigned int i=0; i<total_dof_; i++)
  {
    if( mask[i] >= PRIORITY && mask[i] < PRIORITY * 2 )
    {
      desired_q(i) = desired_q_(i);
    }
  }
}
}
