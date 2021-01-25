# @(#)Login 1.4 88/03/03 SMI
# Cshrc 1.0 (c) by Sven-Bodo Scholz, Carsten Rathsack
##################################################################
#
#         .login file
#
#         Read in after the .cshrc file when you log in.
#         Not read in for subsequent shells.  For setting up
#         terminal and global environment characteristics.
#
##################################################################

#
# Setup some globel shell variables
#
set xconfig =		.xinitrc.x
set openwinconfig =	.openwin-init
set xerrors =		.xerrors
set pass_env =		/tmp/display.$USER

#
# Check whether i am remote.
#
if( (`who am i | fgrep "(" | wc -l` == 1) || ($?REMOTEHOST == 1)) then 
  set remote =		true
  if -e $pass_env then
    source $pass_env
    rm $pass_env
  else
    setenv SYSTEM	unknown
  endif

  setenv TERMCAP "vs|xterm|vs100|xterm terminal emulator (X window system):li#58:co#92:  :cr=^M:do=^J:nl=^J:bl=^G:le=^H:ho=\E[H:co#80:li#65:cl=\E[H\E[2J:bs:am:cm=\E[%i%d;%dH:nd=\E[C:up=\E[A:ce=\E[K:cd=\E[J:so=\E[7m:se=\E[m:us=\E[4m:ue=\E[m:md=\E[1m:mr=\E[7m:me=\E[m:ku=\EOA:kd=\EOB:kr=\EOC:kl=\EOD:kb=^H:k1=\EOP:k2=\EOQ:k3=\EOR:k4=\EOS:ta=^I:pt:sf=\n:sr=\EM:al=\E[L:dl=\E[M:ic=\E[@:dc=\E[P:MT:ks=\E[?1h\E=:ke=\E[?1l\E>:is=\E[r\E[m\E[2J\E[H\E[?7h\E[?1;3;4;6l:rs=\E[r\E<\E[m\E[2J\E[H\E[?7h\E[?1;3;4;6l:xn:AL=\E[%dL:DL=\E[%dM:IC=\E[%d@:DC=\E[%dP:hs:ts=\E[?E\E[?%i%dT:fs=\E[?F:es:ds=\E[?E::"

endif

#
# When not using X-terminal and not being remote ask for system
#

if (($?XSESSION == 0) && ($?remote == 0)) then

setenv  SYSTEM          0

while ($SYSTEM == 0)

  echo " "
  echo "Which Window-System do you want ?"
  echo "C: Console"
  echo "S: Sunview"
  echo "X: Plain X-Windows"
  echo "O: Openwindows"

  setenv       SYSTEM          $<

  switch ($SYSTEM)
    case "[sS]*" :
      setenv SYSTEM    sunview
      breaksw;

    case "[xX]*" :
      setenv SYSTEM    x11r5
      breaksw;

    case "[oO]*" :
      setenv SYSTEM    openwin
      breaksw;

    default :
      setenv SYSTEM    unknown

  endsw
end

endif

#
# Set all paths
#

setenv OPENWINHOME	/usr/openwin
setenv XWINHOME		/opt/X11R5

set path   = (/opt/gnu/bin /bin /usr/bin /usr/ccs/bin /usr/ucb/bin /usr/sbin)
set lpath  = (/opt/local/bin /opt/SUNWspro/bin)
set opath  = ($OPENWINHOME/bin)
set xpath  = ($XWINHOME/bin)
set cpath  = ($HOME/rcsbin $HOME/bin)

set lib   = /usr/lib\:/usr/ucblib\:/opt/gnu/lib
set olib  = $OPENWINHOME/lib
set xlib  = $XWINHOME/lib

set lman  = /opt/local/man\:/opt/SUNWspro/man\:/opt/local/teTeX/man\:/opt/gnu/man
set man   = /usr/share/man
set oman  = $OPENWINHOME/man
set xman  = $XWINHOME/man
set sman  = $HOME/man
 
setenv nrel /usr/ncube/current
set NCUBEpath = $nrel/bin\:$nrel/bin/`/bin/arch`\:$nrel/diag/`/bin/arch`
set path = ($NCUBEpath $path)


#
# Set all paths in dependency of $SYSTEM.
# and launch selected window system
#
switch ( $SYSTEM )

  case "sunview":
    setenv LD_LIBRARY_PATH      $lib
    setenv MANPATH              $man\:$lman\:$sman
    set path =                  ($cpath $lpath $path .)
  breaksw

  case "x11r5":
    setenv LD_LIBRARY_PATH	$xlib\:$olib\:$lib
    setenv MANPATH		$xman\:$oman\:$man\:$lman\:$sman
    set path =			($cpath $xpath $opath $path $lpath .)
  breaksw

  case "openwin":
    setenv LD_LIBRARY_PATH      $olib\:$lib
    setenv MANPATH              $oman\:$man\:$lman\:$sman
    set path  =                 ($cpath $opath $path $lpath .)
  breaksw

  case "unknown":
    setenv LD_LIBRARY_PATH      $xlib\:$olib\:$lib
    setenv MANPATH              $xman\:$oman\:$man\:$lman\:$sman
    set path =                  ($cpath $xpath $opath $path $lpath .)
endsw

#
# When being remote we are done.
#

if ($?remote != 0) then
  source .cshrc
  exit(0)
endif

#
# When using X-terminal ask for system using xmessage
#

if (($?XSESSION != 0) && ($?remote == 0)) then
  setenv SYSTEM		0
  while ($SYSTEM == 0)
    xmessage -buttons "Plain X-Windows":1,"Openwindows":2,"        Quit        ":9 \
		"Which Window-System do you want ?"
    switch ($status)
      case 1:
        setenv SYSTEM	x11r5
        breaksw
      case 2:
        setenv SYSTEM	openwin
        breaksw
      case 9:
        exit(0)
    endsw
  end
endif

#now starting the window-manager

switch ($SYSTEM)

#*********************************************************************

  case "sunview" :

    input_from_defaults
    sunview
    .logout
    login lock
    breaksw

#*********************************************************************

  case "x11r5" :

    #
    # Determine the shell of the X config file.
    #

    #
    # If no XSESSION, start X11.
    #

    if ($?XSESSION == 0) then
      setenv DISPLAY	`hostname`:0.0
      keygen
      xinit /bin/sh -c "DISPLAY=$DISPLAY $shell $xconfig" \
                        -auth $HOME/.Xauthority >& $xerrors
      kbd_mode -a
      .logout
      login lock
    endif

    #
    # Do some Keyboard adjustments
    #

    xmodmap -e "keysym BackSpace = Delete"

    #
    # Startup all goodies.
    #

    source $xconfig

  breaksw

#*********************************************************************

  case "openwin" :

    #
    # If no XSESSION, start openwin.
    #

    if ($?XSESSION == 0) then
      setenv DISPLAY    `hostname`:0.0
      openwin >& $xerrors
      .logout
      login lock
    endif

    #
    # Do some Keyboard adjustments
    #

    xmodmap -e "keysym BackSpace = Delete"

    #
    # Startup all goodies.
    #

    source $openwinconfig
    olwm

  breaksw

************************************************************************

  case "unkown" :

    #
    # unkown session source .cshrc 
    #

    source .cshrc

  breaksw

endsw


