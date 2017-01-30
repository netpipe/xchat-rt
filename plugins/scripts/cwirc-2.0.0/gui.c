/* CWirc - X-Chat plugin for sending and receiving raw morse code over IRC
   (c) Pierre-Philippe Coupard - 18/06/2003

   Graphical user interface

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <gtk/gtk.h>

#include "types.h"
#include "gui.h"
#include "common.h"
#include "cwirc.h"
#include "rcfile.h"
#include "grid.h"
#include "io.h"
#include "cwdecoder.h"
#include "extension.h"
#include "keyer.h"
#include "ipc.h"
#include "smeter.xpm"
#include "sidetone.xpm"
#include "straightkey.xpm"
#include "iambickey.xpm"



/* Definitions */
#define SMETER_NEEDLE_RANGE	83	/* degrees */
#define SMETER_NEEDLE_COLOR	"black"
#define SMETER_ARC_DOWN_OFFSET	14	/* Pixels */

#define IAMBIC_RADIO_BUTTON				1
#define MODEB_TOGGLE_BUTTON				2
#define DECODER_RESET_BUTTON				3
#define DITMEMORY_TOGGLE_BUTTON				4
#define DAHMEMORY_TOGGLE_BUTTON				5
#define MIDELEMENTMODEB_TOGGLE_BUTTON			6
#define AUTOCHARSPACING_TOGGLE_BUTTON			7
#define AUTOWORDSPACING_TOGGLE_BUTTON			8
#define INVERTPADDLES_TOGGLE_BUTTON			9
#define MOUSEINPUT_TOGGLE_BUTTON			10
#define KEYINPUT_TOGGLE_BUTTON				11
#define BOTHINPUT_TOGGLE_BUTTON				12
#define SNDDEVOUTPUT_TOGGLE_BUTTON			13
#define SOUNDEROUTPUT_TOGGLE_BUTTON			14
#define BOTHOUTPUT_TOGGLE_BUTTON			15
#define CWSOUND_BEEPS_RADIO_BUTTON			16
#define IO_CONFIG_SAVE_BUTTON				17
#define REPLY_TO_CTCP_TOGGLE_BUTTON			18
#define GIVE_CALLSIGN_IN_CTCP_REPLY_TOGGLE_BUTTON	19
#define GIVE_GRIDSQUARE_IN_CTCP_REPLY_TOGGLE_BUTTON	20
#define GIVE_CWCHANNEL_IN_CTCP_REPLY_TOGGLE_BUTTON	21
#define SEND_CALLSIGN_WITH_CW_TOGGLE_BUTTON		22
#define SEND_GRIDSQUARE_WITH_CW_TOGGLE_BUTTON		23
#define SIMULATE_QRN_TOGGLE_BUTTON			24
#define SIMULATE_SIGNAL_STRENGTH_TOGGLE_BUTTON		25
#define SIMULATE_SPORADICE_TOGGLE_BUTTON		26
#define PERSONAL_INFO_SAVE_BUTTON			27

#define SMETER		1
#define MORSEKEY	2



/* Global variables */
static GtkWidget *wd;
static GtkWidget *popup_wd;
static GdkColormap *colormap;
static GtkWidget *notebook;
static GtkWidget *table;
static GtkWidget *table2;
static GtkWidget *frame;
static GtkWidget *align_wdg;
static GtkWidget *hbox;
static GtkWidget *vbox,*vbox2;
static GtkWidget *label;
static GtkWidget *optionmenu;
static GtkWidget *menu;
static GtkWidget *menuitem;
static GtkWidget *iambic_keyer_zone;
static GtkWidget *keyer_settings_zone;
static GtkWidget *cwsound_zone;
static GtkWidget *key_debounce_zone;
static GtkWidget *sound_device_zone;
static GtkWidget *serial_device_zone;
static GtkWidget *ctcp_params_zone;
static GtkWidget *callsign_zone;
static GtkWidget *propag_sim_zone;
static GtkWidget *simulate_sporadicE_zone;
static GtkWidget *default_signal_strength_zone1;
static GtkWidget *default_signal_strength_zone2;
static GtkAdjustment *adj;
static GtkAdjustment *channel_spinner_adj;
static GtkWidget *spinner;
static GtkWidget *scrollbar;
static GtkWidget *button;
static GtkWidget *midelementmodeB_button;
static GtkWidget *autowordspacing_button;
static GtkWidget *decoder_text_entry;
static GtkWidget *decoder_wpm_label;
static GtkWidget *snddev_text_entry;
static GtkWidget *serialdev_text_entry;
static GtkWidget *callsign_text_entry;
static GtkWidget *gridsquare_text_entry;
static GdkGC *smeter_gc;
static GdkGC *morsekey_gc;
static GdkColor smeter_needle_color;
static GdkPixmap *smeter_bg_pixmap;
static GdkPixmap *sidetone_bg_pixmap;
static GdkPixmap *straightkey_bg_pixmap;
static GdkPixmap *iambickey_bg_pixmap;
static GtkWidget *smeter_drawingarea;
static GtkWidget *morsekey_drawingarea;
static gint smeter_width;
static gint smeter_height;
static gint morsekey_width;
static gint morsekey_height;
static int signal_strength_prev=0;
static int signal_strength=0;
static T_BOOL local_do_mouse_input;
static T_BOOL local_do_key_input;
static T_BOOL local_do_snddev_output;
static T_BOOL local_do_sounder_output;
static int local_cwsound;
static int local_debounce;
static int local_recv_buffering;
static T_BOOL local_send_callsign_with_cw;
static T_BOOL local_send_gridsquare_with_cw;
static T_BOOL local_reply_to_ctcp;
static T_BOOL local_give_callsign_in_ctcp_reply;
static T_BOOL local_give_gridsquare_in_ctcp_reply;
static T_BOOL local_give_cwchannel_in_ctcp_reply;



/* Prototypes */
static gboolean gtk_delete_event(GtkWidget *wdg,GdkEvent *ev,gpointer data);
static void gtk_destroy(GtkWidget *wdg,GdkEvent *ev,gpointer data);
static void current_preset_channel_changed(GtkWidget *wd,gpointer data);
static void channel_changed(GtkWidget *wd,GtkSpinButton *spinner);
static void rxpitch_changed(GtkWidget *wd,GtkRange *scrollbar);
static void txpitch_changed(GtkWidget *wd,GtkRange *scrollbar);
static void squelch_changed(GtkWidget *wd,GtkRange *scrollbar);
static void volume_changed(GtkWidget *wd,GtkRange *scrollbar);
static void qrnlevel_changed(GtkWidget *wd,GtkRange *scrollbar);
static void default_signal_strength_changed(GtkWidget *wd,GtkRange *scrollbar);
static void wpm_changed(GtkWidget *wd,GtkSpinButton *spinner);
static void cw_decoder_language_changed(GtkWidget *wd,gpointer data);
static void dit_weight_changed(GtkWidget *wd,GtkSpinButton *spinner);
static void extension_program_button_clicked(GtkWidget *wd,gpointer data);
static void debounce_changed(GtkWidget *wd,GtkSpinButton *spinner);
static void recv_buffering_changed(GtkWidget *wd,GtkSpinButton *spinner);
static void button_changed(GtkWidget *wd,gpointer button);
static gboolean smeter_configure_event(GtkWidget *wdg,GdkEventConfigure *ev);
static gboolean smeter_expose_event(GtkWidget *wdg,GdkEventExpose *ev);
static gboolean smeter_pressed_event(GtkWidget *wdg,GdkEventButton *ev);
static gboolean morsekey_configure_event(GtkWidget *wdg,GdkEventConfigure *ev);
static gboolean morsekey_expose_event(GtkWidget *wdg,GdkEventExpose *ev);
static gboolean morsekey_pressed_event(GtkWidget *wdg,GdkEventButton *ev);
static gboolean morsekey_released_event(GtkWidget *wdg,GdkEventButton *ev);
static void draw_smeter(int value,T_BOOL full_refresh);
static void calculate_smeter_needle_coordinates(int value,T_U8 *x1,T_U8 *y1,
	T_U8 *x2,T_U8 *y2);
static void error_popup(char *message);
static gint gtk_timeout(gpointer data);
static int extension_shmid;



/* User interface. The function is passed the id of the extension API's shared
   memory block, to pass to an extension program. */
