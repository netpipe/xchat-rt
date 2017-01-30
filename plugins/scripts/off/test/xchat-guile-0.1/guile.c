#include "xchat-plugin.h"
#include <libguile.h>
#include <string.h>
#include <glib.h>

#define PLUGIN_NAME	"Guile"
#define PLUGIN_DESC	"Scheme (Guile) scripting interface"
#define PLUGIN_VERSION	"0.1"
	
#define PLUGIN_INFO	"Scheme (Guile) plugin for XChat - Version " \
			PLUGIN_VERSION \
 			"\nCopyright 2005 Zeeshan Ali"
#define	PLUGIN_COMMAND	"guile"
#define	PLUGIN_USAGE	"Usage: /guile eval SCHEME_EXPRESSION\n" \
			"              about"

#define	TO_PRINT_MAX	10240
#define	PLUGIN_NAME_MAX 256

#define PLUGIN_INIT_FUNC_SYMBOL	    "xchat-plugin-init"
#define PLUGIN_DEINIT_FUNC_SYMBOL   "xchat-plugin-deinit"
#define PLUGIN_DESCRIPTION_SYMBOL   "xchat-plugin-description"
#define PLUGIN_VERSION_SYMBOL	    "xchat-plugin-version"

#define XCHAT_EAT_NONE_SYMBOL	"xchat-eat-none"    /* pass it on through! */
#define XCHAT_EAT_XCHAT_SYMBOL	"xchat-eat-xchat"   /* don't let xchat see this
						       event */
#define XCHAT_EAT_PLUGIN_SYMBOL "xchat-eat-plugin"  /* don't let other plugins
						       see this event */
#define XCHAT_EAT_ALL_SYMBOL	"xchat-eat-all"     /* don't let anything see
						       this event */

#define XCHAT_PRI_HIGHEST_SYMBOL    "xchat-priority-highest"
#define XCHAT_PRI_HIGH_SYMBOL	    "xchat-priority-high"
#define XCHAT_PRI_NORM_SYMBOL	    "xchat-priority-normal"
#define XCHAT_PRI_LOW_SYMBOL	    "xchat-priority-low"
#define XCHAT_PRI_LOWEST_SYMBOL	    "xchat-priority-lowest"

struct scm_body_thunk_1_data 
{
	SCM body_proc;
	SCM arg;
};

struct scm_body_thunk_2_data 
{
	SCM body_proc;
	SCM arg1;
	SCM arg2;
};

static xchat_plugin *ph;        /* plugin handle */
static SCM port;
static xchat_hook *guile_cmd;
static xchat_hook *load_cmd;
static xchat_hook *unload_cmd;

static scm_t_bits xchat_context_t;

static gchar *to_print;

static GSList *all_hooks = NULL;
static SCM lock; /* one global lock for all global varriables */

static SCM
xchat_context_equalp (SCM a, SCM b)
{
    if (SCM_SMOB_DATA (a) == SCM_SMOB_DATA (b))
	return SCM_BOOL_T;
    else
	return SCM_BOOL_F;
}

static SCM 
scm_body_thunk_1 (void *body_data)
{
    struct scm_body_thunk_1_data *c = 
	(struct scm_body_thunk_1_data *) body_data;

    return scm_call_1 (c->body_proc, c->arg);
}

static SCM 
scm_body_thunk_2 (void *body_data)
{
    struct scm_body_thunk_2_data *c = 
	(struct scm_body_thunk_2_data *) body_data;

    return scm_call_2 (c->body_proc, c->arg1, c->arg2);
}

static SCM
scm_new_port_table_entry (scm_t_bits tag)
#define FUNC_NAME "scm_new_port_table_entry"
{
    SCM port;
    scm_t_port *pt;

    SCM_NEWCELL (port);
    pt = scm_add_to_port_table (port);
    SCM_SET_CELL_TYPE (port, tag);
    SCM_SETPTAB_ENTRY (port, pt);
    return port;
}
#undef FUNC_NAME

static inline int scm2bool (SCM obj)
{
    return (SCM_FALSEP (obj)) ? 0 : 1;
}

static gint
xchat_fill_input (SCM port)
{
    return 'a';
}

