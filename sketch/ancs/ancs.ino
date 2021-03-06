#include "ANCS.h"
#include "LCD.h"
#include "notifications.h"

ANCS ancs ;
LCD lcd ;
void (*resetFunc)(void) = 0 ;

void setup() {
    ancs.setEventHandler(BLEConnected, didConnected) ;
    ancs.setEventHandler(BLEDisconnected, didDisconnected) ;
    ancs.setEventHandler(BLEDisconnected, didBonded) ;
    ancs.setEventHandler(BLERemoteServicesDiscovered, didDiscovered) ;
    ancs.setNoficationHandler(notificatonDidChanged) ;
    byte options = lcd.setup() ;
    if (options & 0x1) {
        ancs.clearStoreData() ;
        resetFunc() ;
    }
    ancs.setup() ;
}

void loop() {
    ancs.poll() ;
    byte result = lcd.poll() ;
    if (result & 0x1) {
        ancs.clearStoreData() ;
        resetFunc() ;
    }
}

void didConnected(ANCS *ancs) {
    if (ancs) lcd.didConnected() ;
}

void didDisconnected(ANCS *ancs) {
    if (ancs) lcd.didDisconnected() ;
}

void didBonded(ANCS *ancs) {
    if (ancs) lcd.didBonded() ;
}

void didDiscovered(ANCS *ancs) {
    if (ancs) lcd.didDiscovered() ;
}

void notificatonDidChanged(BLECentral& central, BLERemoteCharacteristic& characteristic) {
    if (!central) {
        return ;
    }

    struct AncsNotification notification ;
    memcpy(&notification, characteristic.value(), sizeof(notification)) ;
    lcd.notificatonDidChanged(notification.catergoryId, notification.catergoryCount) ;
}

