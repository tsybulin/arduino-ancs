#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

struct AncsNotification {
  unsigned char eventId;
  unsigned char eventFlags;
  unsigned char catergoryId;
  unsigned char catergoryCount;
  unsigned long notificationUid;
} ;

/*
enum AncsNotificationEventId {
  AncsNotificationEventIdAdded    = 0,
  AncsNotificationEventIdModified = 1,
  AncsNotificationEventIdRemoved  = 2
} ;

enum AncsNotificationEventFlags {
  AncsNotificationEventFlagsSilent         = 1,
  AncsNotificationEventFlagsImportant      = 2,
  AncsNotificationEventFlagsPositiveAction = 4,
  AncsNotificationEventFlagsNegativeAction = 8
} ;
*/

#endif

