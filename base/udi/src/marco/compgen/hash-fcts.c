/*
** Dynamic hashing, after CACM April 1988 pp 446-457, by Per-Ake Larson.
** Coded into C, with minor code improvements, and with DH_hsearch(3) interface,
** by ejp@ausmelb.oz, Jul 26, 1988: 13:16;
** also, DH_hcreate/DH_hdestroy routines added to simulate DH_hsearch(3).
**
** These routines simulate DH_hsearch(3) and family, with the important
** difference that the hash table is dynamic - can grow indefinitely
** beyond its original size (as supplied to DH_hcreate()).
**
** Performance appears to be comparable to that of DH_hsearch(3).
** The 'source-code' options referred to in DH_hsearch(3)'s 'man' page
** are not implemented; otherwise functionality is identical.
**
** Compilation controls:
** DEBUG controls some informative traces, mainly for debugging.
** HASH_STATISTICS causes HashAccesses and HashCollisions to be maintained;
** when combined with DEBUG, these are displayed by DH_hdestroy().
**
** Problems & fixes to ejp@ausmelb.oz. WARNING: relies on pre-processor
** concatenation property, in probably unnecessary code 'optimisation'.
*/

# include	<stdio.h>
# include	<assert.h>
# include	<string.h>

# include 	"hash-fcts.h"

#include "dbug.h"

/*
** Constants
*/

# define SegmentSize		256
# define SegmentSizeShift	8	/* log2(SegmentSize)	*/
# define DirectorySize		256
# define DirectorySizeShift	8	/* log2(DirectorySize)	*/
# define Prime1			37
# define Prime2			1048583
# define DefaultMaxLoadFactor	5

/*
** Fast arithmetic, relying on powers of 2,
** and on pre-processor concatenation property
*/

# define MUL(x,y)		((x) << (y##Shift))
# define DIV(x,y)		((x) >> (y##Shift))
# define MOD(x,y)		((x) & ((y)-1))

/*
** local data templates
*/

typedef struct element
    {
    /*
    ** The user only sees the first two fields,
    ** as we pretend to pass back only a pointer to DH_ENTRY.
    ** {S}he doesn't know what else is in here.
    */
    char		*Key;
    char		*Data;
    struct element	*Next;	/* secret from user	*/
    } Element,*Segment;

typedef struct
    {
    short	p;		/* Next bucket to be split	*/
    short	maxp;		/* upper bound on p during expansion	*/
    long	KeyCount;	/* current # keys	*/
    short	SegmentCount;	/* current # segments	*/
    short	MinLoadFactor;
    short	MaxLoadFactor;
    Segment	*Directory[DirectorySize];
    } HashTable;

typedef unsigned long	Address;

/*
** external routines
*/

extern char	*calloc();
extern int	free();

/*
** DH_Entry points
*/

/*
** Internal routines
*/

static Address	Hash();
static void	ExpandTable();

/*
** Local data
*/

static HashTable	*Table = NULL;
# if HASH_STATISTICS
static long		HashAccesses, HashCollisions;
# endif

/*
** Code
*/

int
DH_hcreate(Count)
unsigned Count;
{
    int		i;
    DBUG_ENTER("DH_hcreate");

    /*
    ** Adjust Count to be nearest higher power of 2,
    ** minimum SegmentSize, then convert into segments.
    */
    i = SegmentSize;
    while (i < Count)
	i = i << 1;
    Count = DIV(i,SegmentSize);

    Table = (HashTable*)calloc(sizeof(HashTable),1);
    if (Table == NULL)
	DBUG_RETURN(0);
    /*
    ** resets are redundant - done by calloc(3)
    **
    Table->SegmentCount = Table->p = Table->KeyCount = 0;
    */
    /*
    ** Allocate initial 'i' segments of buckets
    */
    for (i = 0; i < Count; i++)
    {
	Table->Directory[i] = (Segment*)calloc(sizeof(Segment),SegmentSize);
	if (Table->Directory[i] == NULL)
	{
	    DH_hdestroy();
	    DBUG_RETURN(0);
	}
	Table->SegmentCount++;
    }
    Table->maxp = MUL(Count,SegmentSize);
    Table->MinLoadFactor = 1;
    Table->MaxLoadFactor = DefaultMaxLoadFactor;
# if DEBUG
    fprintf(
	    stderr,
	    "[DH_hcreate] Table %x Count %d maxp %d SegmentCount %d\n",
	    Table,
	    Count,
	    Table->maxp,
	    Table->SegmentCount
	    );
# endif
# if HASH_STATISTICS
	HashAccesses = HashCollisions = 0;
# endif
    DBUG_RETURN(1);
}

