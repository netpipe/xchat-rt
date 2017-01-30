/* Prototypes */
int generate_cw_sound_fragment(int sndtype,int keystate_prev,int keystate,
	int samplerate,int nbsamples,int freq,int amplitude,
	unsigned long long offset_keyup,unsigned long long offset_keydown,
	double *samplebuf);
void generate_qrn_sound_fragment(int samplerate,int nbsamples,int amplitude,
				double *samplebuf);
void squelch(int squelch_level,double *sndbuf,int nbsamples,double ticklen);
