#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <stdint.h>
#include <Adafruit_RGBLCDShield.h>

const uint8_t CATEGORY_COUNT = 12 ;

enum AncsStatus {
    AncsStatusUnknown,
    AncsStatusDisconnected,
    AncsStatusConnected,
    AncsStatusBonded,
    AncsStatusDiscovered
} ;

class LCD {
    public:
        LCD() ;
        byte setup() ;
        void poll() ;
        void didConnected() ;
        void didDisconnected() ;
        void didBonded() ;
        void didDiscovered() ;
        void notificatonDidChanged(uint8_t categoryId, byte count) ;
    private:
        void fire() ;
        byte totalCount() ;
        
        Adafruit_RGBLCDShield lcd  ;
        unsigned long backlightTimer ;
        boolean backlight ;
        boolean flash ;
        uint8_t buttons ;
        byte notifications[CATEGORY_COUNT] ;
        AncsStatus ancsStatus ;
} ;

#endif

