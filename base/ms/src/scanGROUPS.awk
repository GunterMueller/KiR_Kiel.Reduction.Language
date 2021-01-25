#! /bin/awk -f

BEGIN	{
		mindex = 1;

		############
		# groups.h #
		############

		printf ("/* This file is automatically generated (by scanGROUPS.awk)\n** DO NOT EDIT BELOW!\n*/\n\n") > "groups.h";

                printf ("#include \"ids.h\"\n\n") > "groups.h";
                printf ("extern char *ids[];\n\n") > "groups.h";

		printf("#define MSCAN(F, TIME, PROCESSOR, PROCESS, ID) \\\n") > "groups.h";
		printf("\tmscanfTime(F, TIME) + \\\n") > "groups.h";
		printf("\tmscanfProcessor(F, PROCESSOR) + \\\n") > "groups.h";
		printf("\tmscanfProcess(F, PROCESS) + \\\n") > "groups.h";
		printf("\tmscanfId(F, ID)\n") > "groups.h";

		printf("#define MPRINT(F, TIME, PROCESSOR, PROCESS, ID) \\\n") > "groups.h";
		printf("\tmprintfTime(F, TIME) + \\\n") > "groups.h";
		printf("\tmprintfDelimiter(F) + \\\n") > "groups.h";
		printf("\tmprintfProcessor(F, PROCESSOR) + \\\n") > "groups.h";
		printf("\tmprintfDelimiter(F) + \\\n") > "groups.h";
		printf("\tmprintfProcess(F, PROCESS) + \\\n") > "groups.h";
		printf("\tmprintfDelimiter(F) + \\\n") > "groups.h";
		printf("\tmprintfId(F, ID)\n\n") > "groups.h";

		printf("#define MSCAN_LENGTH(F, LENGTH) \\\n") > "groups.h";
		printf("\tmscanfLength(F, LENGTH)\n") > "groups.h";

		printf("#define MPRINT_LENGTH(F, LENGTH) \\\n") > "groups.h";
		printf("\tmprintfLength(F, LENGTH)\n\n") > "groups.h";

		#########
		# ids.h #
		#########

                printf ("/* This file is automatically generated (by scanGROUPS.awk)\n** DO NOT EDIT BELOW!\n*/\n\n") > "ids.h";
		printf ("char *ids[] = {\n") > "ids.h";
		printf("  \"\"\n") > "ids.h";

		##############
		# id2index.c #
		##############

		LEN=100;

		printf ("/* This file is automatically generated (by scanGROUPS.awk)\n** DO NOT EDIT BELOW!\n*/\n\n") > "id2index.c";

		printf("short id2Index(id)\n") > "id2index.c";
		printf("char *id;\n") > "id2index.c";

		printf("{\n") > "id2index.c";
		printf("  DBUG_ENTER(\"id2index\");\n\n") > "id2index.c";

		#############
		# convert.c #
		#############

		LEN=100;

		printf ("/* This file is automatically generated (by scanGROUPS.awk)\n** DO NOT EDIT BELOW!\n*/\n\n") > "convert.c";

		printf("#include \"externVars.h\"\n\n") > "convert.c";
		printf("extern void (*eval)();\n\n") > "convert.c";

		printf("int convert(inF)\n") > "convert.c";
		printf("FILE *inF;\n") > "convert.c";

		printf("{\n") > "convert.c";
		printf("  int length = 0;\n") > "convert.c";	
		printf("  DBUG_ENTER(\"convert\");\n\n") > "convert.c";	

		printf("  switch(ID) {\n") > "convert.c";

		#############
		# trevnoc.c #
		#############

		LEN=100;

		printf ("/* This file is automatically generated (by scanGROUPS.awk)\n** DO NOT EDIT BELOW!\n*/\n\n") > "trevnoc.c";

		printf("#include \"externVars.h\"\n\n") > "trevnoc.c";

		printf("void trevnoc()\n") > "trevnoc.c";
		printf("{\n") > "trevnoc.c";
		printf("  int dummy;\n\n") > "trevnoc.c";
		printf("  DBUG_ENTER(\"trevnoc\");\n\n") > "trevnoc.c";	

		printf("  switch(ID) {\n") > "trevnoc.c";

		##########
		# vars.h #
		##########

		printf ("/* This file is automatically generated (by scanGROUPS.awk)\n** DO NOT EDIT BELOW!\n*/\n\n") > "vars.h";

		printf("double TIME;\n") > "vars.h";
		printf("int PROCESS;\n") > "vars.h";
		printf("short PROCESSOR, ID;\n\n") > "vars.h";

		################
		# externVars.h #
		################

		printf ("/* This file is automatically generated (by scanGROUPS.awk)\n** DO NOT EDIT BELOW!\n*/\n\n") > "externVars.h";

		printf("extern double TIME;\n") > "externVars.h";
		printf("int PROCESS;\n") > "externVars.h";
		printf("extern short PROCESSOR, ID;\n\n") > "externVars.h";
	}

