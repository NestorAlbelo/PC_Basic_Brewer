#ifndef  ECL_IMHEAD
#define  ECL_IMHEAD
							  
/* IM.h    Include file for IM.C module */
  
/* Status returns from IMODEM functions */

#define IM_RSP_EXCEED      -13     /* Maximum number of modem responses
                                      for one command has been exceeded
                                      Maximum = IM_MAX_NRESP */
#define IM_MAXCRSP_EXCEED  -12     /* Maximum number of response charaters
                                      for one command has been exceeded
                                      Maximum = IM_MAX_RESP_CHARS */
#define IM_HW_TIMEOUT      -11     /* Timeout due to hardware handshaking  */
#define IM_TIMEOUT         -10     /* Timeout waiting for modem response */
#define IM_RESP            -9      /* unrecognized modem response */
#define IM_RING            -8      /* Ring indication */
#define IM_NO_ANSWER       -7      /* No silence while dialing */
#define IM_BUSY            -6      /* Engaged (busy) signal detected */
#define IM_NO_DIALTONE     -5      /* No Dialtone detected */
#define IM_INVALTYPE       -4      /* Invalid modem type */
#define IM_NOCAR           -3      /* No carrier */
#define IM_SESS            -2      /* No session in progress */
#define IM_ERROR           -1      /* Invalid command, Command line length
                                      exceeded */
#define IM_OK               0      /* Success - acknowledgement */
#define IM_CONCT            1      /* Connected */
  
  
#define IM_SPKR_OFF         0       /* IM_Speaker speaker off */
#define IM_SPKR_ONC         1       /* IM_Speaker - on till connect */
#define IM_SPKR_ON          2       /* IM_Speaker speaker on always */

/* modem operating mode values */  
#define IM_ONLINE          0           /* MODE - Connected to other CPU */
#define IM_COMMAND         1           /* MODE - modem commands allowed */

/* old ECL ver 3.0- defines */
#define SPKR_OFF         0       /* IM_Speaker speaker off */
#define SPKR_ONC         1       /* IM_Speaker - on till connect */
#define SPKR_ON          2       /* IM_Speaker speaker on always */
#define ON_LINE         0           /* MODE - Connected to other CPU */
#define COMMAND         1           /* MODE - modem commands allowed */

/* Modem default operating values */  
#define IM_DTERM           '\015'  /* Default terminator character */
#define IM_DESC            '+'     /* Escape back to command mode */
#define IM_DESC_GUARD      25      /* Exceed 1 second */
#define IM_D_ICDELAY        2      /* Default Intercharacter Delay (in clock ticks) */
#define IM_NORM_TO         40      /* Normal command timeout ( 2.2 sec) */
#define IM_DIAL_TO       1080      /* Dialing command timeout (60 sec) */
  
  
/* maximum number of characters comprising any modem numrical response
   (includes terminator character) */
#define IM_MAX_RESP_CHARS     5
  
/* Modem types */
#define IM_HAYES_ULTRA      0
#define IM_USR_COURIER      1
#define IM_MICROCOM_QX      2
#define IM_UDS_FASTALK      2
#define IM_TELEBIT          3
#define IM_MULTITECH        4
#define IM_GVC              5
#define IM_VENTEL           6
#define IM_DIGICOM_SYSTEMS  7
#define IM_GENERIC_TYPE     8

#define IM_STANDARD_TYPE    9    /* Always equal to IM_MODEMTYPE_MAX */
#define IM_MODEMTYPE_MAX    9


/* Modem structure */
typedef struct ecl_modem  {
    char  in_progress;    /* session in progress on port: 0=no; 1=yes */
    char  imodem_mode;    /* IM_ONLINE or IM_COMMAND modes */
    char  terminator;     /* Terminator character */
    char  escape;         /* Char to Escape back to command mode */
    char  ic_delay;       /* intercharacter delay for sending comands */
    int   escape_guard;   /* Used to enter COMMAND mode (Exceed 1 second) */
    int   timeout;        /* command response timeout */
    int   im_baud;        /* baud rate set by im_put, im_answ & im_dial */
	 int   modem_type;     /* determines: additional initialization string
                             and filtering of modem responses.  */
}ECL_MODEM;
  
  
/* Intellegent Modem global variables. Defined in "im.c" */
extern int im_baud;
extern ECL_MODEM     im_modem[];
  
#endif
