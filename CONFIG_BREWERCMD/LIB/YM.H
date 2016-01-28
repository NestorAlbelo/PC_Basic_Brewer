#ifndef YM_H
#define YM_H
  
#define SOH_BUF_LEN            128
#define STX_BUF_LEN            1024
#define MAX_BUF_LEN            1024
#define SOH_FRAME_LEN          133
#define STX_FRAME_LEN          1029
#define SOH_FR_LEN_MINUS_ONE   132
#define STX_FR_LEN_MINUS_ONE   1028
#define MAX_FRAME_LEN          1029
  
  
#define SPACE   0X20
#define BS      0X08
  
/* YMODEM file xfer types */
#define    YM_REC                 0
#define    YM_SEND                1
  
/* MACRO EXPANSION FOR AND ALLOWABLE VALUES OF YM_ABORT_FLAG(port_no) */
#define YM_ABORT_FLAG(port_no)      xmk_abort_flag[port_no]
#define   YM_NO_ABORT    0   /* no Control-Break or Data Carrier Detect enabled */
#define   YM_CB          1   /* Only Control-Break enabled */
#define   YM_DCD         2   /* Only Data Carrier Detect enabled */
#define   YM_CB_AND_DCD  3   /* Control-Break and Data Carrier Detect enabled */
  
/* YMODEM file xfer status values */
#define    YM_COMPL             1
#define    YM_OK                0
#define    YM_SESS             -1
#define    YM_NAK              -2
#define    YM_CANC             -3
#define    YM_ABORT            -4
#define    YM_FIO              -6
#define    YM_SILENCE          -7
#define    YM_NO_DCD           -8
#define    YM_OUT_OF_MEM       -9
#define    YM_INVALID_FRAME    -10
  
/* Times to send out 'C's before sending out NAKs */
#define    YM_C_RETRIES           5
  
/* Times to wait for 'C's or NAKs before canceling session */

#define    YM_WAIT_RETRIES        7
  
/* YMODEM file xfer states : allowable values for ym_xfer_state[comm port] */
#define    YM_NOT_IN_PROGRESS     0
                                      /* YMODEM BATCH */
#define    YM_WAIT_C_NAK          1   /* Wait for initial C or NAK to begin batch session */
#define    YM_FORM_FR0            2   /* create frame 0 */
#define    YM_SND_FR0             3   /* send frame 0 */
#define    YM_SET_TIMER           4   /* wait for frame 0 to be sent, then set timer */
#define    YM_WAIT_FR0_ACK        5   /* wait for ACK of frame 0 */
#define    YM_SND_XMK             6   /* send file via XMODEM services */
#define    YM_SEND_C              7   /* send initial C to begin session */
#define    YM_SEND_NAK            8   /* send initial NAK to begin session */
#define    YM_REC_XMK             9   /* receive file via XMODEM services */
                                      /* YMODEM G */
#define    YM_G_WAIT_G           10   /* Wait for initial G to begin batch session */
#define    YM_G_SND_F0           11   /* send frame 0 */
#define    YM_G_SET_TIMER        12   /* wait for frame 0 to be sent, then set timer */
#define    YM_G_WAIT_F0_G        13   /* wait for G response to frame 0 */
#define    YM_G_SND_OPEN_FILE    14   /* open file to xfer */
#define    YM_G_SND_FRAME        15   /* sent one frame of file data */
#define    YM_G_WAIT_FRAME_SENT  16   /* wait for frame to be sent before sending next frame */
#define    YM_G_SND_EOT          17   /* all frames sent, now send EOT */
#define    YM_G_REC_EOT_ACK      18   /* receive ACK of sent EOT */
#define    YM_G_WAIT_NULL_F0_ACK 19   /* wait for ACK of sent null frame 0 */
#define    YM_G_SND_FILE_COMPL   20   /* file send completed */
#define    YM_G_SND_G            21   /* send initial G to begin batch session */
#define    YM_G_WAIT_F0          22   /* wait for frame 0 */
#define    YM_G_SND_F0_G         23   /* send G in response to good frame 0 */
#define    YM_G_REC_OPEN_FILE    24   /* open file to receive */
#define    YM_G_REC_FILE         25   /* receive file */
#define    YM_G_SND_EOT_ACK      26   /* send ACK in reponse to EOT */
#define    YM_G_REC_FILE_COMPL   27   /* receive file completed */
  
