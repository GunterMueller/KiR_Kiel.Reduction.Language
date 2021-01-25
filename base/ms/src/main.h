#ifndef MAIN

#define MAIN	main(argc, argv)				\
			int argc;				\
			char ** argv;

#define OPT	while (--argc > 0 && **++argv == '-') {		\
		  switch (*++*argv) {				\
		    case 0:	--*argv;			\
		           	break;				\
		    case '-':	if (!(*argv)[1]) {		\
				  ++argv, --argc;		\
				  break;			\
				  }				\
		    default:	do {				\
				  switch (**argv) {		\
				  case 0: /* Dirty trick!!! TB */
#define ARG				continue;		\
				    case
#define OTHER				continue;		\
				    }
#define ENDOPT			  } while (*++*argv);		\
				continue;			\
		    }						\
		    break;					\
		  }

#define PARM	if (*++*argv) ;					\
		else if (--argc > 0)				\
		       ++argv;					\
		else {						\
		  --*argv, ++argc;				\
		  break;					\
		  }

#define NEXTOPT	*argv += strlen(*argv) -1;

#endif /* MAIN */
