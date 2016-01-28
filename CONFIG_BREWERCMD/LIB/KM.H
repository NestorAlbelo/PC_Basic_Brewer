#ifndef KM_H
#define KM_H
  
#include <stdio.h>
  
/* Kermit file transfer states : allowable values for km_xfer_state[comm port] */
#define KM_NOT_IN_PROGRESS      0
                                  /* Kermit Send File */
#define KM_FORM_S_PKT           1   /* Form Send Init packet */
#define KM_WAIT_S_PKT_ACK       2   /* Wait for S packet ACK */
#define KM_FORM_F_PKT           3   /* Form Frame Header packet */
#define KM_WAIT_F_PKT_ACK       4   /* Wait for F packet ACK */
#define KM_OPEN_SEND_FILE       5   /* Open file to xfer */
#define KM_FORM_D_PKT           6   /* Form Data packet */
#define KM_WAIT_D_PKT_ACK       7   /* Wait for D packet ACK */
#define KM_FORM_Z_PKT           8   /* Form End of File (Z) packet */
#define KM_WAIT_Z_PKT_ACK       9   /* Wait for Z packet ACK */
#define KM_FORM_B_PKT          10   /* Form B packet */
#define KM_WAIT_B_PKT_ACK      11   /* Wait for B packet ACK */
                                  /* Kermit Receive File */
#define KM_REC_WAIT_S_PKT      12   /* Wait for Send Init (S) packet */
#define KM_FORM_ACK_S_PKT      13   /* Form ACK Send Init (Y) packet */
#define KM_FORM_ACK_PKT        14   /* Form ACK (Y) packet */
#define KM_FORM_NAK_PKT        15   /* Form NAK (N) packet */
#define KM_REC_WAIT_F_PKT      16   /* Wait for Frame Header (F) packet or Break (B) packet */
#define KM_OPEN_REC_FILE       17   /* Open file to receive */
#define KM_REC_WAIT_D_PKT      18   /* Wait for Data (D) Packet or End of File (Z) Packet */
#define KM_SENT_ACK_B_PKT      19   /* B packet sent, now close session */
                                 /* Kermit Receive and Send File */
#define KM_SND_PKT             20   /* Send packet */
#define KM_SET_TIMER           21   /* Wait for packet to be sent, then set timer */
#define KM_FORM_E_PKT          22   /* Form Error (E) packet */
#define KM_ABORT_XFER          23   /* Abort file transfer session */
  
  
/* Allowable values to KM_REC_PKT_STATE(port_no) */
#define KM_REC_MARK             0   /* Receive packet MARK field */
#define KM_REC_LEN              1   /* Receive packet LEN field */
#define KM_REC_SEQ              2   /* Receive packet SEQ field */
#define KM_REC_TYPE             3   /* Receive packet TYPE field */
#define KM_REC_DATA_AND_CHKT    4   /* Receive DATA and CHECK fields */
  
/* Recognizable Kermit Packet Types */
#define KM_ACK_PKT            'Y'   /* ACKNOWLEDGE PACKET */
#define KM_NAK_PKT            'N'   /* NAK PACKET */
#define KM_DATA_PKT           'D'   /* DATA PACKET */
#define KM_SEND_INIT_PKT      'S'   /* SEND INIT PACKET */
#define KM_BREAK_PKT          'B'   /* BREAK PACKET */
#define KM_FILE_HDR_PKT       'F'   /* FILE HEADER PACKET */
#define KM_EOF_PKT            'Z'   /* END OF FILE PACKET */
#define KM_ERROR_PKT          'E'   /* ERROR PACKET */
  
/* Allowable values for CHKT */
#define KM_1BYTE_XSUM      '1'  /* ONE BYTE CHECKSUM ERROR CHECK */
  
/* Kermit protocol design constraints */
#define KM_MAX_PKT_LEN      96  /* max length of packet */
#define KM_MAX_LEN          94  /* max value of LEN */
#define KM_MAX_DATA_LEN     93  /* max length of data info */
#define KM_MARK           0X01  /* SOH */
  
