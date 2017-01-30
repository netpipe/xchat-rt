/* Types */
struct cwirc_keyer_state
{
  T_BOOL keyer_initialized;	/* Put 0 here for the first call to the keyer */
  T_BOOL prev_dit;
  T_BOOL prev_dah;
  int last_element;		/* -1 -> nothing, 0 -> dit, 1 -> dah */
  int current_element;		/* -1 -> nothing, 0 -> dit, 1 -> dah */
  T_BOOL insert_inverted_element;
  int iambic_in_element;	/* 0 -> none, 1 -> squeezed, 2 -> released */
  T_BOOL paddles_squeezed_after_mid_element;
  double beep_timeout;
  double mid_element_timeout;
  double element_timeout;
  double delay_timeout;
  int delay_type;		/* 0 -> none, 1 -> inter-char spacing,
				   2 -> inter-word spacing */
};



/* Variables */
extern char cwirc_available_iambic_modes[][10];



/* Prototypes */
T_BOOL cwirc_run_keyer(struct cwirc_keyer_state *is,T_BOOL dit,T_BOOL dah,
	int wpm,int iambicmode,T_BOOL midelementmodeB,T_BOOL ditmemory,
	T_BOOL dahmemory,T_BOOL autocharspacing,T_BOOL autowordspacing,
	int weight,double ticklen);
