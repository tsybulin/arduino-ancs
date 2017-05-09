#ifndef LCD_H
#define LCD_H

#undef USE_LED
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
        byte poll() ;
        void didConnected() ;
        void didDisconnected() ;
        void didBonded() ;
        void didDiscovered() ;
        void notificatonDidChanged(uint8_t categoryId, byte count) ;
    private:
        void fire() ;
        byte menu() ;
        byte totalCount() ;
        
        Adafruit_RGBLCDShield lcd  ;
        unsigned long backlightTimer ;
        byte options ;
        byte notifications[CATEGORY_COUNT] ;
        AncsStatus ancsStatus ;
} ;

#endif

