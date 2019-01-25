 // categories
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_SYSTEM                  0x0
#define FACILITY_RUNTIME                 0x2
#define FACILITY_STUBS                   0x3
#define FACILITY_IO_ERROR_CODE           0x4


//
// Define the severity codes
//
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: idcSuccess
//
// MessageText:
//
// Success
//
#define idcSuccess                       ((WORD)0x00000001L)

//
// MessageId: idcInfo
//
// MessageText:
//
// Info
//
#define idcInfo                          ((WORD)0x00000002L)

//
// MessageId: idcWarning
//
// MessageText:
//
// Warning
//
#define idcWarning                       ((WORD)0x00000003L)

//
// MessageId: idcError
//
// MessageText:
//
// Error
//
#define idcError                         ((WORD)0x00000004L)

 // messages
//
// MessageId: ideUnknown
//
// MessageText:
//
// %1
//
#define ideUnknown                       ((DWORD)0x00000000L)

//
// MessageId: ideSuccess
//
// MessageText:
//
// %1
//
#define ideSuccess                       ((DWORD)0x00000064L)

//
// MessageId: ideInfo
//
// MessageText:
//
// %1
//
#define ideInfo                          ((DWORD)0x400000C8L)

//
// MessageId: ideWarning
//
// MessageText:
//
// %1
//
#define ideWarning                       ((DWORD)0x8000012CL)

//
// MessageId: ideError
//
// MessageText:
//
// %1
//
#define ideError                         ((DWORD)0xC0000190L)

