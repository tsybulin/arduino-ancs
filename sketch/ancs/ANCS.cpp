#include "ANCS.h"

ANCS::ANCS() :
    BLEPeripheral(),
    bleBondStore(),
    ancsService("7905f431b5ce4e99a40f4b1e122d00d0"),
    ancsNotificationSourceCharacteristic("9fbf120d630142d98c5825e699a21dbd", BLENotify)
{
    memset(this->eventHandlers, 0x00, sizeof(this->eventHandlers)) ;
}

void ANCS::setup() {
    this->bleBondStore.clearData() ;
    this->setBondStore(this->bleBondStore) ;
    this->setServiceSolicitationUuid(this->ancsService.uuid()) ;
    this->setLocalName("ANCS") ;
    this->setDeviceName("Arduino ANCS") ;
    this->setAppearance(0x0080) ; // Ordinal PC
    this->addRemoteAttribute(this->ancsService) ;
    this->addRemoteAttribute(this->ancsNotificationSourceCharacteristic) ;
    
    this->begin() ;
}

void ANCS::setEventHandler(BLEPeripheralEvent event, ANCSEventHandler handler) {
    this->eventHandlers[event] = handler ;
}

void ANCS::setNoficationHandler(BLERemoteCharacteristicEventHandler handler) {
    this->ancsNotificationSourceCharacteristic.setEventHandler(BLEValueUpdated, handler) ;
}

void ANCS::poll() {
    BLEPeripheral::poll() ;
}

void ANCS::BLEDeviceConnected(BLEDevice& device, const unsigned char* address) {
    BLEPeripheral::BLEDeviceConnected(device, address) ;

    if (this->eventHandlers[BLEConnected]) {
        this->eventHandlers[BLEConnected](this) ;
    }
}

void ANCS::BLEDeviceDisconnected(BLEDevice& device) {
    BLEPeripheral::BLEDeviceDisconnected(device) ;

    if (this->eventHandlers[BLEDisconnected]) {
        this->eventHandlers[BLEDisconnected](this) ;
    }
}

void ANCS::BLEDeviceBonded(BLEDevice& device) {
    BLEPeripheral::BLEDeviceBonded(device) ;
    
    if (this->ancsNotificationSourceCharacteristic.canSubscribe()) {
        this->ancsNotificationSourceCharacteristic.subscribe() ;
    }

    if (this->eventHandlers[BLEBonded]) {
        this->eventHandlers[BLEBonded](this) ;
    }
}

void ANCS::BLEDeviceRemoteServicesDiscovered(BLEDevice& device) {
    BLEPeripheral::BLEDeviceRemoteServicesDiscovered(device) ;

    if (this->ancsNotificationSourceCharacteristic.canSubscribe()) {
        this->ancsNotificationSourceCharacteristic.subscribe() ;
    }

    if (this->eventHandlers[BLERemoteServicesDiscovered]) {
        this->eventHandlers[BLERemoteServicesDiscovered](this) ;
    }
}

