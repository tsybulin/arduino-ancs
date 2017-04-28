#ifndef ANCS_H
#define ANCS_H

#include <BLEPeripheral.h>

class ANCS ;

typedef void (*ANCSEventHandler)(ANCS *ancs) ;

class ANCS : public BLEPeripheral {
    public:
        ANCS() ;
        void setup() ;
        void poll() ;
        void clearStoreData() ;
        void setEventHandler(BLEPeripheralEvent event, ANCSEventHandler handler) ;
        void setNoficationHandler(BLERemoteCharacteristicEventHandler handler) ;
    protected:
        virtual void BLEDeviceConnected(BLEDevice& device, const unsigned char* address) ;
        virtual void BLEDeviceDisconnected(BLEDevice& device) ;
        virtual void BLEDeviceBonded(BLEDevice& device) ;
        virtual void BLEDeviceRemoteServicesDiscovered(BLEDevice& device) ;
    private:
        BLEBondStore  bleBondStore ;
        BLERemoteService ancsService  ;
        BLERemoteCharacteristic ancsNotificationSourceCharacteristic ;
        ANCSEventHandler eventHandlers[4]  ; 
} ;

#endif