/* Kermit default file transfer constraining parameters */
#define KM_DEFAULT_MAXL     90    /* Default max length of packet */
#define KM_DEFAULT_TIME     10    /* Default timeout value (in seconds) */
#define KM_DEFAULT_NPAD      0    /* Default number of padding characters */
#define KM_DEFAULT_PADC      0    /* Default control character used for padding */
#define KM_DEFAULT_EOL      13    /* Default character used to terminate an incomming packet */
#define KM_DEFAULT_QCTL     '#'   /* Default printable character I will us to quote control characters */
#define KM_DEFAULT_QBIN     'Y'   /* I agree to 8 bit quoting but I don't need it */
#define KM_7BIT_QBIN        '&'   /* (0X21<=QBIN<=0X3E)||(0X60<=QBIN<=0X7E) */
#define KM_DEFAULT_CHKT     '1'   /* Default error check type */
#define KM_DEFAULT_REPT     ' '   /* No repeat count used */
#define KM_DEFAULT_CAPASA    0    /* Default capability bit mask */
#define KM_DEFAULT_RETRIES   9    /* Default retry count */
  
#define KM_FIO_BUF_SIZE    1024   /* file I/O information buffer */
  
/* Allowable values for KM_ABORT_FLAG */
#define KM_NO_ABORT          0   /* Cntrl-Break not enabled, DCD not enabled */
#define KM_CB                1   /* Only Control-break enabled */
#define KM_DCD               2   /* Only Data Carrier Detect enabled */
#define KM_CB_AND_DCD        3   /* Control-break and Data Carrier Detect enabled */
  
/* Kermit function return status values */
#define KM_COMPL             1
#define KM_OK                0
#define KM_SESS             -1
#define KM_NAK              -2
#define KM_ABORT            -4
#define KM_FIO              -6
#define KM_SILENCE          -7
#define KM_OUT_OF_MEM       -8
#define KM_BAD_CHKT         -9
#define KM_NO_DCD           -10
#define KM_EOF              -11
#define KM_INVAL_PKT        -12
#define KM_REC_EPKT         -13
#define KM_REC_BPKT         -14
#define KM_REC_SPKT         -15
#define KM_REC_FPKT         -16
#define KM_REC_PREV_DPKT    -17
#define KM_REC_DPKT         -18
#define KM_NO_XMIT_RESIZE   -19
#define KM_UNMATCHED_QBIN   -20
#define KM_NO_8TH_PREFIX    -21  
  
/* Kermit File Transfer Data Structures */
  
/* structure to hold info pertaining to each file in xfer */
typedef struct  {
    char    filename[13];      /* file to be xfered */
    char    file_length[10];   /* decimal string */
    unsigned int    mod_date;          /* modification date */
    unsigned int    file_time;         /* time file was created */
} KM_FILE_DESC;
  
/* structure to hold info for each xfer session on a particular comm port */
typedef struct  {
    char   path[64];         /* path to files */
    char   filespec[77];     /* path + filename of current file */
    int   no_of_files;      /* no. of files to be xfered */
    int   file_cnt;         /* cnt of files xfered (start w/ 1) */
    int   current_file;     /* current file index in file xfer array */
    int   next_xfer_state;  /* next xfer state */
    unsigned long int   bytes_xfered;     /* number of bytes currently xfered */
    int   prev_seq;         /* previous pkt SEQ no. */
    int   seq;              /* current pkt SEQ no. */
    int   next_seq;         /* next pkt SEQ no. */
    int   retries;          /* current no. of retries */
    int   timeout;          /* timeout value (clock ticks) */
    char   abort_flag;       /* cntl-break enable flag */
    char   print_stat;       /* print xfer status flag */
    int   acked;            /* no. ACKs received */
    int   naked;            /* no. NAKs received */
    int   timed;            /* no. timeouts received */
    int   junked;           /* no. junk info. received */
    char   xfer_msg[60];     /* message to copy into E pkt */
    int   abort_err;        /* error which caused session to abort */
    char   *xmit_buffer_ptr; /* padding chars + xmit packet + EOL */
    int   xmit_cnt;         /* cnt of bytes in xmit buf, for cur pkt */
    char   pkt_type;         /* current pkt type to xmit */
    int   data_len;         /* no. of bytes in data field of xmit pkt */
    char   *rec_buffer_ptr;  /* received packet + EOL */
    int   rec_pkt_state;    /* receive packet xfer states */
    int   expected;         /* no. of bytes expected */
    FILE   *file_ptr;        /* pointer to current file xfered */
    char   *fio_buf_ptr;     /* buffer between xfer file and xfer session */
    int   fio_buf_cnt;      /* current position in fio_buf_ptr */
    unsigned char   maxl;             /* max length of packet to send */
    unsigned char   time;             /* timeout value in seconds */
    unsigned char   npad;             /* no. of padding characters */
    unsigned char   padc;             /* paddind character */
    unsigned char   eol;              /* EOL character */
    unsigned char   qctl;             /* quote cntrl char */
    unsigned char   qbin;             /* 8th bit set quote char */
    unsigned char   chkt;             /* check type */
    unsigned char   chkt_len;         /* no. of bytes in CHKT */
    unsigned char   rept;             /* repeate char */
    unsigned char   capasa;           /* capability bit mask */
    unsigned char   windo;            /* window size */
    unsigned char   maxlx1;           /* extended packet length */
    unsigned char   maxlx2;           /* extended packet length */
    KM_FILE_DESC   *km_file_info_ptr;/* ptr to array of files to xfer */
}  KM_FILE_TABLE;
  
