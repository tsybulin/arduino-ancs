#include "ANCS.h"
#include "LCD.h"

ANCS ancs ;
LCD lcd ;

void setup() {
    ancs.setEventHandler(BLEConnected, didConnected) ;
    ancs.setEventHandler(BLEDisconnected, didDisconnected) ;
    ancs.setEventHandler(BLEDisconnected, didBonded) ;
    ancs.setEventHandler(BLERemoteServicesDiscovered, didDiscovered) ;
    ancs.setNoficationHandler(notificatonDidChanged) ;
    lcd.setup() ;
    ancs.setup() ;
}

void loop() {
    ancs.poll() ;
    lcd.poll() ;
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

//enum AncsNotificationEventId {
//  AncsNotificationEventIdAdded    = 0,
//  AncsNotificationEventIdModified = 1,
//  AncsNotificationEventIdRemoved  = 2
//};
//
//enum AncsNotificationEventFlags {
//  AncsNotificationEventFlagsSilent         = 1,
//  AncsNotificationEventFlagsImportant      = 2,
//  AncsNotificationEventFlagsPositiveAction = 4,
//  AncsNotificationEventFlagsNegativeAction = 8
//};

struct AncsNotification {
  unsigned char eventId;
  unsigned char eventFlags;
  unsigned char catergoryId;
  unsigned char catergoryCount;
  unsigned long notificationUid;
} ;

void notificatonDidChanged(BLECentral& central, BLERemoteCharacteristic& characteristic) {
    if (!central) {
        return ;
    }

    struct AncsNotification notification ;
    memcpy(&notification, characteristic.value(), sizeof(notification)) ;
    lcd.notificatonDidChanged(notification.catergoryId, notification.catergoryCount) ;
}
