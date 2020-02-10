# Copyright: (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Valentina Vasco
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
#
# tutorial_interface-control-cartesian.thrift

/**
* controlCartesian_IDL
*
* IDL Interface to control-cartesian Module.
*/

service controlCartesian_IDL
{
    /**
     * Start module.
     * @return true (/false) on success (/failure).
     */
    bool start_cartesian();

    /**
     * Stop module.
     * @return true (/false) on success (/failure).
     */
    bool stop_cartesian();

    /**
     * Enable torso in cartesian control.
     * @param pitch 1 (/0) if activated (/disactivated).
     * @param roll 1 (/0) if activated (/disactivated).
     * @param yaw 1 (/0) if activated (/disactivated).
     * @return true (/false) on success (/failure).
     */
    bool enable_torso(1:i32 pitch, 2:i32 roll, 3:i32 yaw);

}
