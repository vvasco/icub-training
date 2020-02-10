# icub-training

[![Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/vvasco/icub-training)

[![GitPitch](https://gitpitch.com/assets/badge.svg)](https://gitpitch.com/vvasco/icub-training/master)

## Get started

Welcome to the icub training!

The first thing you need to do is subscribe to [Gitpod](https://gitpod.io) using your github credentials.  
You can find [here](https://spectrum.chat/icub/technicalities/yarp-enabled-gitpod-workspaces-available~73ab5ee9-830e-4b7f-9e99-195295bb5e34) a description of how to deal with a YARP-enabled Gitpod workspaces available.

Ready? Great! Now you can simply click on the Gitpod badge appearing on the top of this page and the environment will be magically set for you!

## Overview
We will go through the following tutorials: 

- [basics of yarp](https://github.com/vvasco/icub-training/tree/master/tutorial_yarp-basics);
- [robot vision and openCV](https://github.com/vvasco/icub-training/tree/master/tutorial_yarp-opencv);
- [how to use the gaze and cartesian interface](https://github.com/vvasco/icub-training/tree/master/tutorial_interface).

Each tutorial has some intro slides.. all you need to do is to click on the gitpitch badge.

## Build and Install the code
Just a last step before starting coding:

```
$ cd /workspace/icub-training
$ mkdir build && cd build
$ cmake ../
$ make
$ make install
```
the `make install` will install your module (binary, xml files, etc) in the icub contrib folder which is already setup.


