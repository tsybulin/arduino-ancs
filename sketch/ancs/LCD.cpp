#include "LCD.h"
#include "characters.h"

#define LCD_COLS 16
#define LCD_LINES 2

#define LCD_BACKLIGHT_ON 0x1
#define LCD_BACKLIGHT_OFF 0x0

#define BUZZER_PIN 6
#define LED_PIN 5

const unsigned int BACKLIGHT_TIMEOUT = 5000 ;
const unsigned int FLASH_TIMEOUT_ON  = 1000 ;
const unsigned int FLASH_TIMEOUT_OFF = 2000 ;
const unsigned int BOOT_TIMEOUT      = 5000 ;

#define OPTIONS_BACKLIGHT B000001
#define OPTIONS_FLASH     B000010

#define OPTIONS_SOUND     B010000
#define OPTIONS_LIGHT     B001000

#define backlightAvailable (this->options & OPTIONS_LIGHT)
#define baclkightOn (this->options & OPTIONS_BACKLIGHT)

LCD::LCD() : lcd() {
    this->backlightTimer = 0 ;
    this->options = OPTIONS_SOUND | OPTIONS_LIGHT ;
    this->ancsStatus = AncsStatusUnknown ;
    memset(this->notifications, 0x00, sizeof(this->notifications)) ;
}

byte LCD::setup() {
    byte result = 0x0 ;
    pinMode(BUZZER_PIN, OUTPUT) ;
    pinMode(LED_PIN, OUTPUT) ;
    
    this->lcd.begin(LCD_COLS, LCD_LINES) ;
    this->lcd.clear() ;
    this->lcd.setBacklight(LCD_BACKLIGHT_ON) ;
    
    uint8_t icon[8] = ICO_DISCONNECTED ;
    this->lcd.createChar(0, icon) ;

    uint8_t buzzer[8] = ICO_SOUND_ON ;
    this->lcd.createChar(1, buzzer) ;

    this->lcd.write((char) 0x0) ;
    this->lcd.print(F(" HELLO, ANCS")) ;
    this->lcd.setCursor(0, 1) ;
    this->lcd.write((char) LCD_CHAR_ARROW_LEFT) ;
    this->lcd.print(F(" CLEAR  BOOT ")) ;
    this->lcd.write((char) LCD_CHAR_ARROW_RIGHT) ;
    this->lcd.setCursor(15, 0) ;
    this->lcd.blink() ;

    uint8_t buttons  ;
    unsigned long now = millis() ;
    for(;;) {
        buttons = this->lcd.readButtons() ;

        // debounce
        if (buttons) {
            byte b = 0x1 ;
            do {
                delay(30) ;
                b = this->lcd.readButtons() ;
            } while (b) ;
        }

        if (buttons & BUTTON_LEFT) {
            result |= 0x1 ;
        }

        if  ((millis() - now > BOOT_TIMEOUT) || (buttons & (BUTTON_LEFT | BUTTON_RIGHT))) {
            break ;
        }
    } 

    this->lcd.noBlink() ;
    this->fire() ;
    return result ;
}

byte LCD::poll() {
    byte result = 0x0 ;

    uint8_t buttons = this->lcd.readButtons() ;

    if (buttons) {
        // debounce
        byte b = 0x1 ;
        do {
            delay(30) ;
            b = this->lcd.readButtons() ;
        } while (b) ;

        this->options &= ~OPTIONS_FLASH ;

        if (buttons & BUTTON_SELECT) {
            result = this->menu() ;
        } else if (backlightAvailable && !baclkightOn) {
            this->options ^= OPTIONS_BACKLIGHT ;
            this->lcd.setBacklight(LCD_BACKLIGHT_ON) ;
        }

        this->backlightTimer = millis() ;
    }
    
    if ((this->options & OPTIONS_FLASH)) {
        if (baclkightOn && ((millis() - this->backlightTimer) > FLASH_TIMEOUT_ON)) {
            this->options ^= OPTIONS_BACKLIGHT ;
            digitalWrite(BUZZER_PIN, LOW) ;
            digitalWrite(LED_PIN, LOW) ;
            if (backlightAvailable) {
                this->lcd.setBacklight(LCD_BACKLIGHT_OFF) ;
            }
            this->backlightTimer = millis() ;
        } else if (!baclkightOn && ((millis() - this->backlightTimer) > FLASH_TIMEOUT_OFF)) {
            this->options ^= OPTIONS_BACKLIGHT ;
            analogWrite(BUZZER_PIN, 179) ;
            digitalWrite(LED_PIN, HIGH) ;
            if (backlightAvailable) {
                this->lcd.setBacklight(LCD_BACKLIGHT_ON) ;
            }
            this->backlightTimer = millis() ;
        }
    } else if (baclkightOn && ((millis() - this->backlightTimer) > BACKLIGHT_TIMEOUT)) {
        this->options ^= OPTIONS_BACKLIGHT ;
        digitalWrite(LED_PIN, LOW) ;
        digitalWrite(BUZZER_PIN, LOW) ;
        this->lcd.setBacklight(LCD_BACKLIGHT_OFF) ;
    }

    return result ;
}

