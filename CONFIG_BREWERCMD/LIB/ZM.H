/*
 *   Z M O D E M . H     Manifest constants for ZMODEM
 *    application to application file transfer protocol
 *    05-23-87  Chuck Forsberg Omen Technology Inc
 */

#ifndef ECLZMHEAD
#define ECLZMHEAD

typedef (*userfunc)( int port, int event, char *file, long bytes, long size,
	int acked, int naked, int timed, int junked, int error_ck );

#define ZPAD '*'	/* 052 Padding character begins frames */
#define ZDLE 030	/* Ctrl-X Zmodem escape - `ala BISYNC DLE */
#define ZDLEE (ZDLE^0100)	/* Escaped ZDLE as transmitted */
#define ZBIN 'A'	/* Binary frame indicator */
#define ZHEX 'B'	/* HEX frame indicator */
#define ZBIN32 'C'	/* Binary frame with 32 bit FCS */

/* Frame types (see array "frametypes" in zm.c) */
#define ZRQINIT	0	/* Request receive init */
#define ZRINIT	1	/* Receive init */
#define ZSINIT 2	/* Send init sequence (optional) */
#define ZACK 3		/* ACK to above */
#define ZFILE 4		/* File name from sender */
#define ZSKIP 5		/* To sender: skip this file */
#define ZNAK 6		/* Last packet was garbled */
#define ZABORT 7	/* Abort batch transfers */
#define ZFIN 8		/* Finish session */
#define ZRPOS 9		/* Resume data trans at this position */
#define ZDATA 10	/* Data packet(s) follow */
#define ZEOF 11		/* End of file */
#define ZFERR 12	/* Fatal Read or Write error Detected */
#define ZCRC 13		/* Request for file CRC and response */
#define ZCHALLENGE 14	/* Receiver's Challenge */
#define ZCOMPL 15	/* Request is complete */
#define ZCAN 16		/* Other end canned session with CAN*5 */
#define ZFREECNT 17	/* Request for free bytes on filesystem */
#define ZCOMMAND 18	/* Command from sending program */
#define ZSTDERR 19	/* Output to standard error, data follows */

/* ZDLE sequences */
#define ZCRCE 'h'	/* CRC next, frame ends, header packet follows */
#define ZCRCG 'i'	/* CRC next, frame continues nonstop */
#define ZCRCQ 'j'	/* CRC next, frame continues, ZACK expected */
#define ZCRCW 'k'	/* CRC next, ZACK expected, end of frame */
#define ZRUB0 'l'	/* Translate to rubout 0177 */
#define ZRUB1 'm'	/* Translate to rubout 0377 */

/* zdlread return values (internal) */
/* -1 is general error, -2 is timeout */
#define GOTOR 0400
#define GOTCRCE (ZCRCE|GOTOR)	/* ZDLE-ZCRCE received */
#define GOTCRCG (ZCRCG|GOTOR)	/* ZDLE-ZCRCG received */
#define GOTCRCQ (ZCRCQ|GOTOR)	/* ZDLE-ZCRCQ received */
#define GOTCRCW (ZCRCW|GOTOR)	/* ZDLE-ZCRCW received */
#define GOTCAN	(GOTOR|030)	/* CAN*5 seen */

/* Byte positions within header array */
#define ZF0	3	/* First flags byte */
#define ZF1	2
#define ZF2	1
#define ZF3	0
#define ZP0	0	/* Low order 8 bits of position */
#define ZP1	1
#define ZP2	2
#define ZP3	3	/* High order 8 bits of file position */

/* Bit Masks for ZRINIT flags byte ZF0 */
#define CANFDX	01	/* Rx can send and receive true FDX */
#define CANOVIO	02	/* Rx can receive data during disk I/O */
#define CANBRK	04	/* Rx can send a break signal */
#define CANCRY	010	/* Receiver can decrypt */
#define CANLZW	020	/* Receiver can uncompress */
#define CANFC32	040	/* Receiver can use 32 bit Frame Check */
#define ESCCTL 0100	/* Receiver expects ctl chars to be escaped */
#define ESC8   0200	/* Receiver expects 8th bit to be escaped */

/* Parameters for ZSINIT frame */
#define ZATTNLEN 32	/* Max length of attention string */

/* Bit Masks for ZSINIT flags byte ZF0 */
#define TESCCTL 0100	/* Transmitter expects ctl chars to be escaped */
#define TESC8   0200	/* Transmitter expects 8th bit to be escaped */

/* Parameters for ZFILE frame */
/* Conversion options one of these in ZF0 */
#define ZCBIN	1	/* Binary transfer - inhibit conversion */
#define ZCNL	2	/* Convert NL to local end of line convention */
#define ZCRESUM	3	/* Resume interrupted file transfer */
/* Management include options, one of these ored in ZF1 */
#define ZMSKNOLOC	0200	/* Skip file if not present at rx */
/* Management options, one of these ored in ZF1 */
#define ZMMASK	037	/* Mask for the choices below */
#define ZMNEWL	1	/* Transfer if source newer or longer */
#define ZMCRC	2	/* Transfer if different file CRC or length */
#define ZMAPND	3	/* Append contents to existing file (if any) */
#define ZMCLOB	4	/* Replace existing file */
#define ZMNEW	5	/* Transfer if source newer */
	/* Number 5 is alive ... */