int cwirc_ui(int ext_shmid)
{
  gint argc;
  gchar **argv;
  char buf[10];
  int i;

  extension_shmid=ext_shmid;

  /* Find out the list of available CWirc extensions */
  get_available_cwirc_extensions();

  /* Save some internal values locally, so we can control when they change */
  local_do_mouse_input=sharedmem->do_mouse_input;
  local_do_key_input=sharedmem->do_key_input;
  local_do_snddev_output=sharedmem->do_snddev_output;
  local_do_sounder_output=sharedmem->do_sounder_output;
  local_cwsound=sharedmem->cwsound;
  local_debounce=sharedmem->debounce;
  local_recv_buffering=sharedmem->recv_buffering;
  local_send_callsign_with_cw=sharedmem->send_callsign_with_cw;
  local_send_gridsquare_with_cw=sharedmem->send_gridsquare_with_cw;
  local_reply_to_ctcp=sharedmem->reply_to_ctcp;
  local_give_callsign_in_ctcp_reply=sharedmem->give_callsign_in_ctcp_reply;
  local_give_gridsquare_in_ctcp_reply=sharedmem->give_gridsquare_in_ctcp_reply;
  local_give_cwchannel_in_ctcp_reply=sharedmem->give_cwchannel_in_ctcp_reply;

  /* Initialize GTK */
  argc=1;
  argv=g_new(gchar *,1);
  argv[0]=g_strdup("CWirc");
  gtk_init(&argc,(char ***)&argv);

  /* Create the window for the control panel */
  wd=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(G_OBJECT(wd),"delete_event",G_CALLBACK(gtk_delete_event),
			NULL);
  g_signal_connect(G_OBJECT(wd),"destroy",G_CALLBACK(gtk_destroy),NULL);
  gtk_window_set_title(GTK_WINDOW(wd),"CWirc");
  colormap=gtk_widget_get_colormap(wd);

  /* Create the S-meter and "sidetone" GDK pixmap */
  smeter_bg_pixmap=gdk_pixmap_colormap_create_from_xpm_d(NULL,colormap,
		NULL,NULL,smeter_xpm);
  sidetone_bg_pixmap=gdk_pixmap_colormap_create_from_xpm_d(NULL,colormap,
		NULL,NULL,sidetone_xpm);
  gdk_window_get_size(smeter_bg_pixmap,&smeter_width,&smeter_height);

  /* Create a GC for the S-meter */
  smeter_gc=gdk_gc_new(smeter_bg_pixmap);
  gdk_colormap_alloc_color(colormap,&smeter_needle_color,TRUE,TRUE);
  gdk_color_parse(SMETER_NEEDLE_COLOR,&smeter_needle_color);
  gdk_gc_set_foreground(smeter_gc,&smeter_needle_color);

  /* Create the "straight key icon" and "iambic key icon" GDK pixmaps */
  straightkey_bg_pixmap=gdk_pixmap_colormap_create_from_xpm_d(NULL,colormap,
		NULL,NULL,straightkey_xpm);
  iambickey_bg_pixmap=gdk_pixmap_colormap_create_from_xpm_d(NULL,colormap,
		NULL,NULL,iambickey_xpm);
  gdk_window_get_size(straightkey_bg_pixmap,&morsekey_width,&morsekey_height);

  /* Create a GC for the morse key icon */
  morsekey_gc=gdk_gc_new(straightkey_bg_pixmap);

  /* Create the notebook for the tabbed pages inside the window */
  notebook=gtk_notebook_new();
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook),GTK_POS_TOP);
  gtk_container_add(GTK_CONTAINER(wd),notebook);

  /* Create the first page of the notebook with a table inside */
  label=gtk_label_new("Main");
  table=gtk_table_new(6,2,FALSE);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),table,label);

  /* Create the S-meter drawing area inside an alignment widget, inside a
     frame, inside the table */
  frame=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
  gtk_table_attach(GTK_TABLE(table),frame,0,1,0,1,GTK_SHRINK,GTK_SHRINK,
	7,7);

  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_container_add(GTK_CONTAINER(frame),align_wdg);

  smeter_drawingarea=gtk_drawing_area_new(); 
  gtk_drawing_area_size(GTK_DRAWING_AREA(smeter_drawingarea),
		smeter_width,smeter_height);
  gtk_signal_connect(GTK_OBJECT (smeter_drawingarea),"expose_event",
		(GtkSignalFunc)smeter_expose_event,NULL);
  gtk_signal_connect(GTK_OBJECT(smeter_drawingarea),"configure_event",
		(GtkSignalFunc)smeter_configure_event,NULL);
  gtk_signal_connect(GTK_OBJECT(smeter_drawingarea),"button_press_event",
		(GtkSignalFunc)smeter_pressed_event,NULL);
  gtk_widget_set_events(smeter_drawingarea,GDK_BUTTON_PRESS_MASK);
  gtk_container_add(GTK_CONTAINER(align_wdg),smeter_drawingarea);

  /* Create a vbox to contain the preset channel selector and the channel
     spinner, inside a labeled frame, inside the table */
  frame=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_label(GTK_FRAME(frame),"Channel");
  gtk_frame_set_label_align(GTK_FRAME(frame),0,.5);
  gtk_table_attach(GTK_TABLE(table),frame,1,2,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(frame),vbox);

  /* Create the preset channel selector inside an alignment widget, inside the
     vbox */
  align_wdg=gtk_alignment_new(.5,.75,1,0);
  gtk_box_pack_start(GTK_BOX(vbox),align_wdg,TRUE,TRUE,0);

  optionmenu=gtk_option_menu_new();
  menu=gtk_menu_new();
  strcpy(buf,"PR x");
  for(i=0;i<5;i++)
  {
    buf[3]='1'+i;
    menuitem=gtk_menu_item_new_with_label(buf);
    g_signal_connect(G_OBJECT(menuitem),"activate",
		G_CALLBACK(current_preset_channel_changed),(gpointer)i);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
  }
  gtk_option_menu_set_menu(GTK_OPTION_MENU(optionmenu),menu);
  gtk_option_menu_set_history(GTK_OPTION_MENU(optionmenu),
	sharedmem->currcwchannel);
  gtk_container_add(GTK_CONTAINER(align_wdg),optionmenu);

  /* Create the "Channel" spinner inside an alignment widget, inside the vbox */
  align_wdg=gtk_alignment_new(.5,.25,1,0);
  gtk_box_pack_start(GTK_BOX(vbox),align_wdg,TRUE,TRUE,0);

  channel_spinner_adj=(GtkAdjustment *)gtk_adjustment_new(sharedmem->cwchannel[
	sharedmem->currcwchannel],0,3999,1,10,0);
  spinner=gtk_spin_button_new(channel_spinner_adj,0.1,0);
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinner),TRUE);
  g_signal_connect(G_OBJECT(channel_spinner_adj),"value_changed",
	G_CALLBACK(channel_changed),(gpointer)spinner);
  gtk_container_add(GTK_CONTAINER(align_wdg),spinner);

  /* Create a second table to contain the sound settings inside a frame, inside
     the first table */
  frame=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
  gtk_table_attach(GTK_TABLE(table),frame,2,3,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  table2=gtk_table_new(2,4,FALSE);
  gtk_container_add(GTK_CONTAINER(frame),table2);

  /* Create the "RX pitch", "TX pitch", "Squelch" and "AF gain" scrollbars
     inside alignment widgets, inside the second table */
  for(i=0;i<4;i++)
  {
    align_wdg=gtk_alignment_new(.5,.5,1,0);
    gtk_table_attach(GTK_TABLE(table2),align_wdg,0,1,i,i+1,
  	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,3,0);
  
    adj=(GtkAdjustment *)gtk_adjustment_new(i==0?sharedmem->cwrxpitch:
	i==1?sharedmem->cwtxpitch:i==2?sharedmem->squelch:sharedmem->volume,
	(i<2?-50:0),(i<2?50:100),
	1,5,0);
    scrollbar=gtk_hscrollbar_new(adj);
    gtk_widget_set_size_request(GTK_WIDGET(scrollbar),100,-1);
    gtk_range_set_update_policy(GTK_RANGE(scrollbar),GTK_UPDATE_CONTINUOUS);
    g_signal_connect(G_OBJECT(adj),"value_changed",G_CALLBACK(
	i==0?rxpitch_changed:i==1?txpitch_changed:
	i==2?squelch_changed:volume_changed),
	(gpointer)scrollbar);
    gtk_container_add(GTK_CONTAINER(align_wdg),scrollbar);
  
    align_wdg=gtk_alignment_new(.5,.5,1,0);
    gtk_table_attach(GTK_TABLE(table2),align_wdg,1,2,i,i+1,
  	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,3,0);
  
    label=gtk_label_new(i==0?"RX pitch":i==1?"TX pitch":
			i==2?"Squelch":"AF gain");
    gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
    gtk_container_add(GTK_CONTAINER(align_wdg),label);
  }

  /* Create the straight/iambic radio buttons inside an vbox, inside an
     alignment widget, inside a labelled frame, inside the table. */
  frame=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_label(GTK_FRAME(frame),"Key");
  gtk_frame_set_label_align(GTK_FRAME(frame),0,.5);
  gtk_table_attach(GTK_TABLE(table),frame,3,4,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_container_add(GTK_CONTAINER(frame),align_wdg);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  button=gtk_radio_button_new_with_label(NULL,"straight");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	sharedmem->doiambic?FALSE:TRUE);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  button=gtk_radio_button_new_with_label_from_widget
	(GTK_RADIO_BUTTON(button),"iambic");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	sharedmem->doiambic?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),(gpointer)IAMBIC_RADIO_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  /* Create the WPM spinner inside an alignment widget, inside an hbox, inside
     a labelled frame, inside the table */
  iambic_keyer_zone=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(iambic_keyer_zone),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_label(GTK_FRAME(iambic_keyer_zone),"Keyer");
  gtk_frame_set_label_align(GTK_FRAME(iambic_keyer_zone),0,.5);
  gtk_widget_set_sensitive(iambic_keyer_zone,sharedmem->doiambic?TRUE:FALSE);
  gtk_table_attach(GTK_TABLE(table),iambic_keyer_zone,4,5,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  hbox=gtk_hbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(iambic_keyer_zone),hbox);

  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_box_pack_start(GTK_BOX(hbox),align_wdg,TRUE,TRUE,5);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  label=gtk_label_new("WPM");
  gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
  gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,TRUE,0);

  adj=(GtkAdjustment *)gtk_adjustment_new(sharedmem->wpm,1,60,1,5,0);
  spinner=gtk_spin_button_new(adj,0.1,0);
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinner),FALSE);
  g_signal_connect(G_OBJECT(adj),"value_changed",
	G_CALLBACK(wpm_changed),(gpointer)spinner);
  gtk_box_pack_start(GTK_BOX(vbox),spinner,FALSE,TRUE,0);
  
  /* Create the "morse key icon" drawing area inside an alignment widget, inside
     a frame, inside the table */
  frame=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
  gtk_table_attach(GTK_TABLE(table),frame,5,6,0,1,GTK_SHRINK,GTK_SHRINK,
	7,7);

  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_container_add(GTK_CONTAINER(frame),align_wdg);

  morsekey_drawingarea=gtk_drawing_area_new(); 
  gtk_drawing_area_size(GTK_DRAWING_AREA(morsekey_drawingarea),
		morsekey_width,morsekey_height);
  gtk_signal_connect (GTK_OBJECT (morsekey_drawingarea),"expose_event",
		(GtkSignalFunc)morsekey_expose_event,NULL);
  gtk_signal_connect(GTK_OBJECT(morsekey_drawingarea),"configure_event",
		(GtkSignalFunc)morsekey_configure_event,NULL);
  gtk_signal_connect(GTK_OBJECT(morsekey_drawingarea),"button_press_event",
		(GtkSignalFunc)morsekey_pressed_event,NULL);
  gtk_signal_connect(GTK_OBJECT(morsekey_drawingarea),"button_release_event",
		(GtkSignalFunc)morsekey_released_event,NULL);
  gtk_widget_set_events(morsekey_drawingarea,GDK_BUTTON_PRESS_MASK |
						GDK_BUTTON_RELEASE_MASK);
  gtk_container_add(GTK_CONTAINER(align_wdg),morsekey_drawingarea);

  /* Create the decoder text entry and speedo/reset button inside an hbox,
     inside a frame, inside the table */
  frame=gtk_frame_new(NULL);
  gtk_table_attach(GTK_TABLE(table),frame,0,5,1,2,
	GTK_FILL|GTK_SHRINK,GTK_FILL|GTK_SHRINK,7,7);

  hbox=gtk_hbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(frame),hbox);

  decoder_text_entry=gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(decoder_text_entry),FALSE);
  gtk_box_pack_start(GTK_BOX(hbox),decoder_text_entry,TRUE,TRUE,0);

  button=gtk_button_new();
  decoder_wpm_label=gtk_label_new("? WPM");
  gtk_container_add(GTK_CONTAINER(button),decoder_wpm_label);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),(gpointer)DECODER_RESET_BUTTON);
  gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,TRUE,0);

  /* Create the morse decoder language selector inside the table */
  optionmenu=gtk_option_menu_new();
  menu=gtk_menu_new();
  for(i=0;i<NB_CW_CODE_SETS;i++)
  {
    menuitem=gtk_menu_item_new_with_label(cwirc_cw_table[i].lang_menu_entry);
    g_signal_connect(G_OBJECT(menuitem),"activate",
		G_CALLBACK(cw_decoder_language_changed),(gpointer)i);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
  }
  gtk_option_menu_set_menu(GTK_OPTION_MENU(optionmenu),menu);
  gtk_option_menu_set_history(GTK_OPTION_MENU(optionmenu),
	sharedmem->cwcodeset);
  gtk_table_attach(GTK_TABLE(table),optionmenu,5,6,1,2,
	GTK_FILL|GTK_SHRINK,GTK_FILL|GTK_SHRINK,7,7);

  /* Create the second page of the notebook with a table inside */
  label=gtk_label_new("Keyer settings");
  keyer_settings_zone=gtk_table_new(4,1,FALSE);
  gtk_widget_set_sensitive(keyer_settings_zone,sharedmem->doiambic?TRUE:FALSE);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),keyer_settings_zone,label);

  /* Create the "Iambic mode" radio buttons inside an vbox, inside an alignment
     widget, inside a labelled frame, inside the table */
  frame=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_label(GTK_FRAME(frame),"Iambic mode");
  gtk_frame_set_label_align(GTK_FRAME(frame),0,.5);
  gtk_table_attach(GTK_TABLE(keyer_settings_zone),frame,0,1,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_container_add(GTK_CONTAINER(frame),align_wdg);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  button=gtk_radio_button_new_with_label(NULL,"mode-A");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	sharedmem->iambicmode==0?TRUE:FALSE);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);
  
  button=gtk_radio_button_new_with_label_from_widget
	(GTK_RADIO_BUTTON(button),"mode-B");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	sharedmem->iambicmode==1?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),(gpointer)MODEB_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  /* Create the "Memory" check buttons inside an vbox, inside an alignment
     widget, inside a labelled frame, inside the table */
  frame=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_label(GTK_FRAME(frame),"Memory");
  gtk_frame_set_label_align(GTK_FRAME(frame),0,.5);
  gtk_table_attach(GTK_TABLE(keyer_settings_zone),frame,1,2,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_container_add(GTK_CONTAINER(frame),align_wdg);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  button=gtk_check_button_new_with_label("dit memory");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	sharedmem->do_ditmemory?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),(gpointer)DITMEMORY_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  button=gtk_check_button_new_with_label("dah memory");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	sharedmem->do_dahmemory?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),(gpointer)DAHMEMORY_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  /* Create the "Options" check buttons inside an vbox, inside an alignment
     widget, inside a labelled frame, inside the table */
  frame=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_label(GTK_FRAME(frame),"Options");
  gtk_frame_set_label_align(GTK_FRAME(frame),0,.5);
  gtk_widget_set_sensitive(frame,local_do_snddev_output?TRUE:FALSE);
  gtk_table_attach(GTK_TABLE(keyer_settings_zone),frame,2,3,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_container_add(GTK_CONTAINER(frame),align_wdg);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  midelementmodeB_button=gtk_check_button_new_with_label("mid-element mode-B");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(midelementmodeB_button),
	sharedmem->do_midelementmodeB?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(midelementmodeB_button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)MIDELEMENTMODEB_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),midelementmodeB_button,FALSE,TRUE,0);
  gtk_widget_set_sensitive(midelementmodeB_button,sharedmem->iambicmode==1?
	TRUE:FALSE);

  button=gtk_check_button_new_with_label("auto character spacing");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	sharedmem->do_autocharspacing?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)AUTOCHARSPACING_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  autowordspacing_button=gtk_check_button_new_with_label("auto word spacing");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(autowordspacing_button),
	sharedmem->do_autowordspacing?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(autowordspacing_button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)AUTOWORDSPACING_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),autowordspacing_button,FALSE,TRUE,0);
  gtk_widget_set_sensitive(autowordspacing_button,sharedmem->do_autocharspacing?
	TRUE:FALSE);

  button=gtk_check_button_new_with_label("invert paddles");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	sharedmem->invertpaddles?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)INVERTPADDLES_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  /* Create a vbox to contain the "dit weight" spinner, inside an alignment
     widget, inside the table */
  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_table_attach(GTK_TABLE(keyer_settings_zone),align_wdg,3,4,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  /* Create the "dit weight" spinner inside an alignment widget inside the
     vbox */
  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_box_pack_start(GTK_BOX(vbox),align_wdg,TRUE,TRUE,5);

  vbox2=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox2);

  label=gtk_label_new("Dit weight (%)");
  gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
  gtk_box_pack_start(GTK_BOX(vbox2),label,FALSE,TRUE,0);

  adj=(GtkAdjustment *)gtk_adjustment_new(sharedmem->dit_weight,15,85,1,5,0);
  spinner=gtk_spin_button_new(adj,0.1,0);
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinner),FALSE);
  g_signal_connect(G_OBJECT(adj),"value_changed",
	G_CALLBACK(dit_weight_changed),(gpointer)spinner);
  gtk_box_pack_start(GTK_BOX(vbox2),spinner,FALSE,TRUE,0);

  /* Are there one or more CWirc extensions available ? */
  if(cwirc_extensions[0][0])
  {
    /* Create a third, intersticial page to run them */
    label=gtk_label_new("Extensions");
    table=gtk_table_new(1,1,FALSE);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook),table,label);

    /* Create an hbox to contain the extensions start buttons inside an vbox,
       inside an alignment widget, inside a labelled frame, inside the table */
    frame=gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
    gtk_frame_set_label(GTK_FRAME(frame),"Available extensions");
    gtk_frame_set_label_align(GTK_FRAME(frame),0,.5);
    gtk_table_attach(GTK_TABLE(table),frame,0,1,0,2,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

    align_wdg=gtk_alignment_new(.5,.5,0,0);
    gtk_container_add(GTK_CONTAINER(frame),align_wdg);

    hbox=gtk_hbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(align_wdg),hbox);

    /* Populate the hbox with extension program names */
    for(i=0;i<MAX_CWIRC_EXTENSIONS && cwirc_extensions[i][0];i++)
    {
      button=gtk_button_new_with_label(cwirc_extensions[i]);
      gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(extension_program_button_clicked),(gpointer)i);
      gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,TRUE,10);
    }
  }

  /* Create the third (or fourth) page of the notebook with a table inside */
  label=gtk_label_new("Simulation");
  table=gtk_table_new(2,2,FALSE);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),table,label);

  /* Create a second table containing the QRN simulation settings inside an
     alignment widget, inside a labelled frame, inside the table */
  frame=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_label(GTK_FRAME(frame),"QRN simulation");
  gtk_frame_set_label_align(GTK_FRAME(frame),0,.5);
  gtk_table_attach(GTK_TABLE(table),frame,0,1,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  align_wdg=gtk_alignment_new(.5,.5,0,.5);
  gtk_container_add(GTK_CONTAINER(frame),align_wdg);

  table2=gtk_table_new(2,2,FALSE);
  gtk_container_add(GTK_CONTAINER(align_wdg),table2);

  /* Create the "simulate QRN" check button inside an alignment widget inside
     the second table */
  align_wdg=gtk_alignment_new(.5,.5,1,0);
  gtk_table_attach(GTK_TABLE(table2),align_wdg,0,2,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,3,0);

  button=gtk_check_button_new_with_label("simulate QRN");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	sharedmem->simulate_qrn?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)SIMULATE_QRN_TOGGLE_BUTTON);
  gtk_container_add(GTK_CONTAINER(align_wdg),button);

  /* Create the "QRN level" scrollbar inside an alignment widget inside the
     second table */
  align_wdg=gtk_alignment_new(.5,.5,1,0);
  gtk_table_attach(GTK_TABLE(table2),align_wdg,0,1,1,2,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,3,0);

  adj=(GtkAdjustment *)gtk_adjustment_new(sharedmem->qrnlevel,0,100,1,5,0);
  scrollbar=gtk_hscrollbar_new(adj);
  gtk_widget_set_size_request(GTK_WIDGET(scrollbar),100,-1);
  gtk_range_set_update_policy(GTK_RANGE(scrollbar),GTK_UPDATE_CONTINUOUS);
  g_signal_connect(G_OBJECT(adj),"value_changed",
	G_CALLBACK(qrnlevel_changed),(gpointer)scrollbar);
  gtk_container_add(GTK_CONTAINER(align_wdg),scrollbar);

  align_wdg=gtk_alignment_new(.5,.5,1,0);
  gtk_table_attach(GTK_TABLE(table2),align_wdg,1,2,1,2,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,3,0);

  label=gtk_label_new("QRN level");
  gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
  gtk_container_add(GTK_CONTAINER(align_wdg),label);

  /* Create a second table containing the propagation simulation settings
     inside an alignment widget, inside a labelled frame, inside the table */
  propag_sim_zone=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(propag_sim_zone),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_label(GTK_FRAME(propag_sim_zone),sharedmem->gridsquare[0]?
	"Propagation simulation":
	"Propagation simulation [needs the grid square to be set]");
  gtk_frame_set_label_align(GTK_FRAME(propag_sim_zone),0,.5);
  gtk_widget_set_sensitive(propag_sim_zone,sharedmem->gridsquare[0]?TRUE:FALSE);
  gtk_table_attach(GTK_TABLE(table),propag_sim_zone,1,2,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  align_wdg=gtk_alignment_new(.5,.5,0,.5);
  gtk_container_add(GTK_CONTAINER(propag_sim_zone),align_wdg);

  table2=gtk_table_new(2,3,FALSE);
  gtk_container_add(GTK_CONTAINER(align_wdg),table2);

  /* Create the "simulate signal strength ..." check button inside an alignment
     widget inside the second table */
  align_wdg=gtk_alignment_new(.5,.5,1,0);
  gtk_table_attach(GTK_TABLE(table2),align_wdg,0,2,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,3,0);

  button=gtk_check_button_new_with_label(
	"simulate signal strength for signals with grid squares");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	sharedmem->simulate_signal_strength?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)SIMULATE_SIGNAL_STRENGTH_TOGGLE_BUTTON);
  gtk_container_add(GTK_CONTAINER(align_wdg),button);

  /* Create the "simulate sporadic-E ..." check button inside an alignment
     widget inside the second table */
  simulate_sporadicE_zone=gtk_alignment_new(.5,.5,1,0);
  gtk_widget_set_sensitive(simulate_sporadicE_zone,
	sharedmem->simulate_signal_strength?TRUE:FALSE);
  gtk_table_attach(GTK_TABLE(table2),simulate_sporadicE_zone,0,2,1,2,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,3,0);

  button=gtk_check_button_new_with_label(
	"simulate sporadic-E for weak signals");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	sharedmem->simulate_sporadicE?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)SIMULATE_SPORADICE_TOGGLE_BUTTON);
  gtk_container_add(GTK_CONTAINER(simulate_sporadicE_zone),button);

  /* Create the "default signal strength ..." scrollbar inside an alignment
     widget inside the second table */
  default_signal_strength_zone1=gtk_alignment_new(.5,.5,1,0);
  gtk_widget_set_sensitive(default_signal_strength_zone1,
	sharedmem->simulate_signal_strength?TRUE:FALSE);
  gtk_table_attach(GTK_TABLE(table2),default_signal_strength_zone1,0,1,2,3,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,3,0);

  adj=(GtkAdjustment *)gtk_adjustment_new(sharedmem->default_signal_strength,
	0,100,1,5,0);
  scrollbar=gtk_hscrollbar_new(adj);
  gtk_widget_set_size_request(GTK_WIDGET(scrollbar),100,-1);
  gtk_range_set_update_policy(GTK_RANGE(scrollbar),GTK_UPDATE_CONTINUOUS);
  g_signal_connect(G_OBJECT(adj),"value_changed",
	G_CALLBACK(default_signal_strength_changed),(gpointer)scrollbar);
  gtk_container_add(GTK_CONTAINER(default_signal_strength_zone1),scrollbar);

  default_signal_strength_zone2=gtk_alignment_new(.5,.5,1,0);
  gtk_widget_set_sensitive(default_signal_strength_zone2,
	sharedmem->simulate_signal_strength?TRUE:FALSE);
  gtk_table_attach(GTK_TABLE(table2),default_signal_strength_zone2,1,2,2,3,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,3,0);

  label=gtk_label_new(
	"default signal strength for signals without grid squares");
  gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
  gtk_container_add(GTK_CONTAINER(default_signal_strength_zone2),label);

  /* Create the fourth (or fifth) page of the notebook with a table inside */
  label=gtk_label_new("Personal info");
  table=gtk_table_new(4,1,FALSE);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),table,label);

  /* Create a zone to contain the CTCP buttons inside an vbox, inside an
     alignment widget, inside a labelled frame, inside the table */
  frame=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_label(GTK_FRAME(frame),"CTCP");
  gtk_frame_set_label_align(GTK_FRAME(frame),0,.5);
  gtk_table_attach(GTK_TABLE(table),frame,0,1,0,2,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_container_add(GTK_CONTAINER(frame),align_wdg);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  /* Create the "reply to CTCP CWIRC queries" check button inside the vbox */
  button=gtk_check_button_new_with_label("reply to CTCP CWIRC queries");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	local_reply_to_ctcp?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)REPLY_TO_CTCP_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  /* Create the "send callsign in CTCP reply", "send grid square in CTCP reply"
     and "send current channel in CTCP reply" check buttons inside a second
     vbox, inside the vbox */
  ctcp_params_zone=gtk_vbox_new(FALSE,0);
  gtk_widget_set_sensitive(ctcp_params_zone,local_reply_to_ctcp?TRUE:FALSE);
  gtk_box_pack_start(GTK_BOX(vbox),ctcp_params_zone,FALSE,TRUE,0);

  button=gtk_check_button_new_with_label("send callsign in CTCP reply");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	local_give_callsign_in_ctcp_reply?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)GIVE_CALLSIGN_IN_CTCP_REPLY_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(ctcp_params_zone),button,FALSE,TRUE,0);

  button=gtk_check_button_new_with_label("send grid square in CTCP reply");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	local_give_gridsquare_in_ctcp_reply?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)GIVE_GRIDSQUARE_IN_CTCP_REPLY_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(ctcp_params_zone),button,FALSE,TRUE,0);

  button=gtk_check_button_new_with_label("send current channel in CTCP reply");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	local_give_cwchannel_in_ctcp_reply?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)GIVE_CWCHANNEL_IN_CTCP_REPLY_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(ctcp_params_zone),button,FALSE,TRUE,0);

  /* Create a zone to contain the "send [..] with CW" buttons inside an vbox,
     inside an alignment widget, inside a labelled frame, inside the table */
  frame=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_label(GTK_FRAME(frame),"Additional info in signal");
  gtk_frame_set_label_align(GTK_FRAME(frame),0,.5);
  gtk_table_attach(GTK_TABLE(table),frame,1,2,0,2,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_container_add(GTK_CONTAINER(frame),align_wdg);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  /* Create the "send callsign with CW" check button inside the vbox */
  button=gtk_check_button_new_with_label("send callsign with CW");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	local_send_callsign_with_cw?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)SEND_CALLSIGN_WITH_CW_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  /* Create the "send grid square with CW" check button inside the vbox */
  button=gtk_check_button_new_with_label("send grid square with CW");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	local_send_gridsquare_with_cw?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)SEND_GRIDSQUARE_WITH_CW_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  /* Create a vbox to contain the callsign and grid square text entries, inside
     an alignment widget, inside the table */
  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_table_attach(GTK_TABLE(table),align_wdg,2,3,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  /* Create the callsign entry, inside an alignment widget, inside the vbox */
  callsign_zone=gtk_alignment_new(.5,.5,0,0);
  gtk_widget_set_sensitive(callsign_zone,local_send_callsign_with_cw ||
	local_give_callsign_in_ctcp_reply?TRUE:FALSE);
  gtk_box_pack_start(GTK_BOX(vbox),callsign_zone,TRUE,TRUE,5);

  vbox2=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(callsign_zone),vbox2);

  label=gtk_label_new("Callsign");
  gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
  gtk_box_pack_start(GTK_BOX(vbox2),label,FALSE,TRUE,0);

  callsign_text_entry=gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(callsign_text_entry),TRUE);
  gtk_entry_set_max_length(GTK_ENTRY(callsign_text_entry),MAX_NICK_SIZE-1);
  gtk_entry_set_text(GTK_ENTRY(callsign_text_entry),sharedmem->callsign);
  gtk_box_pack_start(GTK_BOX(vbox2),callsign_text_entry,FALSE,TRUE,0);

  /* Create the grid square entry, inside an alignment widget, inside the vbox*/
  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_box_pack_start(GTK_BOX(vbox),align_wdg,TRUE,TRUE,5);

  vbox2=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox2);

  label=gtk_label_new("Grid square");
  gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
  gtk_box_pack_start(GTK_BOX(vbox2),label,FALSE,TRUE,0);

  gridsquare_text_entry=gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(gridsquare_text_entry),TRUE);
  gtk_entry_set_max_length(GTK_ENTRY(gridsquare_text_entry),
	MAX_GRIDSQUARE_SIZE-1);
  gtk_entry_set_text(GTK_ENTRY(gridsquare_text_entry),sharedmem->gridsquare);
  gtk_box_pack_start(GTK_BOX(vbox2),gridsquare_text_entry,FALSE,TRUE,0);

  /* Create a save/change button inside an alignment widget, inside the table */
  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_table_attach(GTK_TABLE(table),align_wdg,3,4,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  button=gtk_button_new_with_label("Change/Save");
  g_signal_connect(G_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),(gpointer)PERSONAL_INFO_SAVE_BUTTON);
  gtk_container_add(GTK_CONTAINER(align_wdg),button);

  /* Create the fifth (sixth) page of the notebook with a table inside */
  label=gtk_label_new("I/O configuration");
  table=gtk_table_new(6,1,FALSE);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),table,label);

  /* Create the "CW input" radio buttons inside an vbox, inside an alignment
     widget, inside a labelled frame, inside the table */
  frame=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_label(GTK_FRAME(frame),"CW input");
  gtk_frame_set_label_align(GTK_FRAME(frame),0,.5);
  gtk_table_attach(GTK_TABLE(table),frame,0,1,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_container_add(GTK_CONTAINER(frame),align_wdg);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  button=gtk_radio_button_new_with_label(NULL,"mouse");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	local_do_mouse_input && !local_do_key_input?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),(gpointer)MOUSEINPUT_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);
  
  button=gtk_radio_button_new_with_label_from_widget
	(GTK_RADIO_BUTTON(button),"real key");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	!local_do_mouse_input && local_do_key_input?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),(gpointer)KEYINPUT_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  button=gtk_radio_button_new_with_label_from_widget
	(GTK_RADIO_BUTTON(button),"both");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	local_do_mouse_input && local_do_key_input?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),(gpointer)BOTHINPUT_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  /* Create the "CW output" radio buttons inside an vbox, inside an alignment
     widget, inside a labelled frame, inside the table */
  frame=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_label(GTK_FRAME(frame),"CW output");
  gtk_frame_set_label_align(GTK_FRAME(frame),0,.5);
  gtk_table_attach(GTK_TABLE(table),frame,1,2,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_container_add(GTK_CONTAINER(frame),align_wdg);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  button=gtk_radio_button_new_with_label(NULL,"soundcard");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	local_do_snddev_output && !local_do_sounder_output?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)SNDDEVOUTPUT_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  button=gtk_radio_button_new_with_label_from_widget
	(GTK_RADIO_BUTTON(button),"sounder");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	!local_do_snddev_output && local_do_sounder_output?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)SOUNDEROUTPUT_TOGGLE_BUTTON);
