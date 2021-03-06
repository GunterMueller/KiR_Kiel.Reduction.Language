#! /bin/awk -f

BEGIN	{
		functionHeaderPrinted = 0;

		numIds = 0;

		printf("/* This file is automatically generated (by scr2c.awk)\n** DO NOT EDIT BELOW!\n*/\n\n");

		printf("#include <dbug.h>\n\n");
		printf("#include <msMacros.h>\n");
		printf("#include <scriptMacros.h>\n");
		printf("#include <groups.h>\n");
		printf("#include <externVars.h>\n\n");

		# BarChart stuff...
		printf("extern void SET_MODE();\n");
		printf("extern void SET_X_LABEL();\n");
		printf("extern void SET_Y_LABEL();\n");
		printf("extern void SET_X_GRID();\n");
		printf("extern void SET_Y_GRID();\n");
		printf("extern void SET_X_MIN();\n");
		printf("extern void SET_X_MAX();\n");
		printf("extern void SET_Y_MIN();\n");
		printf("extern void SET_Y_MAX();\n");
		printf("extern void SET_LEGEND();\n");
		printf("extern void SET_STYLE();\n");
		printf("extern void SET_LINESTYLE();\n");
		printf("extern void SET_MARKER();\n");
		printf("extern void SET_SHADE();\n");
		printf("extern void CREATE_BAR();\n");
		printf("extern void INCREMENT();\n");
		printf("extern void REMOVE_XY();\n");
		printf("extern void ADD_XY();\n");
		printf("extern void UPDATE_XY();\n");

		# ProcArray stuff...
		printf("extern void CREATION_ARROW();\n");
		printf("extern void FILL_SLOT();\n");
		printf("extern void SHOW_HISTORY();\n");
		printf("extern void SHOW_FUTURE();\n");
		printf("extern void SET_METER();\n");
		printf("extern void DRAW_METER();\n");

		# HeapArray stuff...
		printf("extern void SET_DIMENSIONS();\n");
		printf("extern void FILL_ALL();\n");
		printf("extern void FILL_BLOCK();\n");
		printf("extern int GET_HEAPBASE();\n");
		printf("extern void DRAW_FRAGMENTATION();\n");
		printf("extern void SET_CACHING();\n");
		printf("extern void SET_STACKING();\n");
		printf("extern void SET_SEPARATION();\n");
		printf("extern void SET_FRAGMENTATION();\n");

		# ProcTree stuff...
		printf("extern void ADD_NODE();\n")
		printf("extern void REMOVE_NODE();\n");
		printf("extern void SET_NODES();\n");
		printf("extern void SET_LABELS();\n");
		printf("extern void SET_DEPTH();\n");
		printf("extern void ZOOM_IN();\n");
		printf("extern void ZOOM_OUT();\n");
		printf("extern void SHOW_TOP();\n");

		# Global stuff...
		printf("extern void NEXT_FRAME();\n");
		printf("extern void NEXT_FRAME_BUT_CONTINUE();\n");
		printf("extern void SET_DELAY();\n");
		printf("extern void SET_RATE();\n");
		printf("extern void SET_TITLE();\n");
		printf("extern void SET_STYLE();\n");
		printf("extern void SET_COLORS();\n");
		printf("extern void GET_TIME();\n");
		printf("extern void HEADER_FILE();\n");
		printf("extern void HEADER_TIME();\n");
		printf("extern void HEADER_NODISTNUM();\n");
		printf("extern void HEADER_DIMENSION();\n");
		printf("extern void HEADER_STEPS();\n");
		printf("extern void HEADER_FACTOR();\n");
		printf("extern void HEADER_BINARY();\n");
		printf("extern void HEADER_HEAPSIZE();\n");
		printf("extern void HEADER_HEAPDES();\n");
		printf("extern void FLUSH();\n\n");
		printf("extern void SAVE();\n");
		printf("extern void PRINT();\n");
		printf("extern void CLEAR();\n");
	}

($0 == "DEFS:")	{
		printf("\n");
		next;
	}

/^[^\t]/{
		if(!functionHeaderPrinted) {
			printf("\n\nvoid eval()\n{\n");
			printf("  /*DBUG_ENTER(\"eval\");*/\n\n");
			printf("  switch(ID) {\n");

			functionHeaderPrinted = 1;
		}

		printf("\n  case MINDEX_%s\n", $0);

		if(($0 != "BEGIN:") && ($0 != "END:")) {
			rememberIds[numIds] = substr($0, 1, length($0)-1);
			++numIds;
		}
	}

/^\t/	{
		printf("%s\n", substr($0, 2, 10000));
	}

END	{
		printf("  }\n\n  /*DBUG_VOID_RETURN*/\n\n  return;\n}\n");

		printf("\n\nvoid iNeed(needed)\n");
		printf("char needed[IDS_LEN][MAX_IDS]; {\n");
		printf("  int i = 0;\n\n");

		for(i=0; i<numIds; ++i) {
			printf("  strcpy(needed[i++], \"%s\");\n",\
			       rememberIds[i]);
		}
		printf("  strcpy(needed[i++], \"\");\n");

		printf("\n  return;\n}\n");
	}



