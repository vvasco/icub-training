#HSLIDE
### Kinematics
@fa[arrow-down]
#VSLIDE

@snap[north]
### Joint VS Cartesian space
@snapend

![IMAGE](tutorial_interface/images/kinematics.jpg)

#HSLIDE

### The Cartesian Interface
@fa[arrow-down]

#VSLIDE

@snap[north]
### The Cartesian Controller
@snapend

@ul[list-spaced-bullets text-08](false)
- Control the robot's arms directly in the  <span style="color:#e49436">Cartesian</span> space.
@ulend

![IMAGE](tutorial_interface/images/cartesian-controller.png)

#VSLIDE

@snap[north]
### The Solver
<span style="color:#e49436">`iCub::iKin::CartesianSolver`</span>
@snapend

@snap[center span-100]
![IMAGE](tutorial_interface/images/solver.png)
@snapend

@ul[list-spaced-bullets text-08](false)
- Inverse kinematics based on <span style="color:#e49436">[IpOpt](https://coin-or.github.io/Ipopt/)</span>
- Cost function: reaching in orientation
- Constraint: reaching for position @fa[arrow-right] higher priority!
@ulend

#VSLIDE

@snap[north-west]
### The Controller
@snapend

@snap[west]
@ul[list-spaced-bullets text-08](false)
- Server part of the interface
- Human-like @fa[arrow-right] minimum-jerk model
@ulend
@snapend

@snap[north-east span-35]
![IMAGE](tutorial_interface/images/min-jerk-trajectory.png)
@snapend

@snap[south-east span-35]
![IMAGE](tutorial_interface/images/min-jerk-velocity.png)
@snapend

#VSLIDE

@snap[north]
### ICartesianControl
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-08](false)
- Exposes C++ methods
@ulend
@snapend

#HSLIDE

### The Gaze Interface
@fa[arrow-down]

@snap[south-east]
@snapend

#VSLIDE

@snap[north]
### The Gaze Controller
@snapend

Yet another Cartesian Controller

![IMAGE](tutorial_interface/images/gaze-controller.png)

- Steer neck and eyes independently

#VSLIDE

@snap[north]
### The Gaze Controller
@snapend

@snap[west span-35]
![IMAGE](tutorial_interface/images/icub-head.png)
@snapend

@snap[east span-65]
![IMAGE](tutorial_interface/images/table-head.png)
@snapend

#HSLIDE
@snap[north]
### iCub Frame Root
@snapend

@snap[west span-45]
![IMAGE](tutorial_interface/images/robot-reference-frame.jpg)
@snapend

@snap[east span-45]
@ul[list-spaced-bullets text-center text-04](false)
- red: x-axis
- green: y-axis
- blue: z-axis
@ulend
![IMAGE](tutorial_interface/images/LeftHandCADRefFrame.jpg)
@snapend


#HSLIDE
Now let's code :)
