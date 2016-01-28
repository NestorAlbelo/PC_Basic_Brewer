#ifndef XMK_H
#define XMK_H
  
#define SOH         0x01                    /* ^A Start of header prefix */
#define STX         0x02                    /* ^B Start of header prefix */
#define EOT         0x04                    /* ^D End of Transmission signal */
#define ACK         0x06                    /* ^F Receiver acknowledge */
#define NAK         0x15                    /* ^U Receive not acknowledge */
#define CAN         0x18                    /* ^X Cancel transmission */
#define C           0x43                    /*  C CRC method delimiter */
#define SUB         0X1A                    /* ^Z END OF FILE indicator */
#define G           0X47                    /* G  YMODEM-G */
  
/* definition of and allowable values of XMK_ABORT_FLAG(port_no) */
#define   XMK_ABORT_FLAG(port_no)      xmk_abort_flag[port_no]
#define   XMK_NO_ABORT    0   /* no Control-Break or Data Carrier Detect enabled */
#define   XMK_CB          1   /* Only Control-Break enabled */
#define   XMK_DCD         2   /* Only Data Carrier Detect enabled */
#define   XMK_CB_AND_DCD  3   /* Control-Break and Data Carrier Detect enabled */
  
  
#endif   /* XMK_H */
