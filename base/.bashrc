# --------------------------------------------------------------------     
# .bashrc fuer bash
#   
# HISTORY
#  
# 13.02.2000 wib    erste Version
# 28.03.2000 wib    Feinheiten korrigiert
# 12.06.2001 wib    X11R5 komplett gestrichen
# 15.06.2001 wib    KDEDIR eingebaut
# 18.06.2003 wib    kde3, PKG_CONFIG_PATH
#  
  
# --------------------------------------------------------------------
# zuerst ein paar globale Einstellungen bzw Pfade
# 
  
export OPENWINHOME=/usr/openwin

if [ -z "$KDEDIR" ]; then
    KDEDIR=/opt/kde3
fi

XPATH=$KDEDIR/bin:$OPENWINHOME/bin:/usr/dt/bin
CPATH=/opt/gnu/bin:/usr/ccs/bin:/opt/SUNWspro/bin
LPATH=/usr/local/bin:/usr/local/teTeX/bin:$HOME/bin:.

export PATH=/bin:/usr/sbin:/usr/proc/bin:$XPATH:$CPATH:$LPATH

XMANPATH=$KDEDIR/man:$OPENWINHOME/man:/usr/dt/man
CMANPATH=/opt/gnu/man:/opt/SUWNspro/man
LMANPATH=/usr/local/man:/usr/local/teTeX/man

export MANPATH=/usr/share/man:$XMANPATH:$CMANPATH:$LMANPATH

export INFOPATH=/opt/gnu/info:/usr/local/info:/usr/local/teTeX/info

#
# falls Software compiliert werden soll
#
export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:/opt/gnu/lib/pkgconfig"

#
# for csh compat mode !
#
path="$PATH"

#
# Der 'normale' Prompt ist langweilig
#
export PS1="`uname -n`{$LOGNAME} "

#
# damit das mit den Umlauten klappt
#
export LC_CTYPE=iso_8859_1
export LC_COLLATE=C

#
# emacs ist unser Standard
#
export EDITOR=emacs

if [ -x /opt/gnu/bin/less -o /bin/less ]; then
  export PAGER=less 
  export LESS=MQi
else
  export PAGER=more
fi

# --------------------------------------------------------------------
# aliase fuer jeden Geschmack und Bedarf
#

alias ll='ls -laF'
alias ls='ls -F'
alias rm='rm -i'
alias cp='cp -pi'
alias mv='mv -i'
alias md='mkdir'
alias h='history'
alias p='pushd'
alias o='popd'
 
# --------------------------------------------------------------------
# bei xterms wollen wir 'user@rechner:pfad' in die Titelleiste bringen
#

function is_xterm()
{
  [ ${TERM/*xterm*/xterm} == 'xterm' ]
}

if is_xterm
then
    alias cd="_xcd"
else
    alias cd="_vcd"
fi
   
function _vcd()
{
  \cd "$@"
  echo $PWD
} 

function _xcd()
{
  \cd "$@"
  echo $PWD
  echo -n "]2;$LOGNAME@`uname -n`:$PWD"
}

# --------------------------------------------------------------------
# fuer die ewig gestrigen :-)
#

function setenv()
{
  k="$1"
  shift

  export $k="$@"
} 

# --------------------------------------------------------------------
# ein paar nuetzliche Einstellungen der BASH
#

# eine lange History
export HISTSIZE=500

# doppelte Eintraege ignorieren
export HISTCONTROL=ignoreboth

# bei TAB completion diverse Dateien ignorieren
export FIGNORE='~:.o:.orig:.ori:.old:.backup'

# --------------------------------------------------------------------
# und noch ein kleiner Rest
# 
umask 022

# limit coredumpsize 0
ulimit -c 0

# --------------------------------------------------------------------
# end of file
# --------------------------------------------------------------------
