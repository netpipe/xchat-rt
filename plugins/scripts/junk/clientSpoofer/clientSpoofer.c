#include <xchat/xchat-plugin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNAME "Client Spoofer"
#define PDESC "Responds to CTCP versions with a string of your choice, and bars all other replies";
#define PVERSION "0.1"
#define MAXLEN 400

static xchat_plugin *ph;   /* plugin handle */
static char spoofClient[MAXLEN];
static char outp[15];
static char *colPos;
static char *exPos;

static int spoof_cb(char *word[], char *word_eol[], void *userdata)
{
   char *tempPoint;
   int i = 0;
   if(strstr(word_eol[4], ":VERSION") != NULL) {
      colPos = strstr(word[1],":");
      exPos = strstr(word[1],"!");      
      tempPoint = colPos;
      for(tempPoint = ++colPos; tempPoint < exPos ; tempPoint++) {
         outp[i++] = *tempPoint;
      }
      outp[i]='\0';
      xchat_commandf(ph,"NCTCP %s VERSION %s", outp, spoofClient);
   }
   

   return XCHAT_EAT_NONE;  /* don't eat this event, xchat needs to see it! */
}

static int change_client_cb(char *word[], char *word_eol[], void *userdata) {
   FILE *writeMe = fopen(".clientSpoof","w");
   strcpy(spoofClient,word_eol[2]);
   fprintf(writeMe,"%s",spoofClient);
   fclose(writeMe);
   xchat_printf(ph,"Currenty spoofing: %s",spoofClient);
   return XCHAT_EAT_ALL;
}

static int check_client_cb(char *word[], char *word_eol[], void *userdata) {
   xchat_printf(ph,"Currenty spoofing: %s",spoofClient);
   return XCHAT_EAT_ALL;
}

void xchat_plugin_get_info(char **name, char **desc, char **version, void **reserved)
{
   *name = PNAME;
   *desc = PDESC;
   *version = PVERSION;
}

int xchat_plugin_init(xchat_plugin *plugin_handle,
                      char **plugin_name,
                      char **plugin_desc,
                      char **plugin_version,
                      char *arg)
{
   char c = 1;
   int i = 0;
   /* we need to save this for use with any xchat_* functions */
   ph = plugin_handle;

   /* tell xchat our info */
   *plugin_name = PNAME;
   *plugin_desc = PDESC;
   *plugin_version = PVERSION;

   xchat_hook_server(ph, "PRIVMSG", XCHAT_PRI_NORM, spoof_cb, NULL);
   xchat_hook_command(ph, "SPOOF", XCHAT_PRI_NORM, change_client_cb,"Usage: SPOOF <text>\nThe exact text to spoof when VERSION'd", NULL);
   xchat_hook_command(ph, "SPOOFX", XCHAT_PRI_NORM, check_client_cb,"Usage: SPOOFX\nEchoes the string that will be sent when version'd", NULL);
   FILE *readMe = fopen(".clientSpoof","r+");
   while(c != EOF) {
      c = fgetc(readMe);
      spoofClient[i++] = c;
   }
   spoofClient[i-1] = '\0';
   fclose(readMe);
   xchat_print(ph, "Client Spoofer loaded successfully!\n");
   return 1;       /* return 1 for success */
}
