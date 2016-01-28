#ifndef ECL_XMHEAD
#define ECL_XMHEAD

/*
    XM.h    Include file for XMODEM module
    Updated rev 2.0 extensions
  
    ====================================================================
        Status returns from XMODEM functions
    ==================================================================== */
  
#define XM_COMPL      1                 /* Successful completion */
#define XM_OK         0                 /* Successful ACKnowledgement */
  
#define XM_SESS      -1                 /* No session in progress */
#define XM_NAK       -2                 /* Not acknowledged or refused */
#define XM_CANC      -3                 /* Other cpu cancelled */
#define XM_ABORT     -4                 /* User aborted */
  
#define XM_FOPEN     -5                 /* File error during open */
#define XM_FIO       -6                 /* File error during I/O */
#define XM_SILENCE   -7                 /* Rev 2.0 No response yet */
#define XM_NO_DCD    -8                 /* Rev 3.0 NO Data Carrier Detect Signal */
#define XM_OUT_OF_MEM -9                /* Rev 3.0 Unable to allocate asked for memory */
  
/*  ====================================================================
    Miscellaneous
    ==================================================================== */
  
#define XM_MAXB      1024               /* Rev 2.0 Maximum data field size */
#define XMODEM_1K    1024               /* Rev 2.0 For CRC, 1K data frames */
#define XMODEM_128   128                /* Rev 2.0 For CRC, 128 byte data frames */
  
#define XMODEM_CRC   0                  /* Rev 2.0 For CRC checksum method */
#define XMODEM_XSUM  1                  /* Rev 2.0 For Additive checksum method */

#endif
  
