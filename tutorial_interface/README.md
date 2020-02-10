[![GitPitch](https://gitpitch.com/assets/badge.svg)](https://gitpitch.com/vvasco/icub-training/master?p=tutorial_interface)

# Tutorial on Gaze and Cartesian interface

This tutorial shows how to use the Gaze and Cartesian interface.

## Goal of the Gaze tutorial

There is a robot standing in a world far away, whose body is fully anchored to the ground. Fortunately, the robot can still gaze around. There is also a magic red ball sticking out from the beautiful landscape. The ball floats insanely in front of the robot, moving along unpredictable trajectories and even teleporting from one place to another.

Given that the robot observes the world through its 320x240 couple of cameras and now we know how to track red balls, your task is to let the robot look constantly at the floating ball.

We are going to visualize the ball's pixel coordinates and the robot's eyes tilt and pan. 
You will see that:
- the x pixel coordinate of the ball in the left camera image shall approach the center: ul ≈ 160.
- the y pixel coordinate of the ball in the left camera image shall approach the center: vl ≈ 120.
- the x pixel coordinate of the ball in the right camera image shall approach the center: ur ≈ 160.
- the y pixel coordinate of the ball in the right camera image shall approach the center: vr ≈ 120.
- the robot shall keep the eyes tilt close to 0 degrees: eyes-tilt ≈ 0 [deg].
- the robot shall keep the eyes pan close to 0 degrees: eyes-pan ≈ 0 [deg].

## Goal of the Cartesian tutorial

The same robot can also move its arms to track the ball. 
Given the ball position known through the previous tutorial, your task is to let the robot constantly track the ball with the left hand facing down.

We are going to visualize the robot arm's desired and current position and desired and current orientation. 
You will see that the current position and orientation should stay around the desired ones.