/* NOTE: We cache strings untill someone sends a string with a '\n'.
 * I don't like to implement it this way but it's just a way to avoid getting
 * lists (e.g) to be printed on multiple lines (one member printer on one
 * line) which would normally happen because:
 * 1. xchat_printf puts each string it gets on a new line
 * 2. guile (and maybe other scheme/lisp interpreters too) display lists by 
 * calling display on each member.
 */
static void
xchat_flush_print (void)
{
    if (to_print != NULL) {
	xchat_printf (ph, "%s", to_print);
	g_free (to_print);
	to_print = NULL;
    }
}

static void
xchat_write (SCM port, const void *data, size_t size)
{
    gchar *str = g_strndup ((gchar *) data, size);

    scm_lock_mutex (lock);
    if (to_print != NULL)
	to_print = g_strjoin (NULL, to_print, str, NULL);

    else
	to_print = g_strdup (str);

    if (g_pattern_match_simple ("*\n*", str) || 
	    g_utf8_strlen (to_print, -1) >= TO_PRINT_MAX) {
	xchat_flush_print ();
    }
    scm_unlock_mutex (lock);

    g_free (str);
}

static void
eval_func (gchar * expression)
{
    SCM value;

    value = scm_internal_catch (SCM_BOOL_T,
	    (scm_t_catch_body) scm_c_eval_string,
	    (void *) expression,
	    (scm_t_catch_handler) scm_handle_by_message_noexit,
	    NULL);

    if (!SCM_EQ_P (value, SCM_UNDEFINED) && !SCM_EQ_P (value, SCM_UNSPECIFIED))
	scm_display (value, port);

    scm_lock_mutex (lock);
    xchat_flush_print ();
    scm_unlock_mutex (lock);
}

static gboolean
load_plugin (gchar * filename)
{
    SCM success;
    struct scm_body_thunk_1_data c;

    c.body_proc = SCM_VARIABLE_REF (scm_c_lookup ("xchat:load-plugin"));
    c.arg = scm_makfrom0str (filename);

    success = scm_internal_catch (SCM_BOOL_T,
	    scm_body_thunk_1,
	    (void *) &c,
	    (scm_t_catch_handler) scm_handle_by_message_noexit,
	    (void *) NULL);

    if (success == SCM_BOOL_F)
	return FALSE;

    return TRUE;
}

static gboolean
unload_plugin (gchar * plugin_name)
{
    SCM success;
    struct scm_body_thunk_1_data c;

    c.body_proc = SCM_VARIABLE_REF (scm_c_lookup ("xchat:unload-plugin"));
    c.arg = scm_makfrom0str (plugin_name);

    success = scm_internal_catch (SCM_BOOL_T,
	    scm_body_thunk_1,
	    (void *) &c,
	    (scm_t_catch_handler) scm_handle_by_message_noexit,
	    (void *) NULL);

    if (success == SCM_BOOL_F)
	return FALSE;

    return TRUE;
}

static gchar *
get_file_extension (const gchar *filename)
{
    gchar **tokens;
    gchar *extension;

    tokens = g_strsplit (filename, ".", 2);
    if (tokens[0] == NULL)
	extension = NULL;
    else
	extension = g_strdup (tokens[1]);
    g_strfreev (tokens);

    return extension;
}

static gint
guile_command (gchar * word[], gchar * word_eol[], void *userdata)
{
    if (word_eol[2][0] == 0)
	xchat_printf (ph, PLUGIN_USAGE);

    else if (g_ascii_strcasecmp (word[2], "eval") == 0 && word_eol[3][0] != 0)
	eval_func (word_eol[3]);

    else if (g_ascii_strcasecmp (word[2], "load") == 0) {
	load_plugin (word[3]);
    }

    else if (g_ascii_strcasecmp (word[2], "unload") == 0) {
	unload_plugin (word[3]);
    }

    else if (g_ascii_strcasecmp (word[2], "about") == 0)
	xchat_printf (ph, PLUGIN_INFO);

    else
	xchat_printf (ph, PLUGIN_USAGE);

    return XCHAT_EAT_ALL;
}

