#ifndef ECLDIGIPCXX
#define ECLDIGIPCXX

/* Intelligent Board State */
#define  ECL_DIGIPC_Xx			0x04

/* Intelligent Board Types */
#define  ECL_DIGIPC_Xe         30   /* Digiboard PC/Xe boards */
#define  ECL_DIGIPC_Xi         31   /* Digiboard PC/Xi boards */
#define  ECL_DIGIPC_Xm         32   /* Digiboard PC/Xm boards */

/* FEP Returns */
#define  ECL_PCXX_CMDQ_OK       0
#define  ECL_PCXX_BAD_BOARD    -2
#define  ECL_PCXX_MEM_FAIL     -3
#define  ECL_PCXX_BIOS_FAIL    -4
#define  ECL_PCXX_FEP_FAIL     -5
#define  ECL_PCXX_CMDQ_FULL    -6

/* Event Flag Masks */
#define  PCXX_TX_EVENT_MASK		0x06		/* Tx low water or empty mask */
#define  PCXX_RX_AVAIL_MASK		0x08
#define  PCXX_MOD_CHNG_MASK		0x20
#define  PCXX_BRK_REC_MASK		   0x01

/* MSTAT Masks */
#define  PCXX_RTS_MASK		0x02
#define  PCXX_DCD_MASK		0x08
#define  PCXX_DSR_MASK		0x10
#define  PCXX_CTS_MASK		0x20
#define  PCXX_RI_MASK		0x40
#define  PCXX_DTR_MASK   	0x80

/* PC/Xx Input Flags */
#define  PCXX_BRKINT             0x02
#define  PCXX_IGPAR              0x04
#define	PCXX_IXON               0x0400
#define	PCXX_IXOFF              0x1000
#define	PCXX_IXONA              0x2000

/* PC/Xx HFLOW Flags */
#define  PCXX_CTS_FC             0x10
#define  PCXX_DSR_FC             0x20


/* FEP I/O port commands */
#define	FEP_MEMON			0x02		/* PC/X* dev[0] Board Memory On		*/
#define	FEP_MEMOFF			0x00		/* PC/X* dev[0] Board Memory OFF		*/
#define	FEP_RESET			0x04		/* PC/X* dev[0] Board Reset 		*/
#define	FEP_INT				0x08		/* PC/X* dev[0] Host to Board Interrupt	*/	

#define  BIOS_POST_OFFSET	0xC00		/* Offset of BIOS Post Area */
#define  BIOS_SIGNATURE		0x4447	/* BIOS Reverse Signature */
#define  FEP_SIGNATURE		0x534F	/* FEP Confirm Signature */

#define  FEP_START_OFFSET	0x2000	/* Offset of starting address of FEP */
#define  HB_MAIL_OFFSET		0x0C40	/* Offset of HOST to BIOS Mailbox */
#define  FEPFAIL_OFFSET		0x0C12	/* Offset of FEP fail board code */
#define  PCXX_CHAN_START  	0x1000   /* starting offset of Channel Structures */
#define  PCXX_CHAN_SIZE	  	0x80		/* Size of each Channel Stucture */

/* Miscellaneous Communications Area */
#define  PCXX_NPORT			0x0C22	/* Offset of Number of asynchonous channels */

#define  PCXX_CIN				0x0D10   /* Command in pointer */
#define  PCXX_COUT			0x0D12 	/* Command out pointer */
#define  PCXX_CSTART			0x0D14 	/* Start of Command Buffer */
#define  PCXX_CMAX			0x0D16 	/* End of Command Buffer */

#define  PCXX_EIN				0x0D18   /* Event in pointer */
#define  PCXX_EOUT			0x0D1A 	/* Event out pointer */
#define  PCXX_ISTART			0x0D1C 	/* Start of Event Buffer */
#define  PCXX_IMAX			0x0D1E 	/* End of Event Buffer */

#define  FEPSTAT_OFFSET		0x0D20	/* Offset of FEP Signature */

#define  PCXX_INTERVAL		0x0E04 	/* Interval between interrupts */

/* Channel Data Structure */
#define  PCXX_TSEG			0x08
#define  PCXX_TIN				0x0A
#define  PCXX_TOUT			0x0C
#define  PCXX_TMAX			0x0E
#define  PCXX_RSEG			0x10
#define  PCXX_RIN				0x12
#define  PCXX_ROUT			0x14
#define  PCXX_RMAX			0x16
#define  PCXX_IFLAG			0x28
#define  PCXX_TBUSY			0x4B
#define  PCXX_IEMPTY			0x4C
#define  PCXX_ILOW			0x4D
#define  PCXX_IDATA			0x4E
#define  PCXX_MSTAT			0x54
#define  PCXX_MINT			0x56
#define  PCXX_LSTAT			0x57
#define  PCXX_ORUN			0x59


/* Digiboard PC/Xx function numbers */
#define	PCXX_XCLINK       0x01
#define	PCXX_XCINIT       0x02
#define	PCXX_XCUNLK       0x03
#define	PCXX_XCUPUT       0x04
#define	PCXX_XCTEST			0x05
#define	PCXX_XCFLUSH 		0x06
#define	PCXX_XCUFLUSH 		0x07
#define	PCXX_XCBRK	 		0x08
#define	PCXX_XCBRKD	 		0x09
#define	PCXX_XCCTS	 		0x0A
#define	PCXX_XCDCD	 		0x0B
#define	PCXX_XCDSR	 		0x0C
#define	PCXX_XCRTS	 		0x0D
#define	PCXX_XCRI	 		0x0E
#define	PCXX_XCLSTAT 		0x0F
#define	PCXX_XCDTR	 		0x10
#define	PCXX_XCGET	 		0x11
#define	PCXX_XCGETC	 		0x12
#define	PCXX_XCFCKILL     0x13
#define  PCXX_XCFCOFF      0x14
#define  PCXX_XCFCON       0x15


/* flags to manage receive buffer access */
#define  PCXX_RXCLEAR		0x00
#define  PCXX_RXBUSY			0x01

/* PC/Xx Structure */
typedef struct {
	unsigned int	io_port;
	unsigned char	irq;
	unsigned int	window_dseg;
	unsigned int	memory_seg;
	unsigned long	memory_size;
	unsigned char	starting_port;
	unsigned char	nports;
	unsigned char	link_count;
	unsigned char	ic_mask;
	unsigned char	shared_vector;
	int 				board_type;
}	PC_Xx_structure;

/* Event Structure */
typedef struct {
unsigned char channel_number;
unsigned char event_flags;
unsigned char cur_modem_stat;
unsigned char prev_modem_stat;
}PCXX_Event_Structure;

/* Command Structure */
typedef struct {
unsigned char command_function;
unsigned char channel_number;
unsigned char byte2;
unsigned char byte3;
}PCXX_Command_Structure;

/* command function pointer */
typedef int (*CMD_HND_PTR) (unsigned short,...);

#endif
