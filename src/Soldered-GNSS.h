/**
 **************************************************
 *
 * @file        Soldered-GNSS.h
 * @brief       Header file for the Soldered u-blox GPS/GNSS Arduino Library,
 *              providing a thin wrapper over the SparkFun u-blox GNSS driver
 *              for use with Soldered u-blox GPS/GNSS breakout boards
 *
 * @copyright   GNU General Public License v3.0
 * @authors     Josip Šimun Kuči @ soldered.com
 ***************************************************/

#ifndef _SOLDERED_GNSS_H_
#define _SOLDERED_GNSS_H_

#include "Arduino.h"
#include "libs/u-blox/SparkFun_u-blox_GNSS_Arduino_Library.h"

class Soldered_GNSS : public SFE_UBLOX_GNSS
{
  public:
    Soldered_GNSS() : SFE_UBLOX_GNSS()
    {
    }
};

#endif