#ifndef LINUX
  gtk_widget_set_sensitive(button,FALSE);
#endif
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  button=gtk_radio_button_new_with_label_from_widget
	(GTK_RADIO_BUTTON(button),"both");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	local_do_snddev_output && local_do_sounder_output?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),(gpointer)BOTHOUTPUT_TOGGLE_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  /* Create the "CW sound" radio buttons inside an vbox, inside an alignment
     widget, inside a labelled frame, inside the table */
  cwsound_zone=gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(cwsound_zone),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_label(GTK_FRAME(cwsound_zone),"CW sound");
  gtk_frame_set_label_align(GTK_FRAME(cwsound_zone),0,.5);
  gtk_widget_set_sensitive(cwsound_zone,local_do_snddev_output?TRUE:FALSE);
  gtk_table_attach(GTK_TABLE(table),cwsound_zone,2,3,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_container_add(GTK_CONTAINER(cwsound_zone),align_wdg);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  button=gtk_radio_button_new_with_label(NULL,"beeps");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	local_cwsound==0?TRUE:FALSE);
  gtk_signal_connect(GTK_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),
		(gpointer)CWSOUND_BEEPS_RADIO_BUTTON);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  button=gtk_radio_button_new_with_label_from_widget
	(GTK_RADIO_BUTTON(button),"sounder clicks");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
	local_cwsound==1?TRUE:FALSE);
  gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);

  /* Create a vbox to contain the "debounce" and "recv_buffering" spinners,
     inside an alignment widget, inside the table */
  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_table_attach(GTK_TABLE(table),align_wdg,3,4,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  /* Create the "debounce" spinner inside an alignment widget inside the vbox */
  key_debounce_zone=gtk_alignment_new(.5,.5,0,0);
  gtk_widget_set_sensitive(key_debounce_zone,local_do_key_input?TRUE:FALSE);
  gtk_box_pack_start(GTK_BOX(vbox),key_debounce_zone,TRUE,TRUE,5);

  vbox2=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(key_debounce_zone),vbox2);

  label=gtk_label_new("Key debounce");
  gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
  gtk_box_pack_start(GTK_BOX(vbox2),label,FALSE,TRUE,0);

  adj=(GtkAdjustment *)gtk_adjustment_new(local_debounce,1,15,1,1,0);
  spinner=gtk_spin_button_new(adj,0.1,0);
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinner),FALSE);
  g_signal_connect(G_OBJECT(adj),"value_changed",
	G_CALLBACK(debounce_changed),(gpointer)spinner);
  gtk_box_pack_start(GTK_BOX(vbox2),spinner,FALSE,TRUE,0);

  /* Create the "recv buffering" spinner inside an alignment widget inside the
     vbox */
  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_box_pack_start(GTK_BOX(vbox),align_wdg,TRUE,TRUE,5);

  vbox2=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox2);

  label=gtk_label_new("Recv buffering");
  gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
  gtk_box_pack_start(GTK_BOX(vbox2),label,FALSE,TRUE,0);

  adj=(GtkAdjustment *)gtk_adjustment_new(local_recv_buffering,
	100,3000,100,500,0);
  spinner=gtk_spin_button_new(adj,0.1,0);
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinner),FALSE);
  g_signal_connect(G_OBJECT(adj),"value_changed",
	G_CALLBACK(recv_buffering_changed),(gpointer)spinner);
  gtk_box_pack_start(GTK_BOX(vbox2),spinner,FALSE,TRUE,0);

  /* Create a vbox to contain the sound device and serial device lines, inside
     an alignment widget, inside the table */
  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_table_attach(GTK_TABLE(table),align_wdg,4,5,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  vbox=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(align_wdg),vbox);

  /* Create the sound device entry, inside an alignment widget, inside the
     vbox */
  sound_device_zone=gtk_alignment_new(.5,.5,0,0);
  gtk_widget_set_sensitive(sound_device_zone,local_do_snddev_output?
	TRUE:FALSE);
  gtk_box_pack_start(GTK_BOX(vbox),sound_device_zone,TRUE,TRUE,5);

  vbox2=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(sound_device_zone),vbox2);

  label=gtk_label_new("Sound device");
  gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
  gtk_box_pack_start(GTK_BOX(vbox2),label,FALSE,TRUE,0);

  snddev_text_entry=gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(snddev_text_entry),TRUE);
  gtk_entry_set_max_length(GTK_ENTRY(snddev_text_entry),FILENAME_MAX-1);
  gtk_entry_set_text(GTK_ENTRY(snddev_text_entry),sharedmem->snddev);
  gtk_box_pack_start(GTK_BOX(vbox2),snddev_text_entry,FALSE,TRUE,0);

  /* Create the serial device entry, inside an alignment widget, inside the
     vbox */
  serial_device_zone=gtk_alignment_new(.5,.5,0,0);
  gtk_widget_set_sensitive(serial_device_zone,local_do_key_input ||
	local_do_sounder_output?TRUE:FALSE);
  gtk_box_pack_start(GTK_BOX(vbox),serial_device_zone,TRUE,TRUE,5);

  vbox2=gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(serial_device_zone),vbox2);

  label=gtk_label_new("Serial device");
  gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
  gtk_box_pack_start(GTK_BOX(vbox2),label,FALSE,TRUE,0);

  serialdev_text_entry=gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(serialdev_text_entry),TRUE);
  gtk_entry_set_max_length(GTK_ENTRY(serialdev_text_entry),FILENAME_MAX-1);
  gtk_entry_set_text(GTK_ENTRY(serialdev_text_entry),sharedmem->serialdev);
  gtk_box_pack_start(GTK_BOX(vbox2),serialdev_text_entry,FALSE,TRUE,0);

  /* Create a save/change button inside an alignment widget, inside the table */
  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_table_attach(GTK_TABLE(table),align_wdg,5,6,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  button=gtk_button_new_with_label("Change/Save");
  g_signal_connect(G_OBJECT(button),"clicked",
		G_CALLBACK(button_changed),(gpointer)IO_CONFIG_SAVE_BUTTON);
  gtk_container_add(GTK_CONTAINER(align_wdg),button);

  /* Create the sixth (or seventh) page of the notebook with a table inside */
  label=gtk_label_new("About");
  table=gtk_table_new(2,1,FALSE);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),table,label);

  /* Create the title/author label inside the table */
  label=gtk_label_new("<span size=\"x-large\">CWirc " VERSION "</span>\n"
			"<span size=\"large\">by F8EJF</span>");
  gtk_label_set_use_markup(GTK_LABEL(label),TRUE);
  gtk_table_attach(GTK_TABLE(table),label,1,2,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  /* Create the general information label inside the table */
  label=gtk_label_new("CWirc - IRC Morse code chat application\n"
		"Version: " VERSION ", build date: " __DATE__ "\n"
		"(c) Pierre-Philippe Coupard <pcoupard@skynet.be>\n\n"
		"This program is distributed under the terms of the GNU "
				"General Public License");
  gtk_table_attach(GTK_TABLE(table),label,0,1,0,1,
	GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,7,7);

  /* Make the window non-resizable */
  gtk_window_set_policy(GTK_WINDOW(wd),FALSE,FALSE,FALSE);

  /* Show all the widgets and the window */
  gtk_widget_show_all(wd);

  /* Add the periodic timeout */
  gtk_timeout_add(20,gtk_timeout,0);

  /* Enter the main GTK loop */
  gtk_main();

  /* Save the current settings in the configuration file */
  cwirc_save_rcfile(RCFILE);

  return(0);
}



