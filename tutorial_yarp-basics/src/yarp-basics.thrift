# Copyright: (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Valentina Vasco
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
#
# yarp-basics.thrift

/**
* yarp-basics_IDL
*
* IDL Interface to yarp-basics Module.
*/

service yarpBasics_IDL
{
    /**
     * Get the position of the specified head joint.
     * @param jnt: joint number (joint 0 if not specified).
     * @return position (degree) of the specified head joint.
     */
    double get_enc(1:i32 jnt = 0);


}
