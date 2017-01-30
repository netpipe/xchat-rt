#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <xchat-plugin.h>
#include <glib.h>

/* plugin handle */
static xchat_plugin *ph;   

/* only one channel is supported */
gchar *channel = NULL;

/* integer value of the maximum accepted idle time */
gint max_idle_time;

/* integer value of the maximum accepted kick time before you were banned */
gint max_kick_before_ban;

/* integer value. Check users idle time each 'check_idletime_time' */
gint check_idletime_time = 10000;

/* GSList including all users, which are allowed to idle, 
   and so excluded from the "idletime-check" */
GSList *excluded_users_list;

/* list of still kicked users + count 
   0: user1
   1: count_of_user1
   X+0: userX+0 
   X+1: count of userX+0
*/
GSList *kicked_users_list;
 
/* true if idlekick is enabled */
gboolean idlekick_is_enabled = FALSE;

/* count of kicked users */
gint kicked_users_count = 0;

/* the wii hook */
xchat_hook *wii_hook;

/* the get user_list (of the channel) hook */
xchat_hook *user_list_hook;

// ------------------------------------------------------------------------
// check if irc user is operator on this channel
// ------------------------------------------------------------------------

static gboolean check_channel_operator_status()
{
  xchat_list *list; 
  
  list = xchat_list_get(ph, "users");

  if (list)
  {
    while(xchat_list_next(ph, list))
    {
      if (strcmp(xchat_get_info(ph, "nick"), xchat_list_str(ph, list, "nick")) == 0)
	if (strcmp(xchat_list_str(ph, list, "prefix"), "@") == 0)
	  return TRUE;
    }
  }
    xchat_list_free(ph, list);
    return FALSE;
}

// ------------------------------------------------------------------------
// get the count of one kicked user (0 if till now not kicked)
// ------------------------------------------------------------------------

static int get_user_kick_count (gchar *user, int *position)
{
  gint i;
  gchar *checked_user=NULL;
  
  for (i=0; i<g_slist_length(kicked_users_list); i++)
  {
    checked_user = g_strdup(g_slist_nth_data(kicked_users_list, i));
    
    if (strcmp(user, checked_user) == 0)
    {
      *position = i+1;
      return atoi(g_slist_nth_data(kicked_users_list, *position));
    }
  }

  g_free(checked_user);
  return -1;
}

// ------------------------------------------------------------------------
// set new user count
// ------------------------------------------------------------------------
static void set_new_user_kick_count(GSList *changed_element, int number)
{
    changed_element->data = g_strdup_printf("%i", number);
}

// ------------------------------------------------------------------------
// set the count of one kicked user
// ------------------------------------------------------------------------

static int set_user_kick_count(gchar *user)
{
  gint new_count = 0;
  int position;

  new_count = get_user_kick_count(user, &position);

  if (new_count != -1)
  {
    GSList *changed_element;
    changed_element = g_slist_nth (kicked_users_list,
				   position);

    new_count = atoi(changed_element->data);
    new_count++;

    set_new_user_kick_count(changed_element, new_count);
  }
  else
  {
    new_count = 1;
    kicked_users_list = g_slist_append(kicked_users_list, g_strdup(user));
    kicked_users_list = g_slist_append(kicked_users_list, g_strdup_printf("%i",new_count));
  }

  return new_count;
}

// ------------------------------------------------------------------------
// get the configuration settings 
// ------------------------------------------------------------------------