static gint
load_command (gchar * word[], gchar * word_eol[], void *userdata)
{
    gchar *extension;
    gint ret = XCHAT_EAT_NONE;

    if (word_eol[2][0] == 0)
	return ret;

    extension = get_file_extension (word[2]);

    /* The only way we know that the file is ours is the file extension */
    if (extension == NULL)
	return ret;

    if (g_utf8_collate (extension, "scm") == 0) {
	load_plugin (word[2]);
	ret = XCHAT_EAT_XCHAT;
    }

    g_free (extension);
    return ret;
}

static gint
unload_command (gchar * word[], gchar * word_eol[], void *userdata)
{
    gint ret = XCHAT_EAT_NONE;

    if (word_eol[2][0] == 0)
	return ret;

    if (unload_plugin (word[2]))
	ret = XCHAT_EAT_XCHAT;

    else
	ret = XCHAT_EAT_NONE;

    return ret;
}

/* API starts here */
static SCM 
api_print (SCM str)
{
    xchat_printf (ph, "%s", SCM_STRING_CHARS (str));

    return SCM_UNSPECIFIED;
}

static SCM 
api_emit_print (SCM event_name, SCM args)
{
    /* We'll only take the first 9 args passed to us */
    gchar *argv[10];
    gint result, i;
    SCM lst;

    memset(&argv, 0, sizeof (gchar *) * 10);

    lst = args;

    for (i=0; i<9 && !scm2bool (scm_null_p (lst)); i++) {
	argv[i] = SCM_STRING_CHARS (SCM_CAR (lst));
	lst = SCM_CDR (lst);
    }

    result = xchat_emit_print (ph, 
	    SCM_STRING_CHARS (event_name),
	    argv[0],
	    argv[1],
	    argv[2],
	    argv[3],
	    argv[4],
	    argv[5],
	    argv[6],
	    argv[7],
	    argv[8]);

    return SCM_BOOL (result);
}

static SCM 
api_command (SCM command)
{
    xchat_command (ph, SCM_STRING_CHARS (command));

    return SCM_UNSPECIFIED;
}

static inline SCM
scm_context (xchat_context *context)
{
    SCM context_obj = scm_make_smob (xchat_context_t);
    SCM_SET_SMOB_DATA (context_obj, context);

    return context_obj;
}

static SCM 
api_find_context (SCM server, SCM channel)
{
    xchat_context *context;
    gchar *server_name, *channel_name;

    if (server == SCM_BOOL_F)
	server_name = NULL;
    else
	server_name = SCM_STRING_CHARS (server);

    if (channel == SCM_BOOL_F)
	channel_name = NULL;
    else
	channel_name = SCM_STRING_CHARS (channel);

    context = xchat_find_context (ph, server_name, channel_name);

    if (context == NULL)
	return SCM_BOOL_F;

    else
	return scm_context (context);
}

static SCM 
api_current_context (void)
{
    xchat_context *context;

    context = xchat_get_context (ph);

    if (context == NULL)
	return SCM_BOOL_F;

    else
	return scm_context (context);
}

static SCM 
api_set_context (SCM context_obj)
{
    int res;

    res = xchat_set_context (ph, (xchat_context *) SCM_SMOB_DATA (context_obj));

    return SCM_BOOL (res);
}

static SCM 
api_nick_compare (SCM nick1, SCM nick2)
{
    int res;

    res = xchat_nickcmp (ph, 
	    SCM_STRING_CHARS (nick1),
	    SCM_STRING_CHARS (nick2));

    return scm_int2num (res);
}

static SCM 
api_get_info (SCM id)
{
    const gchar *info;

    info = xchat_get_info (ph, SCM_STRING_CHARS (id));

    return scm_makfrom0str (info);
}

static SCM
api_send_modes (SCM targets, SCM modes_per_line, SCM sign, SCM mode)
{
    glong ntargets;

    ntargets = scm_ilength (targets);

    if (ntargets <= 0)
	scm_throw (scm_makfrom0str ("xchat-guile"),
		scm_list_1 (scm_makfrom0str ("The first argument should be" 
			"a proper list containing atleast one object")));
    else {
	gint i;
	/* Allocate the array of char pointers */
	gchar **targets_array = g_malloc (ntargets * sizeof (gchar *));

	for (i=0; i<ntargets; i++)
	    targets_array[i] = 
		SCM_STRING_CHARS (scm_list_ref (targets, scm_int2num (i)));
	xchat_send_modes (ph,
		(const gchar **) targets_array,
		ntargets, SCM_INUM (modes_per_line),
		SCM_CHAR (sign),
		SCM_CHAR (mode));
    }

    return SCM_UNSPECIFIED;
}

