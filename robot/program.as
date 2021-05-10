.PROGRAM hockey.pc()
  ;
  .timeout = 1 ; sec
  .max_bytes = 255
  ;
  IF TASK(1002) == 0 THEN
    PCEXECUTE 2: ifp.pc
  END
  ;
  WHILE TRUE DO
    UDP_RECVFROM .err_code, udp_port, .$recv_str[0], .null, .timeout, .ip[0], .max_bytes
    IF (.err_code == 0) THEN
      $temp = $DECODE(.$recv_str[0],",")
      smx = VAL($temp)
      $temp = $DECODE(.$recv_str[0],",",1)
      $temp = $DECODE(.$recv_str[0],",")
      smy = VAL($temp)
    END
  END
  ;
.END
.PROGRAM ifp.pc()
	; *******************************************************************
	;
	; Program:      ifp.pc
	; Comment:      
	; Author:       User
	;
	; Date:         5/10/2021
	;
	; *******************************************************************
	;
	
.END
.PROGRAM teach.pc()
  ;
  
  ;
.END
.PROGRAM Comment___ () ; Comments for IDE. Do not use.
	; @@@ PROJECT @@@
	; @@@ HISTORY @@@
	; @@@ INSPECTION @@@
	; @@@ CONNECTION @@@
	; 
	; 
	; 
	; @@@ PROGRAM @@@
	; 0:hockey.pc:B
	; 0:ifp.pc:B
	; 0:teach.pc:B
	; @@@ TRANS @@@
	; @@@ JOINTS @@@
	; @@@ REALS @@@
	; @@@ STRINGS @@@
	; @@@ INTEGER @@@
	; @@@ SIGNALS @@@
	; @@@ TOOLS @@@
	; @@@ BASE @@@
	; @@@ FRAME @@@
	; @@@ BOOL @@@
.END
.REALS
udp_port = 49152
.END