/* YMODEM file description structure */
  
typedef struct {
    char filename[13];     /* file to be xfered */
    /* THE FOLLOWING FEILDS ARE OPTIONAL AND CAN BE SET AS A NULL STRING */
    char file_length[10];  /* decimal string. if unknown, =0 or null  */
    unsigned long int i_filelength; /* integer representation of file length */
    char mod_date[40];     /* octal string: # of sec. from 1/1/80 */
                          /* 0 = unknown and current date used   */
    char mode[10];     /* octal string; =0 for non unix systems */
    char ser_no[10];   /* octal string; if unknown, =0 or null */
} YM_FILE_DESC;
  
  
typedef struct {
    int   no_of_files;  /* number of files in array of file info. */
    int   current_file; /* current file number being sent */
    char   file_cnt;     /* count of files sent */
    char   path[64];     /* path of files to xfer */
    char   filespec[77];  /* path + filename */
    unsigned long int   bytes_xfered; /* no. of bytes currently xfered */
    YM_FILE_DESC   *ym_file_info; /* pointer to file information */
} YM_FILE_TABLE;
  
  
/************* MACRO EXPANSIONS FOR ym_file_table_ptr[] ******************/
#define  YM_FILES(comm_port)              ym_file_table_ptr[comm_port]->no_of_files
#define  YM_CUR_FILE(comm_port)           ym_file_table_ptr[comm_port]->current_file
#define  YM_FILE_CNT(comm_port)           ym_file_table_ptr[comm_port]->file_cnt
#define  YM_PATH(comm_port)               ym_file_table_ptr[comm_port]->path
#define  YM_FILESPEC(comm_port)           ym_file_table_ptr[comm_port]->filespec
#define  YM_BYTES_XFERED(comm_port)       (ym_file_table_ptr[comm_port])->bytes_xfered
#define  YM_FILE_DESC_PTR(comm_port)      ym_file_table_ptr[comm_port]->ym_file_info
#define  YM_CUR_FILE_PTR(comm_port)       (YM_FILE_DESC*)(YM_FILE_DESC_PTR(comm_port)+YM_CUR_FILE(comm_port))
#define  YM_CUR_FILE_INFO(comm_port)      (ym_file_table_ptr[comm_port])->ym_file_info[YM_CUR_FILE(comm_port)]
#define  YM_CUR_FILENAME(comm_port)       YM_CUR_FILE_INFO(comm_port).filename
#define  YM_CUR_FILELENGTH(comm_port)     YM_CUR_FILE_INFO(comm_port).file_length
#define  YM_CUR_I_FILELENGTH(comm_port)   YM_CUR_FILE_INFO(comm_port).i_filelength
#define  YM_CUR_FMOD_DATE(comm_port)      YM_CUR_FILE_INFO(comm_port).mod_date
#define  YM_CUR_FILE_MODE(comm_port)      YM_CUR_FILE_INFO(comm_port).mode
#define  YM_CUR_FILE_SN(comm_port)        YM_CUR_FILE_INFO(comm_port).ser_no
  
  
/* MACRO EXPANSIONS USED IN ym_bld_xfer_table() FOR COMPILER PORTABILITY */
#ifdef __TURBOC__
#define YM_FIND_FIRST(path,buf_ptr)     findfirst(path,buf_ptr,0)
#define YM_FIND_NEXT(buf_ptr)           findnext(buf_ptr)
#define YM_FILE_NAME                    fib.ff_name
#define YM_FILE_LENGTH                  fib.ff_fsize
#define YM_FILE_DATE                    fib.ff_fdate
#endif
  
#ifdef M_I86
#define YM_FIND_FIRST(path,buf_ptr)     _dos_findfirst(path,_A_NORMAL,buf_ptr)
#define YM_FIND_NEXT(buf_ptr)           _dos_findnext(buf_ptr)
#define YM_FILE_NAME                    fib.name
#define YM_FILE_LENGTH                  fib.size
#define YM_FILE_DATE                    fib.wr_date
#endif
/********/
  
#endif /* YM_H */
