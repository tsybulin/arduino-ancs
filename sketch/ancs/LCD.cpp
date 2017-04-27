#include "LCD.h"
#include "characters.h"

#define LCD_COLS 16
#define LCD_LINES 2

#define LCD_BACKLIGHT_ON 0x1
#define LCD_BACKLIGHT_OFF 0x0

const unsigned int BACKLIGHT_TIMEOUT = 5000 ;
const unsigned int FLASH_TIMEOUT_ON  = 1000 ;
const unsigned int FLASH_TIMEOUT_OFF = 2000 ;

LCD::LCD() : lcd() {
    this->backlightTimer = 0 ;
    this->backlight = false ;
    this->flash = false ;
    this->buttons = 0x0 ;
    this->ancsStatus = AncsStatusUnknown ;
    memset(this->notifications, 0x00, sizeof(this->notifications)) ;
}

void LCD::setup() {
    this->lcd.begin(LCD_COLS, LCD_LINES) ;
    this->lcd.clear() ;
    this->lcd.setBacklight(LCD_BACKLIGHT_ON) ;
    uint8_t icon[8] = ICO_DISCONNECTED ;
    this->lcd.createChar(0, icon) ;
    this->lcd.write((char) 0x0) ;
    this->lcd.print(F(" HELLO, ANCS")) ;
    this->lcd.setCursor(0, 1) ;
    this->lcd.write((char) LCD_CHAR_ARROW_RIGHT) ;
    this->lcd.print(F(" PRESS SELECT ")) ;
    this->lcd.blink() ;

    do {
        this->buttons = this->lcd.readButtons() ;
    } while (!this->buttons & BUTTON_SELECT) ;
    this->lcd.noBlink() ;
    
    this->fire() ;
}

void LCD::poll() {    
    this->buttons = this->lcd.readButtons() ;
    if (this->buttons) {
        if (this->buttons & BUTTON_SELECT) {
            this->flash = false ;

            if (this->backlight) {
                this->backlight = false ;
                this->lcd.setBacklight(LCD_BACKLIGHT_OFF) ;
            }
        } else if (this->buttons & BUTTON_LEFT) {
            this->backlight = true ;
            this->lcd.setBacklight(LCD_BACKLIGHT_ON) ;

            for (uint8_t i = 0; i < CATEGORY_COUNT; i++) {
                this->notifications[i] = 0 ;
            }

            this->fire() ;
        } else if (!this->backlight) {
            this->backlight = true ;
            this->lcd.setBacklight(LCD_BACKLIGHT_ON) ;
        }

        this->backlightTimer = millis() ;
    }
    
    if (this->flash) {
        if (this->backlight && ((millis() - this->backlightTimer) > FLASH_TIMEOUT_ON)) {
            this->backlight = false ;
            this->lcd.setBacklight(LCD_BACKLIGHT_OFF) ;
            this->backlightTimer = millis() ;
        } else if (!this->backlight && ((millis() - this->backlightTimer) > FLASH_TIMEOUT_OFF)) {
            this->backlight = true ;
            this->lcd.setBacklight(LCD_BACKLIGHT_ON) ;
            this->backlightTimer = millis() ;
        }
    } else if (this->backlight && ((millis() - this->backlightTimer) > BACKLIGHT_TIMEOUT)) {
        this->backlight = false ;
        this->lcd.setBacklight(LCD_BACKLIGHT_OFF) ;
    }
}

enum AncsNotificationCategoryId {
  AncsNotificationCategoryIdOther              = 0,
  AncsNotificationCategoryIdIncomingCall       = 1,
  AncsNotificationCategoryIdMissedCall         = 2,
  AncsNotificationCategoryIdVoicemail          = 3,
  AncsNotificationCategoryIdSocial             = 4,
  AncsNotificationCategoryIdSchedule           = 5,
  AncsNotificationCategoryIdEmail              = 6,
  AncsNotificationCategoryIdNews               = 7,
  AncsNotificationCategoryIdHealthAndFitness   = 8,
  AncsNotificationCategoryIdBusinessAndFinance = 9,
  AncsNotificationCategoryIdLocation           = 10,
  AncsNotificationCategoryIdEntertainment      = 11
} ;