/* GTK callback to handle the "delete_event" signals */
static gboolean gtk_delete_event(GtkWidget *wdg,GdkEvent *ev,gpointer data)
{
  /* Stop the frontend */
  sharedmem->stop_frontend=1;

  return(FALSE);
}



/* GTK callback to handle the "destroy" signals */
static void gtk_destroy(GtkWidget *wdg,GdkEvent *ev,gpointer data)
{
  /* Stop the frontend */
  sharedmem->stop_frontend=1;

  gtk_main_quit();
}



/* GTK callback for when a new preset channel is selected. */
static void current_preset_channel_changed(GtkWidget *wd,gpointer data)
{
  int i;
  
  /* Did the user select another preset channel ? */
  if((int)data!=sharedmem->currcwchannel)
  {
    /* Acquire the semaphore */
    if(!cwirc_sem_P(sharedmem->semid,SEM_ST))
    {
      /* If the actual channel changed, clear the reception buffer */
      if(sharedmem->cwchannel[(int)data]!=sharedmem->cwchannel[
		sharedmem->currcwchannel])
      {
        for(i=0;i<MAX_SENDERS;i++)
          sharedmem->sender[i].name[0]=0;
      }

      /* Change the current preset channel and update the spinner's value */
      sharedmem->currcwchannel=(int)data;
      gtk_adjustment_set_value(GTK_ADJUSTMENT(channel_spinner_adj),
		sharedmem->cwchannel[sharedmem->currcwchannel]);

      /* Release the semaphore */
      cwirc_sem_V(sharedmem->semid,SEM_ST);
    }
  }
}