static int check_idle_time(char *word[], void *userdata)
{
  char array_idle_time[7];
  gint i=0;
  gint user_count=0;
  gchar *user;
  gint int_idle_time;
  gchar *char_idle_time = NULL;

  if (idlekick_is_enabled)
  {
    user = g_strdup_printf("%s", word[1]);
    char_idle_time = g_strdup_printf("%s",word[2]);
    
    while (*char_idle_time != '\0')
    {
      if (*char_idle_time != ':')
      { 
	array_idle_time[i] = *char_idle_time;
	i++;
      }
      
      char_idle_time++;
    }
    
    array_idle_time[i] = '\0';
    int_idle_time = atoi(array_idle_time);
    
    if (int_idle_time >= max_idle_time)
    {
      /* set the counf of this kicked user */
      user_count = set_user_kick_count(user);
      
      /* ban the user, if kick-count > max_kick_before_ban (-1 is disabled) */
      if (max_kick_before_ban != -1)
      {
	if (user_count > max_kick_before_ban)
	{
	  int position;
	  GSList *changed_element;
	  
	  get_user_kick_count(user, &position);
	  changed_element = g_slist_nth (kicked_users_list,
					 position);
	  set_new_user_kick_count(changed_element, 0);
	  xchat_commandf(ph, "QUOTE MODE %s +b %s", channel, user);
	}
	else if (user_count == max_kick_before_ban)
	{
	  xchat_commandf(ph, "MSG %s You were %i times kicked, because you are idle "
			 "to long (longer as %i seconds). Next time "
			 "(in exactly %i seconds) you were banned from %s", 
			 user, 
			 user_count,
			 max_idle_time, 
			 max_idle_time, 
			 channel);
	}
      }
      
      /* kick the user */
      xchat_commandf(ph, "QUOTE KICK %s %s Idle-time", 
		     channel, 
		     user);
      
      /* set the count of kicked users +1 */
      kicked_users_count++;
    }
    
    g_free(user);
  }

  return XCHAT_EAT_NONE;
}

// ------------------------------------------------------------------------
// get the configuration settings 
// ------------------------------------------------------------------------

static int get_user_list(void *userdata)
{
  xchat_list *list; 
  gboolean kick_user = TRUE;
  int i;

  if (check_channel_operator_status() 
      && (strcmp(channel, xchat_get_info(ph, "channel")) == 0)
      )
  {
    list = xchat_list_get(ph, "users");
    
    if(list)
    {
      while(xchat_list_next(ph, list))
      {
	kick_user = TRUE;
	for (i=0; i<g_slist_length(excluded_users_list); i++)
	{
	  if (strcmp(xchat_list_str(ph, list, "nick"), g_slist_nth_data(excluded_users_list, i)) == 0)
	    kick_user = FALSE;
	}
	
	if (kick_user)
	  xchat_commandf(ph, "wii %s", xchat_list_str(ph, list, "nick"));
      }
      xchat_list_free(ph, list);
    }
  }
  
  return XCHAT_EAT_ALL;
}

// ------------------------------------------------------------------------
// get a integer value out of the config
// ------------------------------------------------------------------------
static int get_int_from_config(char puffer[200])
{
  gboolean var_begin, var_end;
  char char_int_value[10];
  int j, i;
  
  var_begin = FALSE;
  var_end = FALSE;
  j=0;
  
  for (i=0;i<=strlen(puffer);i++)
  {
    if (puffer[i] == ';')
      var_end = TRUE;
    else if (var_begin == TRUE && var_end == FALSE)
    {
      char_int_value[j] = puffer[i];
      j++;
    } 
    else if (puffer[i] == '=')
    {		
      var_begin = TRUE;
      j=0;
    }
  }
  return atoi(char_int_value);
}

// ------------------------------------------------------------------------
// get the configuration settings 
// ------------------------------------------------------------------------

static void get_settings()
{
  gchar *configfile=NULL;
  gchar excluded_users_temp[30];
  gchar puffer[200];
  int i, j;
  gboolean var_begin, var_end;
  int user=0;
  FILE *fd;

  /* get the configuration file */
  configfile = g_strdup_printf ("%s/.xchat2/idlekick.conf", g_get_home_dir ());
  
  fd=fopen (configfile,"r");

  if (fd != NULL)
  {
    while (fscanf (fd,"%s",puffer) != EOF)
    {
      if (strstr(puffer,"max_idle"))
      {
	max_idle_time = get_int_from_config(puffer);
      }
      else if (strstr(puffer,"max_kick_before_ban"))
      {
	max_kick_before_ban = get_int_from_config(puffer);
      }
      else if (strstr(puffer, "check_idletime_time"))
      {
	check_idletime_time = get_int_from_config(puffer);
      }
      else if (strstr(puffer,"excluded_user"))
      {
	var_begin=FALSE; 
	var_end=FALSE; 
	j=0;
	user=0;
	
	for (i=0; i<=strlen(puffer); i++)
	{
	  if (puffer[i]==';')
	  {
	    var_end = TRUE;
	    if (j>0)
	    {
	      excluded_users_temp[j] = '\0';
	      excluded_users_list = g_slist_append(excluded_users_list, 
						   g_strdup_printf("%s", excluded_users_temp));
	    }
	  }
	  else if (puffer[i] == ',')
	  {
	    excluded_users_temp[j] = '\0';
	    excluded_users_list = g_slist_append(excluded_users_list, 
						 g_strdup_printf("%s", excluded_users_temp));
	    strcpy(excluded_users_temp, "");
	    j=0;
	  }
	  else if (var_begin == TRUE && var_end == FALSE)
	  {
	    excluded_users_temp[j] = puffer[i];
	    j++;
	  }
	  else if (puffer[i] == '=')
	    {		
	      var_begin = TRUE;
	      j=0;
	    }
	}
      }
    }
  fclose(fd);
  }
  else
    xchat_print(ph, "Idlekick: Configuration Error. Missing config file ?");



  g_free(configfile);
}

