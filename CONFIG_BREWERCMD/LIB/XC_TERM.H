#ifndef ECL_TERMHEAD
#define ECL_TERMHEAD

/* Return Values of xc_terminal()*/
#define		TERM_OUT_OF_MEM			-1

/* always start scanning receive character node arrays with member zero */
#define TERM_BASE_STATE    0

/* maximum number of digits in row and column (including NULL terminator) */
#define TERM_MAX_SIZE      4


/* Transmit keypress information structure */
typedef struct xkeypressinfo  {
	char  *buffer;   /* Sequence of chars to Xmit when Associated key is hit */
	int	num_chars; /* Number of characters in the sequence */
	int   padding;   /* padding (in clock ticks) needed for keypress */
}XKeypressInfo;


/* Terminal Structure type */
typedef struct term_info  TERM_INFO;

/* Pointer to Terminal Structure type */
typedef TERM_INFO  *TERM_PTR;

/* Pointer to function (with one argument: pointer to  
   Terminal structure) returning an integer */
typedef int (*TERM_FPTR)(TERM_PTR);


/* Transmit character action structure : Xmit character node */
typedef struct xcharnode  {
	unsigned int		key;					/* key press: scan code + key code */
	TERM_FPTR			action_ptr;			/* Associated function to perform */
}XCharNode;

/* pointer to transmit character node */
typedef XCharNode *XCharNode_Ptr;

/* Transmit Key Information Structure */
typedef struct xcharinfo  {
	int					num_keys;				/* number of transmit key nodes */
	XCharNode			*key_info_array;     /* array of Xmit Char Nodes */
}XCharInfo;

/* Received character action structure : Received character node */
typedef struct rcharnode  {
	int		ch_code;				 			 /* Received character */
	int               next_state_array;  /* index of next array to scan */
	TERM_FPTR			node_action_ptr;   /* function performed if not NULL */
}RCharNode;

/* pointer to recieved character node */
typedef RCharNode *RCharNode_Ptr;

/* Receive Character Information Structure */
typedef struct rcharinfo  {
	int					num_nodes;           /* number of character action nodes */
	RCharNode_Ptr		node_array;          /* array of character action nodes */ 
	TERM_FPTR			default_action_ptr;  /* function performed if not NULL 
                                             and match did not occur       */
}RCharInfo;


/* Terminal structure */
struct term_info{
	int port;      /* Comm port being used */
	int upl_row;  	/* Upper left row of terminal screen */
	int upl_col;   /* Upper left column of terminal screen */
	int max_row;   /* lower right row of terminal screen */
	int max_col;   /* lower right column of terminal screen */
	int terminal_mode;   /* Display Attribute of characters:
								   Underscore, Standout */
	int video_mode;		/* Color or Mono monitor */
	char printer_status;  /* flag to print to screen(=0) / printer (=1) */
	char insert_mode;     /* Indicates if terminal in insert mode: 0=no; 1=yes */
	char keypad_mode;     /* Indicates if keypad is: 1=on (default); 0=off */
	int save_row;        /* Absolute cursor row saved when change scrolling region */
	int save_col;        /* Absolute cursor column saved when change scrolling region */
	int scrl_upl_row;  	/* Upper row of scrolling region */
	int scrl_max_row;    /* Lower row of scrolling region */
	int current_state;   /* Index into correct Receive character node array 
									to scan. Used to receive escape sequences */
	int tab_width;       /* size of Tab: must be >0 */
   unsigned int current_val;  /* current keypress or current incoming char */
	char num[2][TERM_MAX_SIZE]; /* Two numbers, of TERM_MAX_SIZE each */
	int  return_key;	 /* special key to force return */
	char abort_flag;   /* Control Break Support: 0=no; 1=yes */
	char pad_flag;     /* Indicates if padding character is used: 0=no; 1=yes */
	char pad_char;	 	 /* specifies padding character */
	int  ic_delay;     /* inter-character delay used to xmit escape sequences */
	XKeypressInfo *xkeypress_ptr;  /* escape sequence to xmit when user 
												 hits a key : must load before using */
	TERM_FPTR   xmit_funct_ptr; /* function used to transmit keypress characters 
                               and escape sequences */
	TERM_FPTR   idle_time_funct_ptr; /* function used to perform idle time processing */
	XCharInfo  	*key_info_ptr;		/* ptr to user action key info */
	XCharInfo  	*xkey_info_ptr;  		/* ptr to Xmit Char info Nodes */
	RCharInfo  	*rchar_info_array; /* array of Receive Char info Nodes */
};


/* Macro Expansion Definitions for readablity */

/* number of user defined action keys */
#define		TERM_USER_NUM_KEYS(term_ptr)	((term_ptr->key_info_ptr)->num_keys)

/* current keypress or current incoming char */
#define		TERM_CURRENT_VAL(term_ptr) 	(term_ptr->current_val)

/* Array of XCharNodes to match */
#define		TERM_USER_KEY_ARRAY(term_ptr)		((term_ptr->key_info_ptr)->key_info_array)

/* Current Terminal COMM port */
#define		TERM_PORT(term_ptr)		(term_ptr->port)

/* number of Terminal keyboard action keys defined */
#define		TERM_XMIT_NUM_KEYS(term_ptr)	((term_ptr->xkey_info_ptr)->num_keys)

/* Array of XCharNodes to match */
#define		TERM_XMIT_KEY_ARRAY(term_ptr)		((term_ptr->xkey_info_ptr)->key_info_array)

/* Current terminal receive state */
#define     TERM_CURRENT_STATE(term_ptr)     (term_ptr->current_state)

/* number of character nodes present in array for current state */
#define     TERM_REC_NUM_NODES(term_ptr)	((term_ptr->rchar_info_array)[(term_ptr->current_state)].num_nodes)

/* Array of XCharNodes to match */
#define		TERM_REC_NODE_ARRAY(term_ptr) ((term_ptr->rchar_info_array)[(term_ptr->current_state)].node_array)

/* pointer to function to preform if there was no match in the current
   receive character array */
#define		TERM_REC_DEFAULT_ACTION(term_ptr) ((term_ptr->rchar_info_array)[(term_ptr->current_state)].default_action_ptr)


#endif