#define ZMDIFF	6	/* Transfer if dates or lengths different */
#define ZMPROT	7	/* Protect destination file */
/* Transport options, one of these in ZF2 */
#define ZTLZW	1	/* Lempel-Ziv compression */
#define ZTCRYPT	2	/* Encryption */
#define ZTRLE	3	/* Run Length encoding */
/* Extended options for ZF3, bit encoded */
#define ZXSPARS	64	/* Encoding for sparse file operations */

/* Parameters for ZCOMMAND frame ZF0 (otherwise 0) */
#define ZCACK1	1	/* Acknowledge, then do command */

/* Possible errors when running ZMODEM */
#define	ZM_OUT_OF_MEM	-9

#define	ZM_OLDER	-10			/* existing file is newer than old one */
								/* when ZMNEW option is used */

#define	ZM_MATCH	-11			/* file already exists with same date and */
								/* length, when ZMDIFF option is used */

#define	ZM_CANT_RESUME	-12		/* can't resume - new file is smaller than */
								/* the existing file.  */

#define	ZM_EXISTS	-13			/* file already exists, resume not specified */

#define ZM_SKIP		-14			/* file doesn't exist (and is skipped due to */
								/* ZMSKNOLOC option */

#define	ZM_FIO		-15			/* unable to read/write/open file */

#define	ZM_CANC		-16			/* Remote cancelled */

#define	ZM_ABORT	-17			/* User pressed CTRL Break */

#define	ZM_NO_DCD	-18			/* Abort - no carrier */


#define	ZM_ERROR	-1
#define ZM_NAK		-1

#define	ZM_OK		0			/* all is ok */

/* user abort flags */
#define	ZM_CB		1
#define	ZM_DCD		2
#define	ZM_CB_AND_DCD 3

/* ZMODEM error check methods */
#define	ZM_CRC_16   0
#define	ZM_CRC_32   1


/* function type flags */
#define	ZM_REC		0
#define	ZM_SEND		1

#define    ZM_SESS             -1


/* zm_setopts() parameter flags */
#define	ZM_RESUME	0
#define	ZM_CLOBBER	1
#define	ZM_WINDOW	2
#define	ZM_TIMEOUT	3
#define	ZM_NULLS	4
#define	ZM_ESCCTL	5
#define	ZM_UNLINK	6
#define	ZM_DOTTOSL	7
#define	ZM_WANTCRC32 8
#define ZM_USERFUNC	9
#define	ZM_AUTO		10


/*
**	User function defines
*/
#define	ZM_BADCRC		1
#define	ZM_SENDERCAN	2
#define	ZM_TIMED		3
#define	ZM_BADSUB		4
#define	ZM_SUB2LONG		5
#define	ZM_GAREXCEED	6
#define	ZM_BADESC		7
#define	ZM_FAILSEC		8
#define	ZM_CRASHREC		9
#define	ZM_SENDSKIP		10
#define	ZM_WAITNAK		11
#define	ZM_GOTZRPOS		12
#define	ZM_GOTERR		13
#define	ZM_BYTES		14


/* misc macro values */
#define MAX_REINIT	10
#define	TRUE		1
#define	FALSE		0
#define HOWMANY		133
#define PATHLEN		127

#define ON			1
#define OFF			0

#define	CB_ABORT	1
#define	DCD_ABORT	2

/* Sorry, Regulus and some others don't work right in raw mode! */
#define Readnum		1

#define DEFBYTL		2000000000L	/* default rx file size */
#define TXBSIZE		1024
#define TXBMASK		(TXBSIZE-1)


/* Ward Christensen / CP/M parameters - Don't change these! */
#define ENQ			005
#define CAN			0x18
#define XOFF		('s'&037)
#define XON			('q'&037)
#define SOH			0x01
#define STX			0x02
#define EOT			0x04
#define ACK			0x06
#define NAK			0x15
#define CPMEOF		032
#define WANTCRC		0103	/* send C not NAK to get crc not checksum */
#define TIMEOUT		(-2)
#define RCDO		(-3)
#define WCEOT		(-10)

/* macros to make calling things a little easier */
#ifdef __TURBOC__
#define ZM_FIND_FIRST(path,buf_ptr)     findfirst(path,buf_ptr,0)
#define ZM_FIND_NEXT(buf_ptr)           findnext(buf_ptr)
#define ZM_FILE_NAME                    fib.ff_name
#define ZM_FILE_LENGTH                  fib.ff_fsize
#define ZM_FILE_DATE                    fib.ff_fdate
#else
#define ZM_FIND_FIRST(path,buf_ptr)     _dos_findfirst(path,_A_NORMAL,buf_ptr)
#define ZM_FIND_NEXT(buf_ptr)           _dos_findnext(buf_ptr)
#define ZM_FILE_NAME                    fib.name
#define ZM_FILE_LENGTH                  fib.fsize
#define ZM_FILE_DATE                    fib.wr_fdate
#endif

#define	v_delay(t)			xct_pend((t))
#define	filenstr(s)			(s)
#define	setnewpath(s)		(s)
#define	sleep( secs )		v_delay( (secs) * 18 )
#define	sendline( c )		xc_putc( Port, (char) (c) )
#define xsendline(c)		xc_putc( Port, (char) (c) )

#endif

/* End of ZMODEM.H */
