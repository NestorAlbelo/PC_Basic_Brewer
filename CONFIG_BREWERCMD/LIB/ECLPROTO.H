#ifndef ECLPROTO
#define ECLPROTO

#ifdef __cplusplus
extern "C" {
#endif


/* REVISIONS :
   1-24-89 JHL : Replaced all references to "struct _iobuf" with "FILE"
   4-17-89 JHL : Added function xmk_crc16()
	10-16-90 JHL : Added compatibility support for Turbo C++
*/
  
#include <stdio.h>

#include "i86.h"
#include "ym.h"
#include "xc_term.h"

/* Hayes Modem Command Functions */  
extern  int im_getn(int,char*);
extern  int im_put(int, char*);
extern  int im_putn(int,char*,int);
extern  int im_open(int);
extern  int im_fopen(int);
extern  int im_close(int);
extern  int im_fclose(int);
extern  int im_stat(int);
extern  int im_mode(int);
extern  int im_attn(int);
extern  int im_tt(int);
extern  int im_pulse(int);
extern  int im_orig(int);
extern  int im_gring(int);
extern  int im_carr(int,int);
extern  int im_hangu(int);
extern  int im_offhk(int);
extern  int im_fulld(int);
extern  int im_halfd(int);
extern  int im_spkr(int,int);
extern  int im_onlin(int);
extern  int im_commd(int);
extern  int im_reset(int);
extern  int im_sdel(int,char);
extern  int im_sesc(int,char);
extern  int im_dial(int,char*);
extern  int im_answ(int);
extern  int im_setmodem(int port,int type,char ic_delay);
extern  int im_send_command(int port,char *cmdbuf,int cmdlen,char ic_delay);
extern  int im_get_modstat(int port,char terminator,int timeout); 

/* Multiport comm functions */
extern  int xc_status_port(int,int);
extern  int xc_shared_vector(int,int);
extern  int xc_shared_icmask(int,int);
extern  int xc_board(int,int);
extern  int xc_startport(int,int);
extern  int xc_maxport(int,int);
extern  int xc_out2(int,int);
extern  int setast(int,int,int,int,int);
extern  int setdigi(int,int,int,int,int);
extern  int setdigimc(int,int,int,int,int);
extern  int setport(int,int,int,int,int);
extern  int setps2(int,int,int,int,int);
extern  int setstar(int,int,int,int,int);
/* Communications low level functions */
extern  int i86_in(int);
extern  int i86_int(unsigned char, union i86_reg*);
extern  int i86_out(int, unsigned char);
extern  int i86_peek(unsigned int, unsigned int);
extern  int i86_poke(unsigned int, unsigned int, unsigned char);
extern  int xc_up(int);
extern  int xc_down(void);
extern  int xc_brk(int,int);
extern  int xc_brkd(int);
extern  void xc_cport(int,int);
extern  int xc_cts(int);
extern  int xc_dcd(int);
extern  int xc_dsr(int);
extern  int xc_dtr(int,int);
extern  int xc_entr(int);
extern  int xc_exit(void);
extern  int xc_up(int);
extern  int xc_down(void);
extern  void xc_retv(void);
extern  int xc_findv(int,char*);
extern  int xc_flush(int);
extern  int xc_gaddr(int,unsigned int*,unsigned int*);
extern  int xc_get(int,char*,int*);
extern  int xc_fastr1(int,int);
extern  int xc_fastr2(int,char*);
extern  int xc_getc(int);
extern  int xc_gport(void);
extern  int xc_init(int,int,int,int,int);
extern  int xc_link(int,int);
extern  unsigned int xc_lstat(int);
extern  int xc_peek(int,char*,long*);
extern  int xc_put(int,char*,int*);
extern  int xc_putc(int,char);
extern  int xc_ri(int);
extern  int xc_rts(int,int);
extern  int xc_stat(int);
extern  long xc_test(int);
extern  int xc_timou(int,int);
extern  int xc_tport(int);
extern  int xc_uflush(int);
extern  int xc_unlk(int);
extern  int xc_uput(int,char*,int);
extern  int xc_upend1(int,int);
extern  int xc_upend2(int,char*);
extern  int xc_uputc(int,char);
extern  int xc_utest(int);
extern  int xc_uwait(int);
extern  int xc_vers(int*,int*,int*,int*);
/* File I/O */
extern  FILE *xfopen(char*,char*);            /*1-24-89 JHL*/
extern  int xfclose(FILE*);                   /*1-24-89 JHL*/
extern  int xfread(char*,int,int,FILE*);      /*1-24-89 JHL*/
extern  int xfwrite(char*,int,int,FILE*);     /*1-24-89 JHL*/
/* Keyboard */
extern  int xck_link(void);
extern  int xck_unlk(void);
extern  int xck_test(void);
extern  int xck_stat(void);
extern  unsigned int xck_getc(void);
extern  int xck_keyt(void);
extern  int xck_shft(void);
/* Generic timer */
extern  int xct_link(void);
extern  int xct_unlk(void);
extern  int xct_clr(void);
extern  int xct_pend(int);
extern  int xct_set(int);
extern  int xct_stat(void);
extern  int xct_test(void);
extern  int xct_wait(void);
/* Port Timers */
extern  int xctp_link(int);
extern  int xctp_unlk(int);
extern  int xctp_clr(int);
extern  int xctp_pend(int,int);
extern  int xctp_set(int,int);
extern  int xctp_stat(int);
extern  int xctp_test(int);
extern  int xctp_wait(int);
/* Video */
extern  int xcv_scur(int,int);
extern  int xcv_gcur(int*,int*);
extern  int xcv_scrl(int,int,int,int,int,int);
extern  int xcv_chat(char,int);
extern  int xcv_wtty(char);
/* flow control */
extern  int xc_fckill(int);
extern  int xc_fcoff(int);
extern  int xc_fcoff_rec(int);
extern  int xc_fcoff_tx(int);
extern  int _xc_fcoff(int,int);
extern  int xc_fcon(int,int,int);
extern  int xc_fcon_rec(int,int,int);
extern  int xc_fcon_tx(int);
extern  int _xc_fcon(int,int,int,int);
extern  void xc_fc_char(int,int);
/* XMODEM Protocol */
extern  void xmk_pf(char*,int,int,int,int);
extern  void _print_status(int,char*);
extern  int xmk_open(int,int,int,int,int,int);
extern  int xmk_close(int);
extern  int xmk_stat(int,int*,int*,int*,int*);
extern  int xmk_can(int);
extern  int xmk_eot(int, char*);
extern  int xmk_pcvt(int,char*,char*);
extern  int xmk_put(int,char*,int);
extern  int xmk_testch(int);
extern  int xmk_waitch(int,char*,int);
extern  int xmk_bputf(int,char*,int,int);
extern  int xmk_cansw(int);
extern  int xmk_answ(int,char);
extern  int xmk_testbk(int);
extern  int xmk_getbk(int,char*);
extern  int xmk_bget(int,char*);
extern  int xmk_bgetf(int,char*,int,int);
extern  int xmk_get(int,char*);
extern  int xmk_putf(int,char*,char*,int,int,int);
extern  int xmk_getf(int,char*,char*,int,int,int);
extern  int xmk_crc16(char*,int);
extern  int xmk_xsum(char*,int);
/* YMODEM Batch Protocol functions */
extern  int ym_bld_xfer_table(int, char*);
extern  int ym_open(int,int,int,int,int);
extern  int ym_close(int,int);
extern  int ym_stat(int,char*,char*,int*,int*,int*,int*,int*);
extern  void ym_print_stat(int);
extern  int ym_get_frame0(int);
extern  int ym_bgetf(int,char*);
extern  int ym_getf(int,char*,int,int);
extern  int ym_block0(YM_FILE_DESC*,char*);
extern  int ym_frame(char*,int,int,char*);
extern  int ym_testch(int);
extern  int ym_bputf(int,char*);
extern  int ym_putf(int,char*,int,int);
/* YMODEM-g Protocol */
extern  int ymgb_bgetf(int,char*);
extern  int ymgb_bputf(int,char*);
extern  int ymgb_getf(int,char*,int,int);
extern  int ymgb_putf(int,char*,int,int);
extern  int ymg_bgetf(int,char*);
extern  int ymg_bputf(int,char*);
extern  int ymg_getf(int,char*,int,int);
extern  int ymg_putf(int,char*,int,int);
extern  void ymg_print_stat(int);
/* Kermit Protocol */
extern  int km_1byte_xsum(char*,int);
extern  int km_bgetf(int,char*);
extern  int km_bputf(int,char*);
extern  int km_getf(int,char*,int,int);
extern  int km_putf(int,char*,int,int);
extern  void km_print_stat(int);
extern  int km_stat(int,char*,char*,int*,char*,unsigned long*,int*,int*,int*,int*);

/* ZMODEM protocol */
extern  int zm_getf( int port, char *path, int print_stat, int abort_flag );
extern  int zm_setopt( int option, ... );
extern  int zm_putf( int port, char *path, int print_stat, int abort_flag );

/* NS16550A support */
extern  int xc_t16550(int port_number);
extern  int set_16550(int port_number,int mode,int trigger);

/* terminal emulation */
extern int xc_terminal(TERM_PTR term_ptr);

extern int xcterm_ansi(int port, int top_row, int bot_row, int return_key,
                       char abort_flag, XCharInfo *key_info_ptr,TERM_FPTR idle_time_funct);

extern int xcterm_vt52(int port, int top_row, int bot_row, int return_key,
                       char abort_flag, XCharInfo *key_info_ptr,TERM_FPTR idle_time_funct);

extern int xcterm_vt100(int port, int top_row, int bot_row, int return_key,
                       char abort_flag, XCharInfo *key_info_ptr,TERM_FPTR idle_time_funct);
extern int vt100_rec_f1(TERM_PTR term_ptr);
extern int vt100_rec_f2(TERM_PTR term_ptr);
extern int vt100_rec_f3(TERM_PTR term_ptr);
extern int vt100_rec_f4(TERM_PTR term_ptr);
extern int vt100_rec_f5(TERM_PTR term_ptr);
extern int vt100_rec_f6(TERM_PTR term_ptr);
extern int vt100_rec_f7(TERM_PTR term_ptr);
extern int vt100_rec_f8(TERM_PTR term_ptr);
extern int vt100_rec_f9(TERM_PTR term_ptr);
extern int vt100_rec_f10(TERM_PTR term_ptr);

extern int xc_term_scan_base_array(TERM_PTR term_ptr);
extern int xc_term_curfwd(TERM_PTR term_ptr);
extern int xc_term_curup(TERM_PTR term_ptr);
extern int xc_term_curdown(TERM_PTR term_ptr);
extern int xc_term_curleft(TERM_PTR term_ptr);
extern int xc_term_curhome(TERM_PTR term_ptr);
extern int xc_term_scrollup(TERM_PTR term_ptr);
extern int xc_term_scrolldwn(TERM_PTR term_ptr);
extern int xc_term_lineinsert(TERM_PTR term_ptr);
extern int xc_term_il(TERM_PTR term_ptr);
extern int xc_term_linedelete(TERM_PTR term_ptr);
extern int xc_term_dl(TERM_PTR term_ptr);
extern int xc_term_clreod(TERM_PTR term_ptr);
extern int xc_term_clreol(TERM_PTR term_ptr);
extern int xc_term_el1(TERM_PTR term_ptr);
extern int xc_term_del_char(TERM_PTR term_ptr);
extern int xc_term_ins_char(TERM_PTR term_ptr);
extern int xc_term_ins_chars(TERM_PTR term_ptr);
extern int xc_term_setvid(TERM_PTR term_ptr);
extern int xc_term_clearscreen(TERM_PTR term_ptr);
extern int xc_term_wtty(TERM_PTR term_ptr);
extern int xc_term_echo(TERM_PTR term_ptr);
extern int xc_term_linefeed(TERM_PTR term_ptr);
extern int xc_term_tab(TERM_PTR term_ptr);
extern int xc_term_bt(TERM_PTR term_ptr);
extern int xc_term_xkeysequence(TERM_PTR term_ptr);
extern int xc_term_init_num0(TERM_PTR term_ptr);
extern int xc_term_init_nums(TERM_PTR term_ptr);
extern int xc_term_cuu(TERM_PTR term_ptr);
extern int xc_term_cud(TERM_PTR term_ptr);
extern int xc_term_cuf(TERM_PTR term_ptr);
extern int xc_term_cub(TERM_PTR term_ptr);
extern int xc_term_hpa(TERM_PTR term_ptr);
extern int xc_term_vpa(TERM_PTR term_ptr);

/* Intelligent DigiBoard support */
extern int   pcxx_command_handler(unsigned short funct_num,...);
extern int   pcxx_init(void);
extern int   set_digi_pcxx(int io_port,int irq,int starting_port,long int dram_address,int multi); 
extern int   xc_buffer_gmem(int port,int buffer_seq);
extern void  xc_buffer_fmem(int port);
extern void  xc_buffer_pmem(int rx_seg,int rx_off);
extern int   xc_recbuf(int port,char *bufptr,int *cntptr);
extern int   pcxx_int_handler(void);
extern int   initisrdigipcxx(int int_no,int (*func)(),int chain);
extern int   freeisrdigipcxx(void);

/* device handler support */
extern void xc_set_int_exit(int handler, int mask, int (far *func)());


#ifdef __cplusplus
}
#endif

#endif