static SCM
api_list_fields (SCM list_name)
{
    SCM scm_list;
    const gchar * const *fields;
    gint i;

    scm_list = SCM_LIST0;

    fields = xchat_list_fields (ph, SCM_STRING_CHARS (list_name));
    if (fields == NULL)
	return SCM_BOOL_F;

    for (i = 0; fields[i]; i++) {
	const char *field_name = fields[i]+1;

	scm_list = scm_cons (scm_makfrom0str (field_name), scm_list);
    }

    return scm_reverse (scm_list);
}

static SCM
api_list_get (SCM list_name)
{
    xchat_list *list = NULL;
    SCM scm_list;
    const gchar * const *fields;
    gint i;

    list = xchat_list_get (ph, SCM_STRING_CHARS (list_name));
    if (list == NULL)
	return SCM_BOOL_F;

    fields = xchat_list_fields (ph, SCM_STRING_CHARS (list_name));
    if (fields == NULL) {
	xchat_list_free(ph, list);
	return SCM_BOOL_F;
    }

    scm_list = SCM_LIST0;
    while (xchat_list_next (ph, list)) {
	SCM inner_list = SCM_LIST0;
	for (i = 0; fields[i]; i++) {
	    const gchar *fld = fields[i]+1;
	    SCM attr = NULL;
	    const gchar *sattr;
	    gint iattr;
	    time_t tattr;

	    switch (fields[i][0]) {
		case 's':
		    sattr = xchat_list_str (ph, list, (gchar *) fld);
		    if (sattr != NULL)
			attr = scm_makfrom0str (sattr);
		    break;
		case 'i':
		    iattr = xchat_list_int (ph, list, (gchar *) fld);
		    attr = scm_int2num (iattr);
		    break;
		case 't':
		    tattr = xchat_list_time (ph, list, (gchar *) fld);
		    attr = scm_long2num ((long) tattr);
		    break;
		case 'p':
		    sattr = xchat_list_str (ph, list, (gchar *) fld);
		    if (sattr != NULL && strcmp (fld, "context") == 0)
			attr = scm_context ((xchat_context *) sattr);
		    break;
		default:
		    attr = NULL;
	    }

	    if (attr != NULL)
		inner_list = scm_cons (attr, inner_list);
	}

	scm_list = scm_cons (scm_reverse (inner_list), scm_list);
    }

    xchat_list_free(ph, list);
    return scm_reverse (scm_list);
}

static SCM
scm_list_from_strarray (const gchar **array)
{
    SCM list = SCM_LIST0;
    gint i;

    for (i = 0; array[i] && array[i][0]; i++)
	list = scm_cons (scm_makfrom0str (array[i]), list);

    return scm_reverse (list);
}

static gint
xchat_eat_symbol_to_int (SCM symbol)
{
    gint eat_value = XCHAT_EAT_NONE;

    if (SCM_EQ_P (symbol, scm_str2symbol (XCHAT_EAT_NONE_SYMBOL)))
	eat_value = XCHAT_EAT_NONE;

    else if (SCM_EQ_P (symbol, scm_str2symbol (XCHAT_EAT_XCHAT_SYMBOL)))
	eat_value = XCHAT_EAT_XCHAT;

    else if (SCM_EQ_P (symbol, scm_str2symbol (XCHAT_EAT_PLUGIN_SYMBOL)))
	eat_value = XCHAT_EAT_PLUGIN;

    else if (SCM_EQ_P (symbol, scm_str2symbol (XCHAT_EAT_ALL_SYMBOL)))
	eat_value = XCHAT_EAT_ALL;

    return eat_value;
}

static gint 
command_callback (gchar *word[], gchar *word_eol[], void *user_data)
{
    struct scm_body_thunk_2_data c;
    SCM eat;

    c.body_proc = SCM_PACK ((scm_t_bits) (user_data));
    c.arg1 = scm_list_from_strarray ((const gchar **) word + 1);
    c.arg2 = scm_list_from_strarray ((const gchar **) word_eol + 1);

    eat = scm_internal_catch (SCM_BOOL_T,
	    scm_body_thunk_2,
	    &c,
	    (scm_t_catch_handler) scm_handle_by_message_noexit,
	    NULL);

    return xchat_eat_symbol_to_int (eat);
}