/* GTK callback for when the value of the "Channel" spinner changes */
static void channel_changed(GtkWidget *wd,GtkSpinButton *spinner)
{
  int new_chan;
  int i;

  new_chan=gtk_spin_button_get_value_as_int(spinner);

  /* If the channel has changed, clear the reception buffer */
  if(new_chan!=sharedmem->cwchannel[sharedmem->currcwchannel])
  {
    /* Acquire the semaphore */
    if(!cwirc_sem_P(sharedmem->semid,SEM_ST))
    {
      for(i=0;i<MAX_SENDERS;i++)
        sharedmem->sender[i].name[0]=0;

      /* Change the channel */
      sharedmem->cwchannel[sharedmem->currcwchannel]=new_chan;

      /* Release the semaphore */
      cwirc_sem_V(sharedmem->semid,SEM_ST);
    }
  }
}



/* GTK callback for when the value of the "RX pitch" scrollbar changes */
static void rxpitch_changed(GtkWidget *wd,GtkRange *scrollbar)
{
  sharedmem->cwrxpitch=gtk_adjustment_get_value(
		gtk_range_get_adjustment(scrollbar));
}



/* GTK callback for when the value of the "TX pitch" scrollbar changes */
static void txpitch_changed(GtkWidget *wd,GtkRange *scrollbar)
{
  sharedmem->cwtxpitch=gtk_adjustment_get_value(
		gtk_range_get_adjustment(scrollbar));
}