void
DH_hdestroy()
{
    int		i,j;
    Segment	*s;
    Element	*p,*q;
    DBUG_ENTER("DH_hdestroy");

    if (Table != NULL)
    {
	for (i = 0; i < Table->SegmentCount; i++)
	{
	    /* test probably unnecessary	*/
	    if ((s = Table->Directory[i]) != NULL)
	    {
		for (j = 0; j < SegmentSize; j++)
		{
		    p = s[j];
		    while (p != NULL)
		    {
			q = p->Next;
			free((char*)p);
			p = q;
		    }
		free(Table->Directory[i]);
		}
	    }
	}
	free(Table);
	Table = NULL;
# if HASH_STATISTICS && DEBUG
	fprintf(
		stderr,
		"[DH_hdestroy] Accesses %ld Collisions %ld\n",
		HashAccesses,
		HashCollisions
		);
# endif
    }
}

DH_ENTRY *
DH_hsearch(item,action)
DH_ENTRY   item;
DH_ACTION       action;	/* FIND/ENTER	*/
{
    Address	h;
    Segment	*CurrentSegment;
    int		SegmentIndex;
    int		SegmentDir;
    Segment	*p,q;
    DBUG_ENTER("DH_hsearch");

    assert(Table != NULL);	/* Kinder really than return(NULL);	*/
# if HASH_STATISTICS
    HashAccesses++;
# endif
    h = Hash(item.key);
    SegmentDir = DIV(h,SegmentSize);
    SegmentIndex = MOD(h,SegmentSize);
    /*
    ** valid segment ensured by Hash()
    */
    CurrentSegment = Table->Directory[SegmentDir];
    assert(CurrentSegment != NULL);	/* bad failure if tripped	*/
    p = &CurrentSegment[SegmentIndex];
    q = *p;
    /*
    ** Follow collision chain
    */
    while (q != NULL && strcmp(q->Key,item.key))
    {
	p = &q->Next;
	q = *p;
# if HASH_STATISTICS
	HashCollisions++;
# endif
    }
    if (
	q != NULL	/* found	*/
	||
	action == DH_FIND	/* not found, search only	*/
	||
	(q = (Element*)calloc(sizeof(Element),1))
	==
	NULL		/* not found, no room	*/
	)
	DBUG_RETURN((DH_ENTRY*)q);
    *p = q;			/* link into chain	*/
    /*
    ** Initialize new element
    */
    q->Key = item.key;
    q->Data = item.data;
    /*
    ** cleared by calloc(3)
    q->Next = NULL;
    */
    /*
    ** Table over-full?
    */
    if (++Table->KeyCount / MUL(Table->SegmentCount,SegmentSize) > Table->MaxLoadFactor)
	ExpandTable();	/* doesn't affect q	*/
    DBUG_RETURN((DH_ENTRY*)q);
}

/*
** Internal routines
*/

static Address
Hash(Key)
char *Key;
{
    Address		h,address;
    register unsigned char	*k = (unsigned char*)Key;

    h = 0;
    /*
    ** Convert string to integer
    */
    while (*k)
	h = h*Prime1 ^ (*k++ - ' ');
    h %= Prime2;
    address = MOD(h,Table->maxp);
    if (address < Table->p)
	address = MOD(h,(Table->maxp << 1));	/* h % (2*Table->maxp)	*/
    return(address);
}