static gint
xchat_priority_symbol_to_int (SCM symbol)
{
    gint priority = XCHAT_PRI_NORM;

    if (SCM_EQ_P (symbol, scm_str2symbol (XCHAT_PRI_NORM_SYMBOL)))
	priority = XCHAT_PRI_NORM;

    else if (SCM_EQ_P (symbol, scm_str2symbol (XCHAT_PRI_HIGH_SYMBOL)))
	priority = XCHAT_PRI_HIGH;

    else if (SCM_EQ_P (symbol, scm_str2symbol (XCHAT_PRI_HIGHEST_SYMBOL)))
	priority = XCHAT_PRI_HIGHEST;

    else if (SCM_EQ_P (symbol, scm_str2symbol (XCHAT_PRI_LOW_SYMBOL)))
	priority = XCHAT_PRI_LOW;

    else if (SCM_EQ_P (symbol, scm_str2symbol (XCHAT_PRI_LOWEST_SYMBOL)))
	priority = XCHAT_PRI_LOWEST;

    return priority;
}

static SCM
api_hook_command (SCM command_name,
		    SCM priority,
		    SCM scm_callback,
		    SCM help_text)
{
    xchat_hook *hook;

    scm_callback = scm_gc_protect_object (scm_callback);

    hook = xchat_hook_command (ph,
	    SCM_STRING_CHARS (command_name),
	    xchat_priority_symbol_to_int (priority),
	    command_callback,
	    SCM_STRING_CHARS (help_text),
	    (void *)  SCM_UNPACK (scm_callback));

    if (hook == NULL) {
	scm_gc_unprotect_object (scm_callback);
	return SCM_BOOL_F;
    }

    else {
	/* According to Andreas Rottmann casting of (void *) to a long and
	 * back is portable on all architectures. I hope he is correct.
	 */
	scm_lock_mutex (lock);
	all_hooks = g_slist_append (all_hooks, hook);
	scm_unlock_mutex (lock);
	return scm_long2num ((glong) hook);
    }
}

static gint 
print_callback (gchar *word[], void *user_data)
{
    struct scm_body_thunk_1_data c;
    SCM eat;

    c.body_proc = SCM_PACK ((scm_t_bits) (user_data));
    c.arg = scm_list_from_strarray ((const gchar **) word + 1);

    eat = scm_internal_catch (SCM_BOOL_T,
	    scm_body_thunk_1,
	    &c,
	    (scm_t_catch_handler) scm_handle_by_message_noexit,
	    NULL);

    return xchat_eat_symbol_to_int (eat);
}

static SCM
api_hook_print (SCM name,
		    SCM priority,
		    SCM scm_callback)
{
    xchat_hook *hook;

    scm_callback = scm_gc_protect_object (scm_callback);

    hook = xchat_hook_print (ph,
	    SCM_STRING_CHARS (name),
	    xchat_priority_symbol_to_int (priority),
	    print_callback,
	    (void *)  SCM_UNPACK (scm_callback));

    if (hook == NULL) {
	scm_gc_unprotect_object (scm_callback);
	return SCM_BOOL_F;
    }

    else {
	/* According to Andreas Rottmann casting of (void *) to a long and
	 * back is portable on all architectures. I hope he is correct.
	 */
	scm_lock_mutex (lock);
	all_hooks = g_slist_append (all_hooks, hook);
	scm_unlock_mutex (lock);
	return scm_long2num ((glong) hook);
    }
}

static SCM
api_hook_server (SCM name,
		    SCM priority,
		    SCM scm_callback)
{
    xchat_hook *hook;

    scm_callback = scm_gc_protect_object (scm_callback);

    hook = xchat_hook_server (ph,
	    SCM_STRING_CHARS (name),
	    xchat_priority_symbol_to_int (priority),
	    command_callback,
	    (void *)  SCM_UNPACK (scm_callback));

    if (hook == NULL) {
	scm_gc_unprotect_object (scm_callback);
	return SCM_BOOL_F;
    }

    else {
	/* According to Andreas Rottmann casting of (void *) to a long and
	 * back is portable on all architectures. I hope he is correct.
	 */
	scm_lock_mutex (lock);
	all_hooks = g_slist_append (all_hooks, hook);
	scm_unlock_mutex (lock);
	return scm_long2num ((glong) hook);
    }
}