/* GTK callback for when the value of the "Squelch" scrollbar changes */
static void squelch_changed(GtkWidget *wd,GtkRange *scrollbar)
{
  sharedmem->squelch=gtk_adjustment_get_value(
		gtk_range_get_adjustment(scrollbar));
}



/* GTK callback for when the value of the "Volume" scrollbar changes */
static void volume_changed(GtkWidget *wd,GtkRange *scrollbar)
{
  sharedmem->volume=gtk_adjustment_get_value(
		gtk_range_get_adjustment(scrollbar));
}



/* GTK callback for when the value of the "QRN level" scrollbar changes */
static void qrnlevel_changed(GtkWidget *wd,GtkRange *scrollbar)
{
  sharedmem->qrnlevel=gtk_adjustment_get_value(
		gtk_range_get_adjustment(scrollbar));
}



/* GTK callback for when the value of the "default signal strength ..."
   scrollbar changes */
static void default_signal_strength_changed(GtkWidget *wd,GtkRange *scrollbar)
{
  sharedmem->default_signal_strength=gtk_adjustment_get_value(
		gtk_range_get_adjustment(scrollbar));
}



/* GTK callback for when the value of the "WPM" spinner changes */
static void wpm_changed(GtkWidget *wd,GtkSpinButton *spinner)
{
  sharedmem->wpm=gtk_spin_button_get_value_as_int(spinner);
}



/* GTK callback for when another CW decoder language is selected. */
static void cw_decoder_language_changed(GtkWidget *wd,gpointer data)
{
  if((int)data!=sharedmem->cwcodeset)
    sharedmem->cwcodeset=(int)data;
}



/* GTK callback for when the value of the "dit weight" spinner changes */
static void dit_weight_changed(GtkWidget *wd,GtkSpinButton *spinner)
{
  sharedmem->dit_weight=gtk_spin_button_get_value_as_int(spinner);
}



/* GTK callback for when an extension program has been selected */
static void extension_program_button_clicked(GtkWidget *wd,gpointer data)
{
  char *errmsg;
  
  if((errmsg=exec_extension_program(cwirc_extensions[(int)data],
	extension_shmid))!=NULL)
    error_popup(errmsg);
}



/* GTK callback for when the value of the "debounce" spinner changes */
static void debounce_changed(GtkWidget *wd,GtkSpinButton *spinner)
{
  local_debounce=gtk_spin_button_get_value_as_int(spinner);
}



/* GTK callback for when the value of the "recv_buffering" spinner changes */
static void recv_buffering_changed(GtkWidget *wd,GtkSpinButton *spinner)
{
  local_recv_buffering=gtk_spin_button_get_value_as_int(spinner);
}