void
ExpandTable()
{
    Address	NewAddress;
    int		OldSegmentIndex,NewSegmentIndex;
    int		OldSegmentDir,NewSegmentDir;
    Segment	*OldSegment,*NewSegment;
    Element	*Current,**Previous,**LastOfNew;

    DBUG_ENTER("ExpandTable");

    if (Table->maxp + Table->p < MUL(DirectorySize,SegmentSize))
    {
	/*
	** Locate the bucket to be split
	*/
	OldSegmentDir = DIV(Table->p,SegmentSize);
	OldSegment = Table->Directory[OldSegmentDir];
	OldSegmentIndex = MOD(Table->p,SegmentSize);
	/*
	** Expand address space; if necessary create a new segment
	*/
	NewAddress = Table->maxp + Table->p;
	NewSegmentDir = DIV(NewAddress,SegmentSize);
	NewSegmentIndex = MOD(NewAddress,SegmentSize);
	if (NewSegmentIndex == 0)
	    Table->Directory[NewSegmentDir] = (Segment*)calloc(sizeof(Segment),SegmentSize);
	NewSegment = Table->Directory[NewSegmentDir];
	/*
	** Adjust state variables
	*/
	Table->p++;
	if (Table->p == Table->maxp)
	{
	    Table->maxp <<= 1;	/* Table->maxp *= 2	*/
	    Table->p = 0;
	}
	Table->SegmentCount++;
	/*
	** Relocate records to the new bucket
	*/
	Previous = &OldSegment[OldSegmentIndex];
	Current = *Previous;
	LastOfNew = &NewSegment[NewSegmentIndex];
	*LastOfNew = NULL;
	while (Current != NULL)
	{
	    if (Hash(Current->Key) == NewAddress)
	    {
		/*
		** Attach it to the end of the new chain
		*/
		*LastOfNew = Current;
		/*
		** Remove it from old chain
		*/
		*Previous = Current->Next;
		LastOfNew = &Current->Next;
		Current = Current->Next;
		*LastOfNew = NULL;
	    }
	    else
	    {
		/*
		** leave it on the old chain
		*/
		Previous = &Current->Next;
		Current = Current->Next;
	    }
	}
    }
    DBUG_VOID_RETURN;
}

void
DH_hwalk (errcd, fncp)
int errcd;
void (*fncp)(int, char *, void *);
{
    int	    i,j;
    Segment *s;
    Element *p;

    DBUG_ENTER("DH_hwalk");

    if (Table != NULL) {
	for (i = 0; i < Table->SegmentCount; i++) {
	    /* test probably unnecessary    */
	    if ((s = Table->Directory[i]) != NULL) {
		for (j = 0; j < SegmentSize; j++) {
		    p = s[j];
		    while (p != NULL) {
			(*fncp) (errcd, p->Key, p->Data);  /* call function */
			p = p->Next;
		    }
		}
	    }
	}
    }
}

/************************************************************************/
/*********** DH_hdelete() - delete an DH_entry from the hash table ************/
/************************************************************************/
/* WARNING: frees internal memory, but NOT memory associated with "Key" */
/* and "Data"!								*/
/************************************************************************/

int
DH_hdelete (item)
DH_ENTRY	item;
{
    Address	h;
    Segment	*CurrentSegment;
    int	    SegmentIndex;
    int	    SegmentDir;
    Segment	o,p,q;

    DBUG_ENTER("DH_hdelete");

    assert(Table != NULL);		/* Kinder really than return(NULL);	*/
#ifdef HASH_STATISTICS
    HashAccesses++;
#endif

    h = Hash(item.key);
    SegmentDir = DIV(h,SegmentSize);
    SegmentIndex = MOD(h,SegmentSize);
    /*
     ** valid segment ensured by Hash()
     */
    CurrentSegment = Table->Directory[SegmentDir];
    assert(CurrentSegment != NULL);	/* bad failure if tripped	*/
    o = p = CurrentSegment[SegmentIndex];
    q = p;
    /*
     ** Follow collision chain
     */
    while (q != NULL && strcmp(q->Key,item.key))
    {
	o = p;
	p = q->Next;
	q = p;
    }
    if (q == NULL) {			/* DH_Entry not found */
	DBUG_RETURN (0);
    }

    /* found, now delete it */

    if (o != q) {			/* not the very first element */
	o->Next = q->Next;
    } else {				/* the very first element */
	CurrentSegment[SegmentIndex] = q->Next;
    }
    free ((char*)q);

    DBUG_RETURN (1);
}