/^\t/	{
		if(NF) {
			############
			# groups.h #
			############

			printf("#define MINDEX_%s %d\n", $1, mindex++) > "groups.h";

			if(NF > 1) {
				printf("#define MSCAN_"$1"(F") > "groups.h";
				for(i=2; i<=NF; ++i) {
					printf(", "substr($i, 2)) > "groups.h";
				}
				printf(") \\\n") > "groups.h";

				for(i=2; i<=NF; ++i) {
					if(substr($i, 1, 1)=="#") {
						printf("\tmscanfInt(F, %s)", substr($i, 2)) > "groups.h";
					} else {
						printf("\tmscanfString(F, %s)", substr($i, 2)) > "groups.h";
					}
					if(i < NF) {
						printf(" + \\\n") > "groups.h";
					}
				}
				printf("\n") > "groups.h";
			}

			printf("#define MPRINT_"$1"(F") > "groups.h";
			for(i=2; i<=NF; ++i) {
				printf(", "substr($i, 2)) > "groups.h";
			}
			printf(") \\\n") > "groups.h";

			for(i=2; i<=NF; ++i) {
				printf("\tmprintfDelimiter(F) + \\\n") > "groups.h";
				if(substr($i, 1, 1)=="#") {
					printf("\tmprintfInt(F, %s)", substr($i, 2)) > "groups.h";
				} else {
					printf("\tmprintfString(F, %s)", substr($i, 2)) > "groups.h";
				}
				printf(" + \\\n") > "groups.h";
			}
			printf("\tmprintfNewLine(F)\n") > "groups.h";
			printf("\n") > "groups.h";

			#########
			# ids.h #
			#########

                        printf(", \"%s\"\n", $1) > "ids.h";

			##############
			# id2index.c #
			##############

			printf("  if(!strcmp(id, \"%s\")) {\n", $1) > "id2index.c";

			printf("    DBUG_RETURN(MINDEX_%s);\n", $1) > "id2index.c";
			printf("  }\n") > "id2index.c";
			printf("\n") > "id2index.c";

			#############
			# convert.c #
			#############

			printf("  case MINDEX_%s: {\n", $1) > "convert.c";

			if(NF > 1) {
				printf("    length = MSCAN_%s(inF", $1) > "convert.c";
				for(i=2; i<=NF; ++i) {
					if(substr($i, 1, 1)=="#") {
						printf(", &%s", substr($i, 2)) > "convert.c";
					} else {
						printf(", %s", substr($i, 2)) > "convert.c";
					}
				}
				printf(");\n") > "convert.c";
			}

			printf("    (*eval)();\n") > "convert.c";
			printf("    DBUG_RETURN(length);\n") > "convert.c";

			printf("  }\n") > "convert.c";
			printf("\n") > "convert.c";

			#############
			# trevnoc.c #
			#############

			printf("  case MINDEX_%s: {\n", $1) > "trevnoc.c";

			printf("    dummy = MPRINT_%s(outF", $1) > "trevnoc.c";
			for(i=2; i<=NF; ++i) {
				printf(", %s", substr($i, 2)) > "trevnoc.c";
			}
			printf(");\n") > "trevnoc.c";

			printf("    DBUG_VOID_RETURN;\n") > "trevnoc.c";

			printf("  }\n") > "trevnoc.c";
			printf("\n") > "trevnoc.c";

			##########
			# vars.h #
			##########

			printf("/* %s */\n", $1) > "vars.h";

			for(i=2; i<=NF; ++i) {
				if(substr($i, 1, 1)=="#") {
					printf("int %s;\n", substr($i, 2)) > "vars.h";
				} else {
					printf("char %s[%d];\n", substr($i, 2), LEN) > "vars.h";
				}
			}
			printf("\n") > "vars.h";		

			################
			# externVars.h #
			################

			printf("/* %s */\n", $1) > "externVars.h";

			for(i=2; i<=NF; ++i) {
				if(substr($i, 1, 1)=="#") {
					printf("extern int %s;\n", substr($i, 2)) > "externVars.h";
				} else {
					printf("extern char %s[%d];\n", substr($i, 2), LEN) > "externVars.h";
				}
			}
			printf("\n") > "externVars.h";
		}
	}
END {
		#########
		# ids.h #
		#########

		printf ("};\n") > "ids.h";

		##############
		# id2index.c #
		##############

		printf("  DBUG_RETURN(0);\n") > "id2index.c";
		printf("}\n") > "id2index.c";

		#############
		# convert.c #
		#############

		printf("  }\n  DBUG_PRINT(\"convert\", (\"Unknown id: %%d\", ID));\n") > "convert.c";
		printf("  DBUG_RETURN(-1);\n") > "convert.c";
		printf("}\n") > "convert.c";

		#############
		# trevnoc.c #
		#############

		printf("  }\n  DBUG_PRINT(\"trevnoc\", (\"Unknown id: %%d\", ID));\n") > "trevnoc.c";
		printf("  DBUG_VOID_RETURN;\n") > "trevnoc.c";
		printf("}\n") > "trevnoc.c";
}