static gint 
timer_callback (void *user_data)
{
    struct scm_body_thunk_data c;

    c.body_proc = SCM_PACK ((scm_t_bits) (user_data));
    c.tag = SCM_UNSPECIFIED;

    scm_internal_catch (SCM_BOOL_T,
	    scm_body_thunk,
	    &c,
	    (scm_t_catch_handler) scm_handle_by_message_noexit,
	    NULL);

    return 1;
}

static SCM
api_hook_timer (SCM timeout,
		    SCM scm_callback)
{
    xchat_hook *hook;

    scm_callback = scm_gc_protect_object (scm_callback);

    hook = xchat_hook_timer (ph,
	    scm_num2int (timeout, 0, NULL),
	    timer_callback,
	    (void *)  SCM_UNPACK (scm_callback));

    if (hook == NULL) {
	scm_gc_unprotect_object (scm_callback);
	return SCM_BOOL_F;
    }

    else {
	/* According to Andreas Rottmann casting of (void *) to a long and
	 * back is portable on all architectures. I hope he is correct.
	 */
	scm_lock_mutex (lock);
	all_hooks = g_slist_append (all_hooks, hook);
	scm_unlock_mutex (lock);
	return scm_long2num ((glong) hook);
    }
}
static SCM
real_unhook_func (void *data)
{
    void *user_data = xchat_unhook (ph, data);

    if (user_data != NULL) {
	SCM scm_callback = SCM_PACK ((scm_t_bits) (user_data));
	return scm_gc_unprotect_object (scm_callback);
    }

    else 
	return SCM_BOOL_F;
}

SCM
api_unhook (SCM hook_id)
{
    void *data = (void *) scm_num2long (hook_id, 0, NULL);

    scm_lock_mutex (lock);
    all_hooks = g_slist_remove (all_hooks, data);
    scm_unlock_mutex (lock);
    return real_unhook_func (data);
}

static void
unhook_func (gpointer data, gpointer user_data)
{
    real_unhook_func (data);
}

SCM
api_plugin_gui_add (SCM filename, SCM name, SCM description, SCM version)
{
    void * handle = xchat_plugingui_add (ph,
	    SCM_STRING_CHARS (filename),
	    SCM_STRING_CHARS (name),
	    SCM_STRING_CHARS (description),
	    SCM_STRING_CHARS (version),
	    NULL);

    return scm_long2num ((glong) handle);
}

SCM
api_plugin_gui_remove (SCM handle)
{
    xchat_plugingui_remove (ph, (void *) scm_num2long (handle, 0, NULL));

    return SCM_UNSPECIFIED;
}

