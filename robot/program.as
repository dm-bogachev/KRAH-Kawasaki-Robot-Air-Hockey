.AUXDATA
N_INT1    "pc1_ok  "
N_INT3    "pc2_ok  "
N_INT5    "motion_ok  "
N_INT6    "motion_restart  "
N_INT7    "teach_p1  "
N_INT8    "teach_p1l  "
N_INT9    "teach_p2  "
N_INT10    "teach_p2l  "
N_INT11    "teach_p3  "
N_INT12    "teach_p3l  "
N_INT13    "teach_ok  "
N_INT14    "teach_okl  "
N_INT15    "teach_reset  "
.END
.INTER_PANEL_D
0,1,"  PC1 OK","","","",10,7,4,10,2001,0
1,1,"  PC2 OK","","","",10,7,4,10,2003,0
2,1," MOTION OK","","","",10,7,4,10,2005,0
6,8,"udp_port"," UDP PORT","",10,9,5,1,0
7,10,"","  RESTART","   PCS","",10,4,9,1,PCEX 5: autostart5.pc,0
9,2,"","  RESTART","  MOTION"," ",10,4,7,2006,0
31,3,"","   TEACH","    P[3]","",10,4,7,0,0,2011,2012,0
34,10,"","  TEACH","  START","",10,4,7,2,PCEX 3: teach.pc,0
49,3,"","   TEACH","    P[1]","",10,4,7,0,0,2007,2008,0
52,3,"","   TEACH","    P[2]","",10,4,7,0,0,2009,2010,0
55,3,"","  CREATE","  FRAME","",10,4,7,0,0,2013,2014,0
.END
.INTER_PANEL_TITLE
"KRAH Contol Page",1
"KRAH Setup Frame",1
"",0
"",0
"",0
"",0
"",0
"",0
.END
.INTER_PANEL_COLOR_D
182,3,224,244,28,159,252,255,251,255,0,31,2,241,52,219,
.END
.PROGRAM hockey()
  ;
  SPEED 100 ALWAYS
  ACCURACY 5 ALWAYS
  CP ON
  TOOL NULL
  ;
  .motion_finished = FALSE
  hockey_points = 0
  ;
  WHILE TRUE DO
    FOR .j = 1 TO hockey_points
      .motion_finished = FALSE
      LMOVE tframe + hockey_point[.j]
    END
    BREAK
    IF (.motion_finished == FALSE) THEN
      hockey_points = 0
      .motion_finished = TRUE
    END
    
  END
  ;
.END
.PROGRAM teach ()
  ;
  LMOVE p[1]
  LMOVE p[2]
  LMOVE p[3]
  ;
  
.END
.PROGRAM hockey.pc ()
  ;
  .timeout = 10 ; sec
  .max_bytes = 255
  ;
  WHILE TRUE DO
    UDP_RECVFROM .err_code, udp_port, .$recv_str[0], .null, .timeout, .ip[0], .max_bytes
    IF (.err_code == 0) THEN
      ;TYPE 0: .$recv_str[0]
      .$temp1 = $DECODE (.$recv_str[0], ":")
      .numberOfPoints = VAL (.$temp1)
      .$temp1 = $DECODE (.$recv_str[0], ":", 1)
      ;TYPE 0: .$recv_str[0]
      FOR .i = 1 TO .numberOfPoints - 1
        .$temp1 = $DECODE (.$recv_str[0], ":")
        ;
        CALL addpoint.pc(.$temp1, .i)
        ;
        .$temp1 = $DECODE (.$recv_str[0], ":", 1)
        ;TYPE 0: .deltax, .deltay
        ;POINT hockey_point[.i] = TRANS (.deltax, .deltay)
      END
      .$temp1 = $DECODE (.$recv_str[0], ":")
      CALL addpoint.pc(.$temp1, .numberOfPoints)
      hockey_points = .numberOfPoints
      ;TYPE 0: "Start"
      ;
      ;
    END
  END
  ;
