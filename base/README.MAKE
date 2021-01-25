Sven-Bodo Scholz                                                Kiel, den 17.1.94
=====================================================================================

To remake an actual version of the lred proceed as follows:

1) update red/src		/* checkout actual version                  	*/
2) red/src/configure sun   	/* generate makefiles... 			*/
3) make				/* generate new execuutables in red/....obj 	*/
4) make install			/* copy executables to bin/lib              	*/
5) (make distclean)		/* removes everything built from the sources 	*/
				/* ( even the Makefiles ...)			*/

to be continued..