void module_init (void *data)
{
    glong tc;
    scm_t_port *pt;

    /* Setting up the xchat output window as our default output ports */
    tc = scm_make_port_type ("xchat_console", xchat_fill_input, xchat_write);
    port = scm_new_port_table_entry (tc);
    SCM_SET_CELL_TYPE (port, tc | SCM_OPN | SCM_WRTNG);
    pt = SCM_PTAB_ENTRY (port);
    pt->rw_random = 0;
    scm_set_current_output_port (port);
    scm_set_current_error_port (port);

    /* Define our types */
    xchat_context_t = scm_make_smob_type ("xchat_context_t", 0);
    scm_set_smob_equalp (xchat_context_t, xchat_context_equalp);

    /* Register our API */
    scm_c_define_gsubr ("xchat:print", 1, 0, 0, api_print);
    scm_c_define_gsubr ("xchat:emit-print", 1, 0, 1, api_emit_print);
    scm_c_define_gsubr ("xchat:command", 1, 0, 0, api_command);
    scm_c_define_gsubr ("xchat:find-context", 2, 0, 0, api_find_context);
    scm_c_define_gsubr ("xchat:set-context", 1, 0, 0, api_set_context);
    scm_c_define_gsubr ("xchat:current-context", 0, 0, 0, api_current_context);
    scm_c_define_gsubr ("xchat:nick-compare", 2, 0, 0, api_nick_compare);
    scm_c_define_gsubr ("xchat:get-info", 1, 0, 0, api_get_info);
    scm_c_define_gsubr ("xchat:send-modes", 4, 0, 0, api_send_modes);
    scm_c_define_gsubr ("xchat:list-fields", 1, 0, 0, api_list_fields);
    scm_c_define_gsubr ("xchat:list-get", 1, 0, 0, api_list_get);
    scm_c_define_gsubr ("xchat:hook-command", 4, 0, 0, api_hook_command);
    scm_c_define_gsubr ("xchat:hook-print", 3, 0, 0, api_hook_print);
    scm_c_define_gsubr ("xchat:hook-server", 3, 0, 0, api_hook_server);
    scm_c_define_gsubr ("xchat:hook-timer", 2, 0, 0, api_hook_timer);
    scm_c_define_gsubr ("xchat:unhook", 1, 0, 0, api_unhook);
    scm_c_define_gsubr ("xchat:plugin-gui-add", 4, 0, 0, api_plugin_gui_add);
    scm_c_define_gsubr ("xchat:plugin-gui-remove", 1, 0, 0, api_plugin_gui_remove);
    scm_c_export ("xchat:print", "xchat:emit-print", "xchat:command", 
	    "xchat:find-context", "xchat:set-context", "xchat:current-context",
	    "xchat:nick-compare", "xchat:get-info", "xchat:send-modes",
	    "xchat:list-fields", "xchat:list-get", "xchat:hook-command",
	    "xchat:hook-print", "xchat:hook-server", "xchat:hook-timer",
	    "xchat:unhook", "xchat:plugin-gui-add", "xchat:plugin-gui-remove",
	    NULL);
}

gint
xchat_plugin_init (xchat_plugin * plugin_handle,
		gchar ** plugin_name,
		gchar ** plugin_desc,
		gchar ** plugin_version,
		gchar * arg)
{
    to_print = NULL;

    /* we need to save this for use with any xchat_* functions */
    ph = plugin_handle;

    /* publish our introduction */
    *plugin_name = PLUGIN_NAME;
    *plugin_desc = PLUGIN_DESC;
    *plugin_version = PLUGIN_VERSION;

    /* initializing the guile library */
    scm_init_guile ();

    lock = scm_gc_protect_object (scm_make_mutex ());

    scm_c_define_module("xchat-guile main", module_init, NULL);
    scm_c_use_module ("xchat-guile main");
    scm_c_use_module ("xchat-guile plugin-system");

    /* Hook our guile command */
    guile_cmd = xchat_hook_command (ph,
	    PLUGIN_COMMAND,
	    XCHAT_PRI_NORM,
	    guile_command,
	    PLUGIN_USAGE,
	    NULL);

    /* We must implement our version of xchat's load and unload commands */
    load_cmd = xchat_hook_command (ph,
	    "load",
	    XCHAT_PRI_NORM,
	    load_command,
	    NULL,
	    NULL);

    unload_cmd = xchat_hook_command (ph,
	    "unload",
	    XCHAT_PRI_NORM,
	    unload_command,
	    NULL,
	    NULL);

    /* A 5 second commercial is all that is needed to be famous :) */
    xchat_print (ph, PLUGIN_INFO);

    return 1;
}

gint
xchat_plugin_deinit (xchat_plugin * plugin_handle,
		gchar ** plugin_name,
		gchar ** plugin_desc,
		gchar ** plugin_version,
		gchar * arg)
{
    struct scm_body_thunk_data c;

    xchat_unhook (ph, guile_cmd);
    xchat_unhook (ph, load_cmd);
    xchat_unhook (ph, unload_cmd);

    c.body_proc = SCM_VARIABLE_REF (scm_c_lookup ("xchat:unload-all-plugins"));
    c.tag = SCM_UNSPECIFIED;

    scm_internal_catch (SCM_BOOL_T,
	    scm_body_thunk,
	    &c,
	    (scm_t_catch_handler) scm_handle_by_message_noexit,
	    NULL);

    scm_lock_mutex (lock);
    g_slist_foreach (all_hooks, unhook_func, NULL);
    g_slist_free (all_hooks);
    
    if (to_print != NULL)
	g_free (to_print);

    scm_unlock_mutex (lock);
    scm_gc_unprotect_object (lock);

    return 1;
}