byte LCD::menu() {
    byte result = 0x0 ;
    uint8_t buttons ;
    this->lcd.clear() ;
    byte m = 0, om = 1 ;
    
    this->lcd.setCursor(0, 1) ;
    this->lcd.write((char) 0x1) ;
    this->lcd.write((char) 0x20) ;
    this->lcd.write((char) LCD_CHAR_ARROW_LEFT) ;
    this->lcd.print(F("  SELECT   ")) ;
    this->lcd.write((char) LCD_CHAR_ARROW_RIGHT) ;

    this->lcd.setCursor(0, 0) ;
    this->lcd.write((char) 0x0) ;

    this->lcd.setBacklight(LCD_BACKLIGHT_ON) ;

    for(;;) {
        if (m != om) {
            om = m ;
            this->lcd.setCursor(1, 0) ;
            if (m == 0) {
                this->lcd.print(F("     EXIT      ")) ;
            } else if (m == 1) {
                this->lcd.print(F(" CLEAR MESSAGES")) ;
            } else if (m == 2) {
                this->lcd.print(F("  SOUND :")) ;
                this->lcd.print(this->options & OPTIONS_SOUND ? F("ON    ") : F("OFF   ")) ;
            } else if (m == 3) {
                this->lcd.print(F(" BACKLIGHT :")) ;
                this->lcd.print(backlightAvailable ? F("ON ") : F("OFF")) ;
            } else if (m == 4) {
                this->lcd.print(F(" CLEAR BONDING ")) ;
            }
            this->lcd.setCursor(15, 1) ;
            this->lcd.blink() ;
        }
        
        buttons = this->lcd.readButtons() ;

        // debounce
        if (buttons) {
            byte b = 0x1 ;
            do {
                delay(30) ;
                b = this->lcd.readButtons() ;
            } while (b) ;

            this->lcd.noBlink() ;
        }

        if (buttons & BUTTON_SELECT && m == 0) {
            break ;
        }

        if (buttons & BUTTON_LEFT) {
            om = m ;
            m = m > 0 ? m - 1 : 4 ;
            continue ;
        } else if (buttons & BUTTON_RIGHT) {
            om = m ;
            m = m < 4 ? m + 1 : 0 ;
            continue ;
        } else if (m == 1 && buttons & BUTTON_SELECT) {
            for (uint8_t i = 0; i < CATEGORY_COUNT; i++) {
                this->notifications[i] = 0 ;
            }
            om = m ;
            m = 0 ;
        } else if (m == 2 && buttons & BUTTON_SELECT) {
            om = 0 ;
            this->options ^= OPTIONS_SOUND ;
            if (this->options & OPTIONS_SOUND) {
                uint8_t buzzer[8] = ICO_SOUND_ON ;
                this->lcd.createChar(1, buzzer) ;
            } else {
                uint8_t buzzer[8] = ICO_SOUND_OFF ;
                this->lcd.createChar(1, buzzer) ;
            }
        } else if (m == 3 && buttons & BUTTON_SELECT) {
            om = 0 ;
            this->options ^= OPTIONS_LIGHT ;
            this->options &= ~OPTIONS_BACKLIGHT ;
        } else if (m == 4 && buttons & BUTTON_SELECT) {
            om = m ;
            m = 0 ;
            result |= 0x1 ;
        }
    } 

    this->fire() ;
    if (!backlightAvailable | !baclkightOn) {
        this->lcd.setBacklight(LCD_BACKLIGHT_OFF) ;
    }
    return result ;
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

    this->lcd.setCursor(0, 1) ;
    this->lcd.write((char) 0x1) ;

    this->lcd.setCursor(0, 0) ;
    this->lcd.write((char) 0x0) ;
    
    if (total == 0) {
        this->options &= ~OPTIONS_FLASH ;
        this->lcd.print(F(" THERE ARE NO")) ;
        this->lcd.setCursor(2, 1) ;
        this->lcd.print(F("NOTIFICATIONS")) ;
    } else if (this->notifications[AncsNotificationCategoryIdMissedCall] > 0) {
        this->options |= OPTIONS_FLASH ;
        this->lcd.print(F("  YOU MISSED ")) ;
        this->lcd.print(this->notifications[AncsNotificationCategoryIdMissedCall]) ;
        this->lcd.setCursor(5, 1) ;
        this->lcd.print(F("CALL(S)")) ;
    } else if (this->notifications[AncsNotificationCategoryIdIncomingCall] > 0) {
        this->options |= OPTIONS_FLASH ;
        this->lcd.print(F(" INCOMING CALL")) ;
    } else if (this->notifications[AncsNotificationCategoryIdEmail] > 0) {
        this->options |= OPTIONS_FLASH ;
        this->lcd.print(F("  YOU'VE GOT ")) ;
        this->lcd.print(this->notifications[AncsNotificationCategoryIdEmail]) ;
        this->lcd.setCursor(5, 1) ;
        this->lcd.print(F("MAIL(S)")) ;
    } else if (this->notifications[AncsNotificationCategoryIdSocial] > 0) {
        this->options |= OPTIONS_FLASH ;
        this->lcd.setCursor(4, 0) ;
        this->lcd.print(this->notifications[AncsNotificationCategoryIdSocial]) ;
        this->lcd.print(F(" NEW CHAT")) ;
        this->lcd.setCursor(5, 1) ;
        this->lcd.print(F("MESSAGES")) ;
    } else {
        this->options |= (OPTIONS_FLASH | OPTIONS_BACKLIGHT) ;
        this->lcd.setBacklight(LCD_BACKLIGHT_ON) ;

        this->lcd.print(F(" THERE ARE ")) ;
        this->lcd.print(total) ;
        this->lcd.setCursor(2, 1) ;
        this->lcd.print(F("NOTIFICATIONS")) ;
    }

    if (!backlightAvailable) {
        return ;
    }
    
    if (!baclkightOn) {
        this->options |= OPTIONS_BACKLIGHT ;
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

