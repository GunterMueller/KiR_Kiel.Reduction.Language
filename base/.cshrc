#!/bin/csh
#
# Cshrc 1.0 (c) by Carsten Rathsack
#
#################################################################
#
# .cshrc file
#
# initial setup file for both interactive and noninteractive
# C-Shells and tcsh
#
#################################################################

#
# Setup some globel shell variables
#

setenv RCSROOT ~base
setenv HOST   `hostname`
setenv PRINTER speedy
setenv FHELPFILE        /home/luxor2/base/inc/creduma.hlp
limit  core             0

#
# If .login/.xsession is not yet done or noninteractive shell exit
#

if ($?SYSTEM == 0 || $?prompt == 0) exit(0)

#
# force tcsh
#

if ($?tcsh == 0) then
  exec tcsh
endif

# set this for all shells

set noclobber
set filec
set nobeep

# some settings for tcsh

#set correct = all
set autolist

setenv MBOX "~/Mail/mbox"

# change directory macros
    alias     cd      'cd \!*; header $cwd'

    alias     rm      'rm -i'
    alias     mv      'mv -i'
    alias     cp      'cp -i'
    alias     l       'ls -Fla'
    alias     la      'ls -Fa'
    alias     ll      'ls -Fl'
    alias     ls      'ls -F'
    alias     ls      'ls -F'

switch ($SYSTEM)
  case "sunview":
    alias d		'texsun -ns 4 -tm 1.5 -sm 0.5 $TEXFILE.dvi'
    alias header        'echo -n "]l\!*\"'
    breaksw
  case "x11r5":
    alias d		'xdvi -keep $TEXFILE.dvi'
    alias header        'echo -n "]2;\!*"'
    breaksw
  case "openwin":
    alias d		'xdvi -keep $TEXFILE.dvi'
    alias header        'echo -n "]l\!*\"'
    breaksw
  case "unknown":
    alias header        'echo'
    breaksw
endsw

umask 022
setenv TEXPRINT texprint
set history=40
set ignoreeof
set prompt="-$USER-$HOST-> "
header $cwd
#if ("$LOGNAME" == "$USER") /usr/games/fortune