.END
.PROGRAM ifp.pc ()
  ;
  WHILE TRUE DO
    .start_permission = TRUE
    IF SIG (motion_restart) THEN
      IF SWITCH (CS) THEN
        .start_permission = FALSE
      END
      IF SWITCH (TEACH_LOCK) THEN
        .start_permission = FALSE
      END
      IF NOT SWITCH (REPEAT) THEN
        .start_permission = FALSE
      END
      IF NOT SWITCH (POWER) THEN
        MC ZPOWER ON
        TWAIT 1
      END
      IF .start_permission THEN
        MC EXECUTE hockey
      END
    END
  END
  ;
  
.END
.PROGRAM teach.pc ()
  ;
  WHILE TRUE DO
    IF SIG (teach_p1) THEN
      HERE p[0]
      HERE p[1]
      SIGNAL teach_p1l
      TWAIT 1
    END
    ;
    IF SIG (teach_p2) THEN
      HERE p[2]
      SIGNAL teach_p2l
      TWAIT 1
    END
    ;
    IF SIG (teach_p3) THEN
      HERE p[3]
      SIGNAL teach_p3l
      TWAIT 1
    END
    ;
    IF SIG (teach_ok) THEN
      POINT tframe = FRAME (p[2], p[1], p[3], p[0])
      SIGNAL teach_okl, -teach_p1l, -teach_p2l, -teach_p3l
      deltax = -ABS(DY(p[2]) - DY(p[1]))
      deltay = ABS(DX(p[3]) - DX(p[2]))
      TWAIT 5
      SIGNAL -teach_okl
      GOTO fin
    END
    ;
  END
  fin:
  ;
.END
.PROGRAM autostart5.pc () ;
  ;
  IF TASK (1001) <> 1 THEN
    PCEXECUTE 1: hockey.pc
  END
  ;
  IF TASK (1002) <> 1 THEN
    PCEXECUTE 2: ifp.pc
  END
  ;
.END
.PROGRAM addpoint.pc (.$str,.index) ; 
  ;
  .$temp2 = $DECODE (.$str, ";")
  .deltax = (100 - VAL (.$temp2))
  ;TYPE 0: .deltax
  .deltax = deltax*.deltax/100
  .$temp2 = $DECODE (.$str, ";",1)
  .deltay = VAL (.$str)
  .deltay = deltay*.deltay/100
  POINT hockey_point[.index] = TRANS (.deltax, .deltay)
  TYPE 0: .deltax, .deltay
  ;
.END
.PROGRAM Comment___ () ; Comments for IDE. Do not use.
	; @@@ PROJECT @@@
	; @@@ HISTORY @@@
	; @@@ INSPECTION @@@
	; @@@ CONNECTION @@@
	; Standard 1
	; 192.168.0.2
	; 23
	; @@@ PROGRAM @@@
	; 0:hockey:F
	; 0:teach:F
	; 0:hockey.pc:B
	; 0:ifp.pc:B
	; 0:teach.pc:B
	; 0:autostart5.pc:B
	; 0:addpoint.pc:B
	; .$str 
	; .index 
	; @@@ TRANS @@@
	; @@@ JOINTS @@@
	; @@@ REALS @@@
	; @@@ STRINGS @@@
	; @@@ INTEGER @@@
	; @@@ SIGNALS @@@
	; pc1_ok 
	; teach_p2 
	; pc2_ok 
	; motion_ok 
	; motion_restart 
	; teach_p1 
	; teach_p1l 
	; teach_p2l 
	; teach_p3 
	; teach_p3l 
	; teach_ok 
	; teach_okl 
	; teach_reset 
	; @@@ TOOLS @@@
	; hockey_tool 
	; @@@ BASE @@@
	; @@@ FRAME @@@
	; tframe 
	; @@@ BOOL @@@
.END
.TRANS
p[3] 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
p[2] 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
p[1] 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
p[0] 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
tframe -158.983856 -716.104248 245.000000 175.046768 179.998154 86.935440
hockey_tool 0.000000 0.000000 0.000000 0.000000 180.000000 0.000000
.END
.REALS
udp_port = 49152
pc1_ok = 2001
teach_p2 = 2009
pc2_ok = 2003
motion_ok = 2005
motion_restart = 2006
teach_p1 = 2007
teach_p1l = 2008
teach_p2l = 2010
teach_p3 = 2011
teach_p3l = 2012
teach_ok = 2013
teach_okl = 2014
teach_reset = 2015
deltax = -803.214
deltay = 360.165
hockey_points = 2
.END