/* GTK callback when any of the panel's simple, toggle or radio button change */
static void button_changed(GtkWidget *wd,gpointer button)
{
  char buf[MAX_NICK_SIZE];
  int i,j,k;
  T_BOOL button_active=0;

  /* Get the button's state */
  if((int)button!=IO_CONFIG_SAVE_BUTTON &&
	(int)button!=PERSONAL_INFO_SAVE_BUTTON &&
	(int)button!=DECODER_RESET_BUTTON)
    button_active=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wd))?1:0;

  /* What button was clicked ? */
  switch((int)button)
  {
  case IAMBIC_RADIO_BUTTON:
    /* The "iambic" toggle changed state. Since "iambic" is the negation of
       "straight" in our toggle buttons pair, we don't watch "straight". */
    sharedmem->doiambic=button_active;
    gtk_widget_set_sensitive(iambic_keyer_zone,button_active?TRUE:FALSE);
    gtk_widget_set_sensitive(keyer_settings_zone,button_active?TRUE:FALSE);
    gdk_draw_drawable(morsekey_drawingarea->window,morsekey_gc,
		button_active?iambickey_bg_pixmap:straightkey_bg_pixmap,
		0,0,0,0,morsekey_width,morsekey_height);
    break;

  case DECODER_RESET_BUTTON:
    sharedmem->reset_decoder=1;
    break;

  case MODEB_TOGGLE_BUTTON:
    /* The "mode-B" radio button changes state. Since "mode-A" is the negation
       of "mode-B" in our toggle buttons pair, we don't watch "mode-A" */
    sharedmem->iambicmode=button_active?1:0;
    gtk_widget_set_sensitive(midelementmodeB_button,button_active?TRUE:FALSE);
    break;

  case DITMEMORY_TOGGLE_BUTTON:
    /* The "dit memory" toggle changed state */
    sharedmem->do_ditmemory=button_active;
    break;

  case DAHMEMORY_TOGGLE_BUTTON:
    /* The "dah memory" toggle changed state */
    sharedmem->do_dahmemory=button_active;
    break;

  case MIDELEMENTMODEB_TOGGLE_BUTTON:
    /* The "mid-element mode-B" toggle changed state */
    sharedmem->do_midelementmodeB=button_active;
    break;

  case AUTOCHARSPACING_TOGGLE_BUTTON:
    /* The "auto character spacing" toggle changed state. */
    sharedmem->do_autocharspacing=button_active;
    gtk_widget_set_sensitive(autowordspacing_button,button_active?TRUE:FALSE);
    break;

  case AUTOWORDSPACING_TOGGLE_BUTTON:
    /* The "auto word spacing" toggle changed state. */
    sharedmem->do_autowordspacing=button_active;
    break;

  case INVERTPADDLES_TOGGLE_BUTTON:
    /* The "invert paddles" toggle changed state. */
    sharedmem->invertpaddles=button_active;
    break;

  case MOUSEINPUT_TOGGLE_BUTTON:
    /* The "mouse" toggle changed state. */
    if(button_active)
    {
      local_do_mouse_input=1;
      local_do_key_input=0;
      gtk_widget_set_sensitive(key_debounce_zone,FALSE);
      gtk_widget_set_sensitive(serial_device_zone,local_do_sounder_output?
  	TRUE:FALSE);
    }
    break;

  case KEYINPUT_TOGGLE_BUTTON:
    /* The "real key" toggle changed state. */
    if(button_active)
    {
      local_do_mouse_input=0;
      local_do_key_input=1;
      gtk_widget_set_sensitive(key_debounce_zone,TRUE);
      gtk_widget_set_sensitive(serial_device_zone,TRUE);
    }
    break;

  case BOTHINPUT_TOGGLE_BUTTON:
    /* The "both" (input) toggle changed state. */
    if(button_active)
    {
      local_do_mouse_input=1;
      local_do_key_input=1;
      gtk_widget_set_sensitive(key_debounce_zone,TRUE);
      gtk_widget_set_sensitive(serial_device_zone,TRUE);
    }
    break;

  case SNDDEVOUTPUT_TOGGLE_BUTTON:
    /* The "soundcard" toggle changed state. */
    if(button_active)
    {
      local_do_snddev_output=1;
      local_do_sounder_output=0;
      gtk_widget_set_sensitive(sound_device_zone,TRUE);
      gtk_widget_set_sensitive(serial_device_zone,local_do_key_input?
		TRUE:FALSE);
      gtk_widget_set_sensitive(cwsound_zone,TRUE);
    }
    break;

  case SOUNDEROUTPUT_TOGGLE_BUTTON:
    /* The "sounder" toggle changed state. */
    if(button_active)
    {
      local_do_snddev_output=0;
      local_do_sounder_output=1;
      gtk_widget_set_sensitive(sound_device_zone,FALSE);
      gtk_widget_set_sensitive(serial_device_zone,TRUE);
      gtk_widget_set_sensitive(cwsound_zone,FALSE);
    }
    break;

  case BOTHOUTPUT_TOGGLE_BUTTON:
    /* The "both" (output) toggle changed state. */
    if(button_active)
    {
      local_do_snddev_output=1;
      local_do_sounder_output=1;
      gtk_widget_set_sensitive(sound_device_zone,TRUE);
      gtk_widget_set_sensitive(serial_device_zone,TRUE);
      gtk_widget_set_sensitive(cwsound_zone,TRUE);
    }
    break;

  case CWSOUND_BEEPS_RADIO_BUTTON:
    /* The "beeps" toggle changed state. Since "sounder clicks" is the negation
       of "beeps" in our toggle buttons pair, we don't watch "sounder clicks".*/
    local_cwsound=button_active?0:1;
    break;

  case IO_CONFIG_SAVE_BUTTON:
    /* The "Save/Change" button (I/O config) was pressed : */
    /* Acquire semaphore to prevent reconfiguring I/O process while it works*/
    cwirc_sem_P(sharedmem->semid,SEM_IO_PROCESS_WORKING);
  
    /* Install the new settings "live" */
    sharedmem->do_mouse_input=local_do_mouse_input;
    sharedmem->do_key_input=local_do_key_input;
    sharedmem->do_snddev_output=local_do_snddev_output;
    sharedmem->cwsound=local_cwsound;
    sharedmem->do_sounder_output=local_do_sounder_output;
    sharedmem->debounce=local_debounce;
    sharedmem->recv_buffering=local_recv_buffering;
    strncpy(sharedmem->snddev,
  	gtk_entry_get_text(GTK_ENTRY(snddev_text_entry)),FILENAME_MAX);
    sharedmem->snddev[FILENAME_MAX-1]=0;
    strncpy(sharedmem->serialdev,
  	gtk_entry_get_text(GTK_ENTRY(serialdev_text_entry)),FILENAME_MAX);
    sharedmem->serialdev[FILENAME_MAX-1]=0;
  
    /* Tell the I/O process to reconfigure itself */
    sharedmem->reconfigure_io_process=1;
  
    /* Release semaphore to prevent reconfiguring I/O process while it works*/
    cwirc_sem_V(sharedmem->semid,SEM_IO_PROCESS_WORKING);
  
    /* Save the settings in the configuration file */
    cwirc_save_rcfile(RCFILE);
    break;

  case REPLY_TO_CTCP_TOGGLE_BUTTON:
    /* The "reply to CTCP queries" button was pressed */
    local_reply_to_ctcp=button_active;
    gtk_widget_set_sensitive(ctcp_params_zone,button_active?TRUE:FALSE);
    if(button_active)
      gtk_widget_set_sensitive(callsign_zone,local_give_callsign_in_ctcp_reply?
  	TRUE:local_send_callsign_with_cw?TRUE:FALSE);
    else
      gtk_widget_set_sensitive(callsign_zone,local_send_callsign_with_cw?
  	TRUE:FALSE);
    break;

  case GIVE_CALLSIGN_IN_CTCP_REPLY_TOGGLE_BUTTON:
    /* The "send callsign in CTCP reply" toggle changed state */
    local_give_callsign_in_ctcp_reply=button_active;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wd)))
      gtk_widget_set_sensitive(callsign_zone,TRUE);
    else
      gtk_widget_set_sensitive(callsign_zone,local_send_callsign_with_cw?
  	TRUE:FALSE);
    break;

  case GIVE_GRIDSQUARE_IN_CTCP_REPLY_TOGGLE_BUTTON:
    /* The "send grid square in CTCP reply" toggle changed state */
    local_give_gridsquare_in_ctcp_reply=button_active;
    break;
  
  case GIVE_CWCHANNEL_IN_CTCP_REPLY_TOGGLE_BUTTON:
    /* The "send current channel in CTCP reply" toggle changed state */
    local_give_cwchannel_in_ctcp_reply=button_active;
    break;

  case SEND_CALLSIGN_WITH_CW_TOGGLE_BUTTON:
    /* The "send callsign with CW" toggle changed state */
    local_send_callsign_with_cw=button_active;
    if(button_active)
      gtk_widget_set_sensitive(callsign_zone,TRUE);
    else
      gtk_widget_set_sensitive(callsign_zone,local_reply_to_ctcp &&
  	local_give_callsign_in_ctcp_reply?TRUE:FALSE);
    break;

  case SEND_GRIDSQUARE_WITH_CW_TOGGLE_BUTTON:
    /* The "send grid square with CW" toggle changed state */
    local_send_gridsquare_with_cw=button_active;
    break;

  case SIMULATE_QRN_TOGGLE_BUTTON:
    /* The "simulate QRN" button changed state */
    sharedmem->simulate_qrn=button_active;
    break;

  case SIMULATE_SIGNAL_STRENGTH_TOGGLE_BUTTON:
    /* The "simulate signal strength ..." toggle changed state */
    sharedmem->simulate_signal_strength=button_active;
    gtk_widget_set_sensitive(simulate_sporadicE_zone,button_active?TRUE:FALSE);
    gtk_widget_set_sensitive(default_signal_strength_zone1,button_active?
		TRUE:FALSE);
    gtk_widget_set_sensitive(default_signal_strength_zone2,button_active?
		TRUE:FALSE);
    break;

  case SIMULATE_SPORADICE_TOGGLE_BUTTON:
    /* The "simulate sporadic-E ..." toggle changed state */
    sharedmem->simulate_sporadicE=button_active;
    break;

  case PERSONAL_INFO_SAVE_BUTTON:
    /* The "Save/Change" button (personal info) was pressed : */
    /* Install the new settings "live" */
    sharedmem->reply_to_ctcp=local_reply_to_ctcp;
    sharedmem->give_callsign_in_ctcp_reply=local_give_callsign_in_ctcp_reply;
    sharedmem->give_gridsquare_in_ctcp_reply=
					local_give_gridsquare_in_ctcp_reply;
    sharedmem->give_cwchannel_in_ctcp_reply=local_give_cwchannel_in_ctcp_reply;
    sharedmem->send_callsign_with_cw=local_send_callsign_with_cw;
    sharedmem->send_gridsquare_with_cw=local_send_gridsquare_with_cw;
  
    /* Acquire semaphore to access the personal info */
    cwirc_sem_P(sharedmem->semid,SEM_PERSONAL_INFO);
  
    /* Force callsign to be upper-case and containing only approved characters*/
    strncpy(buf,gtk_entry_get_text(GTK_ENTRY(callsign_text_entry)),
		MAX_NICK_SIZE);
    buf[MAX_NICK_SIZE-1]=0;
  
    k=strlen(buf);
    sharedmem->callsign[0]=0;
    for(i=j=0;i<k;i++)
      if(buf[i]>='!' && buf[i]!=',' && buf[i]<='}')
      {
        sharedmem->callsign[j++]=toupper(buf[i]);
        sharedmem->callsign[j]=0;
      }
  
    /* Force the formatted callsign back to the text entry */
    gtk_entry_set_text(GTK_ENTRY(callsign_text_entry),sharedmem->callsign);
  
    /* Check the grid square */
    strncpy(buf,gtk_entry_get_text(GTK_ENTRY(gridsquare_text_entry)),
  	MAX_GRIDSQUARE_SIZE);
    if(!buf[0] || cwirc_is_grid_square(buf))
    {
      /* Force the grid square to be upper-case */
      k=strlen(buf);
      for(i=0;i<k;i++)
        sharedmem->gridsquare[i]=toupper(buf[i]);
      sharedmem->gridsquare[i]=0;
    }
  
    /* Force the formatted grid square back to the text entry */
    gtk_entry_set_text(GTK_ENTRY(gridsquare_text_entry),sharedmem->gridsquare);
  
    /* Release semaphore to prevent reconfiguring I/O process while it works*/
    cwirc_sem_V(sharedmem->semid,SEM_PERSONAL_INFO);
  
    /* Save the settings in the configuration file */
    cwirc_save_rcfile(RCFILE);
  
    /* Change the sensitivity and title of the propagation simulation zone in
       case a valid grid square was given */
    gtk_widget_set_sensitive(propag_sim_zone,sharedmem->gridsquare[0]?
		TRUE:FALSE);
    gtk_frame_set_label(GTK_FRAME(propag_sim_zone),sharedmem->gridsquare[0]?
  	"Propagation simulation":
  	"Propagation simulation [needs the grid square to be set]");
    break;
  }
}