/************* MACRO EXPANSIONS FOR km_file_table_ptr[] *********************/
/* general info pertaining with entire file xfer session */
#define  KM_PATH(comm_port)            km_file_table_ptr[comm_port]->path
#define  KM_FILESPEC(comm_port)        km_file_table_ptr[comm_port]->filespec
#define  KM_FILES(comm_port)           km_file_table_ptr[comm_port]->no_of_files
#define  KM_FILE_CNT(comm_port)        km_file_table_ptr[comm_port]->file_cnt
#define  KM_CUR_FILE(comm_port)        km_file_table_ptr[comm_port]->current_file
#define  KM_NEXT_XFER_STATE(comm_port) km_file_table_ptr[comm_port]->next_xfer_state
#define  KM_BYTES_XFERED(comm_port)    km_file_table_ptr[comm_port]->bytes_xfered
#define  KM_PREV_SEQ(comm_port)        km_file_table_ptr[comm_port]->prev_seq
#define  KM_SEQ(comm_port)             km_file_table_ptr[comm_port]->seq
#define  KM_NEXT_SEQ(comm_port)        km_file_table_ptr[comm_port]->next_seq
#define  KM_RETRIES(comm_port)         km_file_table_ptr[comm_port]->retries
#define  KM_TIMEOUT(comm_port)         km_file_table_ptr[comm_port]->timeout
#define  KM_ABORT_FLAG(comm_port)      km_file_table_ptr[comm_port]->abort_flag
#define  KM_PRINT_STAT(comm_port)      km_file_table_ptr[comm_port]->print_stat
#define  KM_ACKED(comm_port)           km_file_table_ptr[comm_port]->acked
#define  KM_NAKED(comm_port)           km_file_table_ptr[comm_port]->naked
#define  KM_TIMED(comm_port)           km_file_table_ptr[comm_port]->timed
#define  KM_JUNKED(comm_port)          km_file_table_ptr[comm_port]->junked
#define  KM_XFER_MSG(comm_port)        km_file_table_ptr[comm_port]->xfer_msg
#define  KM_ABORT_ERR(comm_port)       km_file_table_ptr[comm_port]->abort_err
  
/* pointer to xmit buffer */
#define  KM_XMIT_BUF_PTR(comm_port)    km_file_table_ptr[comm_port]->xmit_buffer_ptr
#define  KM_XMIT_CNT(comm_port)        km_file_table_ptr[comm_port]->xmit_cnt
#define  KM_PKT_TYPE(comm_port)        km_file_table_ptr[comm_port]->pkt_type
#define  KM_DATA_LEN(comm_port)        km_file_table_ptr[comm_port]->data_len
/* index to starting position of data in xmit buffer */
#define  KM_XDS(comm_port)             (KM_NPAD(comm_port) + 4)
/* pointer to staring position of data in xmit buffer */
#define  KM_XMIT_PKT_DATA(comm_port)   (&(km_file_table_ptr[comm_port]->xmit_buffer_ptr[KM_XDS(comm_port)]))
  
/* pointer to receive buffer: receive buffer always begins with MARK, padding discarded */
#define  KM_REC_BUF_PTR(comm_port)     km_file_table_ptr[comm_port]->rec_buffer_ptr
#define  KM_REC_PKT_STATE(comm_port)   km_file_table_ptr[comm_port]->rec_pkt_state
#define  KM_EXPECTED(comm_port)        km_file_table_ptr[comm_port]->expected
#define  KM_REC_PKT_MARK(comm_port)    km_file_table_ptr[comm_port]->rec_buffer_ptr[0]
#define  KM_REC_PKT_LEN(comm_port)     km_file_table_ptr[comm_port]->rec_buffer_ptr[1]
#define  KM_REC_PKT_SEQ(comm_port)     km_file_table_ptr[comm_port]->rec_buffer_ptr[2]
#define  KM_REC_PKT_TYPE(comm_port)    km_file_table_ptr[comm_port]->rec_buffer_ptr[3]
#define  KM_REC_PKT_DATA(comm_port)    (&(km_file_table_ptr[comm_port]->rec_buffer_ptr[4]))
  