void LCD::fire() {
    uint8_t total = this->totalCount() ;

    this->lcd.clear() ;
    this->lcd.setCursor(0, 0) ;
    this->lcd.write((char) 0x0) ;
    
    if (total == 0) {
        this->flash = false ;
        this->lcd.print(F(" THERE ARE NO")) ;
        this->lcd.setCursor(2, 1) ;
        this->lcd.print(F("NOTIFICATIONS")) ;
    } else if (this->notifications[AncsNotificationCategoryIdMissedCall] > 0) {
        this->flash = true ;
        this->lcd.print(F("  YOU MISSED ")) ;
        this->lcd.print(this->notifications[AncsNotificationCategoryIdMissedCall]) ;
        this->lcd.setCursor(5, 1) ;
        this->lcd.print(F("CALL(S)")) ;
    } else if (this->notifications[AncsNotificationCategoryIdIncomingCall] > 0) {
        this->flash = true ;
        this->lcd.print(F(" INCOMING CALL")) ;
    } else if (this->notifications[AncsNotificationCategoryIdEmail] > 0) {
        this->flash = true ;
        this->lcd.print(F("   YOU GOT ")) ;
        this->lcd.print(this->notifications[AncsNotificationCategoryIdEmail]) ;
        this->lcd.setCursor(5, 1) ;
        this->lcd.print(F("MAILS")) ;
    } else if (this->notifications[AncsNotificationCategoryIdSocial] > 0) {
        this->flash = true ;
        this->lcd.setCursor(4, 0) ;
        this->lcd.print(this->notifications[AncsNotificationCategoryIdSocial]) ;
        this->lcd.print(F(" NEW CHAT")) ;
        this->lcd.setCursor(5, 1) ;
        this->lcd.print(F("MESSAGES")) ;
    } else {
        this->flash = true ;
        this->backlight = true ;
        this->lcd.setBacklight(LCD_BACKLIGHT_ON) ;

        this->lcd.print(F(" THERE ARE ")) ;
        this->lcd.print(total) ;
        this->lcd.setCursor(2, 1) ;
        this->lcd.print(F("NOTIFICATIONS")) ;
    }
    
    if (!this->backlight) {
        this->backlight = true ;
        this->lcd.setBacklight(LCD_BACKLIGHT_ON) ;
    }
    this->backlightTimer = millis() ;
}

void LCD::notificatonDidChanged(uint8_t categoryId, byte count) {
    this->notifications[categoryId] = count ;
    this->fire() ;
}

byte LCD::totalCount() {
    byte count = 0 ;
    
    for (uint8_t i = 0; i < CATEGORY_COUNT; i++) {
        count += this->notifications[i] ;
    }

    return count ;
}

void LCD::didConnected() {
    if (this->ancsStatus != AncsStatusConnected) {
        this->ancsStatus = AncsStatusConnected ;
        uint8_t icon[8] = ICO_CONNECTED ;
        this->lcd.createChar(0, icon) ;
        this->fire() ;
    }
}

void LCD::didDisconnected() {
    if (this->ancsStatus != AncsStatusDisconnected) {
        this->ancsStatus = AncsStatusDisconnected ;
        uint8_t icon[8] = ICO_DISCONNECTED ;
        this->lcd.createChar(0, icon) ;
        this->fire() ;
    }
}

void LCD::didBonded() {
    if (this->ancsStatus != AncsStatusBonded) {
        this->ancsStatus = AncsStatusBonded ;
        uint8_t icon[8] = ICO_BONDED ;
        this->lcd.createChar(0, icon) ;
        this->fire() ;
    }
}

void LCD::didDiscovered() {
    this->ancsStatus = AncsStatusDiscovered ;
}

