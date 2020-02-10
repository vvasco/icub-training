# Copyright: (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Valentina Vasco
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
#
# tutorial_interface-control-gaze.thrift

/**
* controlGaze_IDL
*
* IDL Interface to control-gaze Module.
*/

service controlGaze_IDL
{
    /**
     * Start module.
     * @return true (/false) on success (/failure).
     */
    bool start_gaze();

    /**
     * Stop module.
     * @return true (/false) on success (/failure).
     */
    bool stop_gaze();


}