/* pointer to xfer file and file I/O buffer */
#define  KM_FILE_PTR(comm_port)        km_file_table_ptr[comm_port]->file_ptr
#define  KM_FIO_BUF_PTR(comm_port)     km_file_table_ptr[comm_port]->fio_buf_ptr
#define  KM_FIO_BUF_CNT(comm_port)     km_file_table_ptr[comm_port]->fio_buf_cnt
  
/* info from & for SEND-INIT packets */
#define  KM_MAXL(comm_port)            km_file_table_ptr[comm_port]->maxl
#define  KM_TIME(comm_port)            km_file_table_ptr[comm_port]->time
#define  KM_NPAD(comm_port)            km_file_table_ptr[comm_port]->npad
#define  KM_PADC(comm_port)            km_file_table_ptr[comm_port]->padc
#define  KM_EOL(comm_port)             km_file_table_ptr[comm_port]->eol
#define  KM_QCTL(comm_port)            km_file_table_ptr[comm_port]->qctl
#define  KM_QBIN(comm_port)            km_file_table_ptr[comm_port]->qbin
#define  KM_CHKT(comm_port)            km_file_table_ptr[comm_port]->chkt
#define  KM_CHKT_LEN(comm_port)        km_file_table_ptr[comm_port]->chkt_len
#define  KM_REPT(comm_port)            km_file_table_ptr[comm_port]->rept
#define  KM_CAPASA(comm_port)          km_file_table_ptr[comm_port]->capasa
#define  KM_WINDO(comm_port)           km_file_table_ptr[comm_port]->windo
#define  KM_MAXLX1(comm_port)          km_file_table_ptr[comm_port]->maxlx1
#define  KM_MAXLX2(comm_port)          km_file_table_ptr[comm_port]->maxlx2
/* pointer to file info structure */
#define  KM_FILE_DESC_PTR(comm_port)   km_file_table_ptr[comm_port]->km_file_info_ptr
  
/* Macro expansions for current file (being xferred) information */
#define  KM_CUR_FILE_PTR(comm_port)    (KM_FILE_DESC*)(KM_FILE_DESC_PTR(comm_port)+KM_CUR_FILE(comm_port))
#define  KM_CUR_FILE_INFO(comm_port)   km_file_table_ptr[comm_port]->km_file_info_ptr[KM_CUR_FILE(comm_port)]
#define  KM_CUR_FILENAME(comm_port)    KM_CUR_FILE_INFO(comm_port).filename
#define  KM_CUR_FILELENGTH(comm_port)  KM_CUR_FILE_INFO(comm_port).file_length
#define  KM_CUR_FMOD_DATE(comm_port)   KM_CUR_FILE_INFO(comm_port).mod_date
#define  KM_CUR_FMOD_TIME(comm_port)   KM_CUR_FILE_INFO(comm_port).file_time
  
/* MACRO EXPANSIONS USED IN km_bld_xfer_table() FOR COMPILER PORTABILITY */
#ifdef __TURBOC__
#define KM_FIND_FIRST(path,buf_ptr)    findfirst(path,buf_ptr,0)
#define KM_FIND_NEXT(buf_ptr)          findnext(buf_ptr)
#define KM_FILE_NAME                   fib.ff_name
#define KM_FILE_LENGTH                 fib.ff_fsize
#define KM_FILE_DATE                   fib.ff_fdate
#define KM_FILE_TIME                   fib.ff_ftime
#endif
  
#ifdef M_I86
#define KM_FIND_FIRST(path,buf_ptr)     _dos_findfirst(path,_A_NORMAL,buf_ptr)
#define KM_FIND_NEXT(buf_ptr)           _dos_findnext(buf_ptr)
#define KM_FILE_NAME                    fib.name
#define KM_FILE_LENGTH                  fib.size
#define KM_FILE_DATE                    fib.wr_date
#define KM_FILE_TIME                    fib.wr_time
#endif
  
/***************************************************************************/
#endif /* KM_H */
