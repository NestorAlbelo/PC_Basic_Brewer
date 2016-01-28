#ifndef  ECL_I86
#define  ECL_I86


/*
    I86.h     Intel 86/88 Interrupt interface
    Software Interrupt interface:
  
    Example (ring the bell BIOS call)
  
    #include "i86.h"
    struct i86_reg packet;
  
    main()
    {
        int flags;
        packet.breg.AH = 0x09;
        packet.breg.AL = 0x07;
        packet.breg.BL = 0x01;
        packet.breg.BH = 0x00;
        packet.wreg.CX = 50  ;
  
        flags = i86_int(0x10, packet);
    }
*/
  
union i86_reg {
  
    struct  {                             /*  15     8 7      0  */
                                        /* +--------+--------+ */
        int    AX;                 /* |       A X       | */
                                        /* |--------+--------| */
        int    BX;                 /* |       B X       | */
                                        /* |--------+--------| */
        int    CX;                 /* |       C X       | */
                                        /* |--------+--------| */
        int    DX;                 /* |       D X       | */
    } wreg;                               /* +--------+--------+ */
  
    struct  {                             /*  15     8 7      0  */
                                        /* +--------+--------+ */
        char   AL;                 /* |   AH   |   AL   | */
        char   AH;                 /* |--------+--------| */
        char   BL;                 /* |   BH   |   BL   | */
        char   BH;                 /* |--------+--------| */
        char   CL;                 /* |   CH   |   CL   | */
        char   CH;                 /* |--------+--------| */
        char   DL;                 /* |   DH   |   DL   | */
        char   DH;                 /* +--------+--------+ */
    } breg;
};
  
/*  Flag definitions  */
  
#define SF  0x0080                      /* 8086 Sign Flag */
#define ZF  0x0040                      /* 8086 Zero Flag */
#define CF  0x0001                      /* 8086 Carry Flag */

#endif