/* Configure event callback for the S-meter */
static gboolean smeter_configure_event(GtkWidget *wdg,GdkEventConfigure *ev)
{
  draw_smeter(signal_strength,1);

  return(TRUE);
}



/* Expose event callback for the S-meter */
static gboolean smeter_expose_event(GtkWidget *wdg,GdkEventExpose *ev)
{
  return(smeter_configure_event(wdg,(GdkEventConfigure *)ev));
}



/* Mouse button press event callback for the S-meter */
static gboolean smeter_pressed_event(GtkWidget *wdg,GdkEventButton *ev)
{
  if(ev->type==GDK_BUTTON_PRESS)	/* Filter out double-clicks here */
  {
    sharedmem->sidetone_mode=!sharedmem->sidetone_mode;
    draw_smeter(signal_strength,1);
  }

  return(TRUE);
}



/* Configure event callback for the "morse key icon" */
static gboolean morsekey_configure_event(GtkWidget *wdg,GdkEventConfigure *ev)
{
  gdk_draw_drawable(morsekey_drawingarea->window,morsekey_gc,
		sharedmem->doiambic?iambickey_bg_pixmap:straightkey_bg_pixmap,
		0,0,0,0,morsekey_width,morsekey_height);

  return(TRUE);
}



/* Expose event callback for the "morse key icon" */
static gboolean morsekey_expose_event(GtkWidget *wdg,GdkEventExpose *ev)
{
  return(morsekey_configure_event(wdg,(GdkEventConfigure *)ev));
}



/* Mouse button press event callback for the "morse key icon" */
static gboolean morsekey_pressed_event(GtkWidget *wdg,GdkEventButton *ev)
{
  if(ev->type==GDK_BUTTON_PRESS)	/* Filter out double-clicks here */
  {
    if(ev->button==1)
      sharedmem->mouseinputbutton0=1;
    else
      sharedmem->mouseinputbutton1=1;
  }

  return(TRUE);
}



/* Mouse button release event callback for the "morse key icon" */
static gboolean morsekey_released_event(GtkWidget *wdg,GdkEventButton *ev)
{
  if(ev->button==1)
    sharedmem->mouseinputbutton0=0;
  else
    sharedmem->mouseinputbutton1=0;

  return(TRUE);
}



/* Draw the S-meter (value is 0 -> 100). If full_refresh is asserted, the entire
   S-meter area is redrawn. Otherwise, only the space occupied by the previous
   needle is redrawn. */
static void draw_smeter(int value,T_BOOL full_refresh)
{
  static int pn_x,pn_y,pn_w,pn_h;
  static T_U8 x1[101],y1[101],x2[101],y2[101];
  static int i=1;

  /* Is this the first call to this function ? */
  if(i)
  {
    full_refresh=1;	/* Make sure we start off by drawing everything */

    /* Precalculate the needle line coordinates for all possible meter values */
    for(i=0;i<=100;i++)
      calculate_smeter_needle_coordinates(i,&x1[i],&y1[i],&x2[i],&y2[i]);

    i=0;
  }

  /* Do we do a full refresh, or is it the first time we're called ? */
  if(full_refresh)
  {
    pn_x=0;
    pn_y=0;
    pn_w=smeter_width;
    pn_h=smeter_height;
  }

  /* Make sure the value is bound */
  if(value>100)
    value=100;
  
  /* Draw the background image first, or the part of it necessary to cover the
     previous needle */
  gdk_draw_drawable(smeter_drawingarea->window,smeter_gc,
		sharedmem->sidetone_mode?sidetone_bg_pixmap:smeter_bg_pixmap,
		pn_x,pn_y,pn_x,pn_y,pn_w,pn_h);

  /* Draw the new needle on top of the background image */
  gdk_draw_line(smeter_drawingarea->window,smeter_gc,x1[value],y1[value],
		x2[value],y2[value]);

  /* Calculate the coordinates of the smallest box that covers the line */
  if(x1[value]<=x2[value])
  { 
    pn_x=x1[value]-1;
    pn_w=x2[value]-x1[value]+3;
  }
  else if(x1[value]>x2[value])
  { 
    pn_x=x2[value]-1;
    pn_w=x1[value]-x2[value]+3;
  }
  pn_y=y2[value];
  pn_h=y1[value]-y2[value];
}



/* Calculate the coordinates of the S-meter's needle within the S-meter box
   given the meter's value */
static void calculate_smeter_needle_coordinates(int value,T_U8 *x1,T_U8 *y1,
	T_U8 *x2,T_U8 *y2)
{
  double nr_rad;
  double angle;
  int nl;
  int ncx,ncy,ntx,nty;
  int x,y,w,h;

  w=smeter_width;
  h=smeter_height;
  x=w/2;
  y=(h+SMETER_ARC_DOWN_OFFSET)/2;

  /* Calculate the needle range in radians */
  nr_rad=(SMETER_NEEDLE_RANGE*M_PI)/180;

  /* Calculate the needle's length in the S-meter rectangle */
  nl=w/(2*tan(nr_rad/2)*cos(nr_rad/2));

  /* Calculate the angle of the needle with the given value */
  angle=((50-value)*nr_rad)/100+M_PI_2;

  /* Calculate the coordinates of the needle's ends in the S-meter rectangle */
  ncx=0;
  ncy=h-nl;
  ntx=nl*cos(angle);
  nty=ncy+nl*sin(angle);

  /* Calculate where the needle intersects with the absissa in the rectangle
     box, so we clip what's below it and that's not supposed to be visible */
  if(nl>h)
  {
    ncx=((ncx+ntx)*(nl-h))/nl;
    ncy=0;
  }

  /* Calculate the final line's endpoints' coordinates */
  *x1=x+ncx;
  *y1=y+h/2-ncy;
  *x2=x+(ntx*.8);
  *y2=y+h/2-(nty*.8);
}



/* Make an error popup with a message and an OK button */
static void error_popup(char *message)
{
  /* Create the popup window */
  popup_wd=gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(popup_wd),"Error");

  label=gtk_label_new(message);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(popup_wd)->vbox),label,TRUE,TRUE,0);

  align_wdg=gtk_alignment_new(.5,.5,0,0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(popup_wd)->action_area),align_wdg,
	TRUE,TRUE,0);

  button=gtk_button_new_with_label("Okay");
  gtk_signal_connect_object(GTK_OBJECT(button),"clicked",
	G_CALLBACK(gtk_widget_destroy),GTK_OBJECT(popup_wd));
  gtk_container_add(GTK_CONTAINER(align_wdg),button);

  gtk_widget_show_all(popup_wd);
}



/* GTK timeout callback : this is were we update the S-meter, the automatic
   decoder line and watch for external orders to stop the frontend. */
static gint gtk_timeout(gpointer data)
{
  char wpm[9];
  
  /* Check if we have a message from the I/O process */
  if(!cwirc_sem_P(sharedmem->semid,SEM_IO_PROCESS_MSG))	/* Acquire semaphore */
  {
    if(sharedmem->io_process_msg[0])
    {
      error_popup(sharedmem->io_process_msg);
      sharedmem->io_process_msg[0]=0;
    }

    /* Release the semaphore */
    cwirc_sem_V(sharedmem->semid,SEM_IO_PROCESS_MSG);
  }

  
  /* Do we need to stop the gui ? */
  if(sharedmem->stop_frontend)
  {
    gtk_main_quit();
    return(FALSE);
  }

  /* Emulate the S-meter */
  signal_strength_prev=signal_strength;
  signal_strength=(signal_strength+sharedmem->recv_signal)/2;
  if(signal_strength!=signal_strength_prev)
    draw_smeter(signal_strength,0);

  /* Has the decoded morse line been updated ? */
  if(sharedmem->decoded_msg_updated)
  {
    /* Update the decoded morse line */
    gtk_entry_set_text(GTK_ENTRY(decoder_text_entry),
	sharedmem->decoded_msg_buf);
    gtk_editable_set_position(GTK_EDITABLE(decoder_text_entry),-1);

    /* Update the WPM label */
    if(sharedmem->decoded_msg_wpm>-1)
      sprintf(wpm,"%.0f WPM",sharedmem->decoded_msg_wpm);
    else
      sprintf(wpm,"? WPM");
    gtk_label_set_text(GTK_LABEL(decoder_wpm_label),wpm);

    sharedmem->decoded_msg_updated=0;
  }

  return(TRUE);
}
