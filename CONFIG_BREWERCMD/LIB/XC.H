#ifndef  ECL_XCHEAD
#define  ECL_XCHEAD

/*
    XC.h        C interface to X_COMMS functions
   revision 2.0    Updated  to include additional communications ports
                    and the Comms_str structure definition.

    +======================================================+
    ||                 W A R N I N G ! ! !                ||
    ||                                                    ||
    ||  If you change the number of ports supported in    ||
    ||  MAX_PORTS (below), you must change the default    ||
    ||  definitions in the IM.C, XM.C, and XMK.C modules. ||
    ||                                                    ||
    +======================================================+
*/

/*  The port unit definitions for all communications services */

#define     MAX_PORTS   34                       /* Maximum ports */

#define     COM1        0                       /* Async Adapter COM1: */
#define     COM2        1                       /* Async Adapter COM2: */
#define     COM3        2                       /* Async Adapter COM3: */
#define     COM4        3                       /* Async Adapter COM4: */
#define     COM5        4                       /* Async Adapter COM5: */
#define     COM6        5                       /* Async Adapter COM6: */
#define     COM7        6                       /* Async Adapter COM7: */
#define     COM8        7                       /* Async Adapter COM8: */
#define     COM9        8                       /* Async Adapter COM9: */
#define     COM10       9                       /* Async Adapter COM10: */
#define     COM11       10 							/* ETC. */
#define     COM12       11 
#define     COM13       12 
#define     COM14       13 
#define     COM15       14 
#define     COM16       15 
#define     COM17       16 
#define     COM18       17 
#define     COM19       18 
#define     COM20       19 
#define     COM21       20 
#define     COM22       21 
#define     COM23       22 
#define     COM24       23 
#define     COM25       24 
#define     COM26       25 
#define     COM27       26 
#define     COM28       27
#define     COM29       28 
#define     COM30       29 
#define     COM31       30 
#define     COM32       31 
#define     COM33       32 
#define     COM34       33 

/*  The INIT (ah=00, Int 14h) communications parameters */

#define     BAUD110     0                       /* Baud rate 110 */
#define     BAUD150     1                       /* Baud rate 150 */
#define     BAUD300     2                       /* Baud rate 300 */
#define     BAUD600     3                       /* Baud rate 600 */
#define     BAUD1200    4                       /* Baud rate 1200 */
#define     BAUD2400    5                       /* Baud rate 2400 */
#define     BAUD4800    6                       /* Baud rate 4800 */
#define     BAUD9600    7                       /* Baud rate 9600 */
#define     BAUD19200   8                       /* Baud rate 19200 - REV 2 */
#define     BAUD38400   9                       /* Baud rate 38400 - REV 2 */
#define     BAUD57K  	10                      /* Baud rate 57K  - REV 3 */
#define     BAUD115K 	11                      /* Baud rate 115K  - REV 3 */

#define     DATA7       0                       /* Data bits 7 */
#define     DATA8       1                       /* Data bits 8 */

#define     NOPAR       0                       /* Parity none */
#define     ODDPAR      1                       /* Parity odd */
#define     EVENPAR     2                       /* Parity even */

#define     STOP1       0                       /* Stop bits 1 */
#define     STOP2       1                       /* Stop bits 2 */


#define		AST			2								/* AST 4PORT DOS "Expanded Mode" */
#define		DIGI8			4								/* DIGIBORD COM/4/8 REV 3 */
#define		STAR8			6								/* STARGATE PLUS8 REV 3 */
#define 		PS2			8								/* PS2 nd generic search */
#define		DIGIMC		10								/* DIGIBOARD Microchannel */
#define     DIGIPC16    12                      /* DigiBoard PC/16 */
#define     ASTCMP      14                      /* AST 4PORT DOS "Compatible Mode" */

#define		COMMS_BUFFER_SIZE		1024				/* Number of bytes in each comms buffer */

/* Flow control bit masks (.XON_flag) */
#define		FC_CLEAR        0x00        /* Flow control is Cleared */
#define		FC_ENABLED      0x01        /* Flow control is enabled */
#define		FC_XOFF_SENT    0x02        /* Xoff has been transmitted */
#define		FC_XOFF_RCVD    0x04        /* Xoff has been received */
#define		FC_SEND_XOFF    0x08        /* Xoff needs to be transmitted */
#define		FC_SEND_XON     0x10        /* Xon needs to be transmitted */
#define		FC_REC_ON       0x20        /* REC Flow control enabled */
#define		FC_TX_ON        0x40        /* TX Flow control enabled */

#define     EOI             0x20        /* End Of Interrupt */
#define     ICA             0x21        /* Interrupt Controller */

/*
    The <Comms_str> structure defines the data structure used in
    XCOMMS to keep track of each port's state.  This structure may
    change in future revisions - use at your own RISK!

*/
typedef struct  Comms_str {
    int             Port_addr;                  /* +00 COM1 or COM2 port addr */
    char            Vector;                     /* +02 Interrupt vector */
    char            IC_Mask;                    /* +03 Interrupt controller */
    unsigned int    New_offset;                 /* +04 Current handler addr */
    unsigned int    New_segment;                /* +06 Current handler addr */
    int             State;                      /* +08 State of handler */
    char            DTR_control;                /* +10 DTR, RTS control flag */
    char            Break_detect;               /* +11 Line break detect flag */

    unsigned int    Old_offset;                 /* +12 Previous handler addr */
    unsigned int    Old_segment;                /* +14 Previous handler addr */
    long            Recv_cnt;                   /* +16 32-bit receive count */
    int             Buffers;                    /* +20 Number of buffers */

    unsigned int    Int_offset;                 /* +22 Interrupt buffer database */
    unsigned int    Int_Segment;                /* +24 */
    unsigned int    Int_Index;                  /* +26 */

    unsigned int    Read_Offset;                /* +28 Read buffer database */
    unsigned int    Read_Segment;               /* +30 */
    unsigned int    Read_Index;                 /* +32 */

    int             Errors;                     /* +34 Error flag */
    int             XON_flag;                   /* +36 XOFF sent? */
    int             High_mark;                  /* +38 XON/XOFF water mark */
    int             Low_mark;                   /* +40 XON/XOFF water mark */

    int             xcount;                     /* +42 Bytes left to transmit */
    unsigned int    xptr_Offset;                /* +44 Current transmit buffer address */
    unsigned int    xptr_Segment;               /* +46 */
    int             unit;                       /* +48 Comms unit number * 2 */
	 int				  UART_Mode; 				      /* +50 16450: 0; 16550: (nonzero) contains trigger and mode information */
	 int 				  IXmit_Base;
	 int				  IXmit_Top;
	 int				  IRec_Base;
	 int				  IRec_Top;
    int 				  rxbusy;
}Comms_structure ;

#endif