// ------------------------------------------------------------------------
// start, stop, status and show status function
// ------------------------------------------------------------------------

static int idlekick_action(char *word[], char *word_eol[], void *userdata)
{
  if (check_channel_operator_status())
  {
    if (strcmp(word[2],"start") == 0)
    {
      if (!idlekick_is_enabled)
      {
	channel = g_strdup_printf("%s", xchat_get_info(ph, "channel"));
	idlekick_is_enabled = TRUE;

	/* get configuration settings */
	get_settings();

	xchat_printf(ph, "IdleKick now enabled on %s !\n", channel);
	xchat_printf(ph, "Will kick users, if idle time is higher than %i\n",	
		     max_idle_time);

	if (max_kick_before_ban != -1) 
	  xchat_printf(ph, "Ban users after %i kicks\n",
		       max_kick_before_ban);
	else
	  xchat_print(ph, "Ban function disabled\n" );

	/* check the idle time if, we get a a "WhoIs Idle Line" (whois, wii) request */
	wii_hook = xchat_hook_print(ph, "WhoIs Idle Line", 
				    XCHAT_PRI_NORM, 
				    check_idletime_time, 
				    NULL); 
	
	/* check users idle time every 10 seconds */
	user_list_hook = xchat_hook_timer(ph, 
					  check_idle_time, 
					  get_user_list, 
					  NULL);
      }
      else
	xchat_printf(ph, "IdleKick is already enabled on %s!\n", channel);
    }
    else if (strcmp(word[2],"stop") == 0)
    {
      if (idlekick_is_enabled)
      {
	idlekick_is_enabled = FALSE;
	xchat_printf(ph, "IdleKick now disabled on %s !\n", channel);
	channel = NULL;

	/* unhook wii and user_list hooks */
	xchat_unhook(ph, wii_hook);
	xchat_unhook(ph, user_list_hook);

	kicked_users_count = 0;
      }
      else
	xchat_print(ph, "IdleKick is already disabled!\n");
    }
    else if (strcmp(word[2],"status") == 0)
    {
      if (idlekick_is_enabled)
	xchat_printf(ph, "IdleKick is enabled on %s (%i kicked users)!\n", 
		     channel, kicked_users_count);
      else
	xchat_print(ph, "IdleKick is disabled!\n");
    }
    else
    {
      xchat_print(ph, 
		   "\nIdleKick Usage:!\n"
		   "   idlekick start  - Start IdleKick\n"
		   "   idlekick stop   - Stop IdleKick\n"
		   "   idlekick status - Show IdleKick status on\n"
		   "   idlekick help   - Show this help message\n"
		  );
    }
  }
  else
    xchat_print(ph, "Idlekick: You are not Channel operator!\n");
  
  return XCHAT_EAT_ALL;   /* eat this command so xchat and other plugins can't process it */
}

// ------------------------------------------------------------------------
// idlekick init function 
// ------------------------------------------------------------------------

int xchat_plugin_init (xchat_plugin *plugin_handle,
		       char **plugin_name,
		       char **plugin_desc,
		       char **plugin_version,
		       char *arg)
{
  /* we need to save this for use with any xchat_* functions */
  ph = plugin_handle;
  
  *plugin_name = "IdleKick";
  *plugin_desc = "Kick users that idle a specific time";
  *plugin_version = "0.3";
  
  xchat_hook_command(ph, "IdleKick", XCHAT_PRI_NORM, idlekick_action,
		     "Usage: IDLEKICK [start, stop, status, help]", NULL);

  return 1;       
}

