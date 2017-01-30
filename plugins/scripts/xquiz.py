__module_name__ = "xquiz"
__module_version__ = "1.11"
__module_description__ = "Quiz script for X-Chat"

# Requires X-CHAT 2.x with Python scripting interface plugin (or X-Chat Aqua 0.13.0 with Python (0.12.0 hangs))
# xquiz started life as TypusQuiz by Rensel & Laurentius
# Extensively modified by btobw for #quiz on oz.org - http://www.ircquiz.net/
# TypusQuiz used a timer to automatically award answers. It required manual input of question/answer at the time the quiz was being run.
# It also used multiple colours, and did not use describes to ask the question and award answers, so was not suitable for #quiz.
# In this version:
#       Most functions are accessed from buttons under the user list (so this option needs to be enabled)
#       xquiz reads questions and answers from a plain text question and answer file (question on one line, answer on the next)
#       Answers are awarded manually by highlighting a nick in the user list and clicking 'Award'
#       Make life easy for yourself by putting both xquiz.py and the question file(s) in your X-Chat directory
# btobw knows nothing about Python or X-Chat, so feel free to correct/modify/enhance. For example:
#       A config file that allows you to specifiy a few things like the location of question files,
#               and the last question asked in each file would be nice. Then, each time the script is loaded,
#               the user could pick up from where they last left off in a file.
#       Scoring for multiple nicks would be nice too

# Change log
# Ver 1.07
#   - loadqfile() was not closing the current file if there was one open. Fixed.
# Ver 1.08 - 4 March 2005
#   - added ranking.final() so final scores are clearer
# Ver 1.09 - 3 October 2005
#   - white space was a mixture of tabs and spaces; now uniformly spaces
# Ver 1.10 - 8 October 2005
#   - in Mac and Unix systems, trailing line terminators were not being removed totally when reading in the
#     questions and answers. Fixed.
# Ver 1.11 - 12 October 2005
#   - now saves your position in the current question file between sessions. The next time you load
#     xquiz and open the same question file as you used last time, it will jump to the position you were at
#     in the file at the end of the last quiz. File name and line number are saved in xquiz.conf in the
#     xchat config directory (e.g.: "/home/user/.xchat2" or "C:\Documents and Settings\User\Application Data\X-Chat 2"
#
# ISSUES: 'Unknown command' errors from xchat, so I guess I have missing 'return xchat.EAT_ALL' in one or more places
#         I don't get the error in the windows version, so it's a bit difficult for me to track down (and knowing
#         nothing about scripting doesn't help). If you know how to fix this, please let me know in #quiz or by email
#         to digormanATgmailDOTcom
#
# * epic Q1: What are Muharram, Rajab and Safar?
# Answer: months in the muslim calendar
# *  :Unknown command
# * epic A1: months in the muslim calendar - no winner
# Next Question: Who wrote The Great Train Robbery, The Terminal Man, The & romeda Strain?
# Next Answer: michael crichton
# * - :Unknown command

import xchat
import pickle
import os

optdefault={
        "lastqfname":    "questions.txt",
        "lastqfline":    0
        }

options=optdefault.copy()

xchatdir = xchat.get_info("xchatdir")
xqcontext = xchat.find_context(channel='#quiz')
if xqcontext is None:
        xqcontext = xchat

def isInt(a):
        if type(a) == type (1):
                return True
        else:
                return False

def isString(a):
        if type(a) == type ('string'):
                return True
        else:
                return False

class qfile:
        active = False
        fname = ''
        qfquestion = ''
        qfanswer = ''
        qflines = 0
        qfline = 0
        def loadquestions(self):
                # Open the selected question file
                loadoptions()
                xchat.command("getstr %s LOADQFILE QuestionFile:" % (options['lastqfname']))

        def qfopen(self):
                try:
                        #self.f = open(self.fname, 'r')
                        self.f = open(xchatdir+"/plugins/settings/"+self.fname, 'r')
                except:
                        print "Unable to open %s" % (self.fname)
                        return xchat.EAT_ALL
                self.active = True
                self.qflines = len( self.f.readlines() )
                self.qfline = 0
                if self.fname <> options['lastqfname']:
                        options['lastqfname'] = self.fname
                        options['lastqfline'] = 0
                        saveoptions()
                # Reset to start of file
                self.f.seek(0)
                print "%s lines in question file" % (self.qflines)
        def qfread(self):
                self.qfquestion = ''
                self.qfanswer = ''
                if self.qflines >= self.qfline + 2:
                        # Read in the question and answer on consecutive lines, and strip the newline at the end
                        self.qfquestion = self.f.readline().rstrip("\r\n")
                        self.qfanswer = self.f.readline().rstrip("\r\n")
                        self.qfline = self.qfline + 2
                        options['lastqfline'] = self.qfline
                else:
                        print "Out of questions in %s" % (self.fname)
                        options['lastqfline'] = 0
                        self.qfclose()
                        self.loadquestions()
        def qfclose(self):
                saveoptions()
                self.qflines = 0
                self.qfline = 0
                if self.active:
                        self.f.close()
                        self.active = False

class quiz:
        active = False
        chan = ''
        def start(self):
                self.active = True
                self.chan = xchat.get_info("channel")
                # We don't want to end up with duplicate buttons by mistake
                self.delbuttons()
                # User list buttons for running the quiz
                self.showbuttons()

        def end(self):
                saveoptions()
                self.delbuttons()
                self.active = False
                xchat.command("ADDBUTTON Help XQ help")
                xchat.command("ADDBUTTON Unload py unload xquiz")
                xchat.command("ADDBUTTON Start XQ start")

        def showbuttons(self):
                # User list buttons for running the quiz
                xchat.command("ADDBUTTON Help XQ help")              # show xquiz help
                xchat.command("ADDBUTTON Show XQ show")              # show the current question and answer (only you can see it)
                xchat.command("ADDBUTTON Ask XQ ask")                # ask the current question
                xchat.command("ADDBUTTON Hint XQ hint")              # give a hint for this question
                xchat.command("ADDBUTTON Award XQ award %s")         # award the question to the currently highlighted nick
                xchat.command("ADDBUTTON Discard XQ discard")        # show the answer if it has been asked, then go to the next question
                xchat.command("ADDBUTTON Final XQ final")            # end the quiz, showing scores
                xchat.command("ADDBUTTON Adjust XQ adjust %s")       # adjust the score for the currently highlighted nick

        def delbuttons(self):
                xchat.command("DELBUTTON Help")
                xchat.command("DELBUTTON Start")
                xchat.command("DELBUTTON Show")
                xchat.command("DELBUTTON Ask")
                xchat.command("DELBUTTON Hint")
                xchat.command("DELBUTTON Award")
                xchat.command("DELBUTTON Discard")
                xchat.command("DELBUTTON Final")
                xchat.command("DELBUTTON Adjust")
                xchat.command("DELBUTTON Unload")

class question:
        active = False
        asked = False
        question = ''
        answer = ''
        points = 1
        number = 0
        def nextquestion(self):
                if self.active:
                        self.cancel()
                self.getquestion()
                if self.active:
                        self.show()
        def getquestion(self):
                if options['lastqfline'] > thisqfile.qfline:
                        self.jumptolastqfline()
                else:
                        thisqfile.qfread()
                self.question = thisqfile.qfquestion
                self.answer = thisqfile.qfanswer
                if self.question == '' or self.answer == '':
                        self.active = False
                else:
                        self.active = True
        def increment(self):
                # When a question is actually asked (not just discarded)
                self.number = self.number + 1
                self.asked = True
        def getcurrentq(self):
                self.question = thisqfile.qfquestion
                self.answer = thisqfile.qfanswer
                if self.question == '' or self.answer == '':
                        self.active = False
                else:
                        self.active = True
                        self.show()
        def jumptolastqfline(self):
                jumptoline = options['lastqfline']
                #jump to a given line in the question file
                if options['lastqfline'] > 0:
                        if thisqfile.qfline > options['lastqfline']:
                                # Reset to start of file
                                thisqfile.f.seek(0)
                                thisqfile.qfline = 0
                                options['lastqfline'] = self.qfline
                                saveoptions()
                        while thisqfile.qfline < jumptoline:
                                thisqfile.qfread()
        def show(self):
                print "Next Question: %s" % (self.question)
                print "Next Answer: %s" % (self.answer)
        def cancel(self):
                self.question = ''
                self.answer = ''
                self.active = False
                self.asked = False
        def end(self):
                self.cancel()
                self.number = 0

class ranking:
        scores = {}
        ranking = []
        currentnick = ''
        def __getitem__(self, a):
                if isInt(a):
                        return (self.ranking[a], self.scores[self.ranking[a]])
                elif isString(a):
                        if self.scores.has_key(a):
                                return self.scores[a]
                        else:
                                return 0
                else:
                        return None
        def __len__(self):
                return len(self.scores)
        def add(self, name, points):
                if isString(name) and isInt(points):
                        if self.scores.has_key(name):
                                self.scores[name] += points
                        else:
                                self.scores[name] = points
                        self.rank()
        def rank(self):
                list = self.scores.items()
                self.ranking = []
                for i in list:
                        self.ranking.append((i[1], i[0]))
                self.ranking.sort()
                self.ranking.reverse()
        def reset(self):
                self.scores = {}
                self.ranking = []
        def show(self):
                self.rank()
                position = 0
                ranks = ''
                for i in self.ranking:
                        position = position + 1
                        ranks = ranks + '(' + str(i[0]) + ')<' + i[1] + '> '
                xqcontext.command("ME Scores: %s" % (ranks))
        def final(self):
                self.rank()
                position = 0
                ranks = ''
                for i in self.ranking:
                        position = position + 1
                        ranks = ranks + '(' + str(i[0]) + ')<' + i[1] + '> '
                xqcontext.command("ME FINAL SCORES: %s" % (ranks))

thisqfile = qfile()
thisquiz = quiz()
thisquestion = question()
thisranking = ranking()

def doquiz(word, word_eol, userdata):
        if len(word) < 2 :
                print "Usage is: XQ ask|hint|award|discard|show|scores|final|adjust|help"
                return xchat.EAT_ALL

        if word[1] == 'start' and not thisquiz.active:
                # If we are out of questions, or don't have a file open, load a question file ready to quiz
                if thisqfile.qflines == 0 or thisqfile.qflines <= thisqfile.qfline + 2:
                        thisqfile.qfclose()
                        thisqfile.loadquestions()
                else:
                        # We already have a question file open, so refresh the q and a
                        thisquestion.getcurrentq()
                if thisqfile.qflines > 0:
                        thisquiz.start()
                        print "Ready to quiz"

        elif word[1] == 'ask' and thisquiz.active:
                if thisquestion.active and thisquestion.asked:
                        # We have an active question that's already been asked - repeat it
                        xqcontext.command("ME (repeat) Q%s: %s" %  (str(thisquestion.number), thisquestion.question))
                        # And show the answer to the persons doing the asking
                        print "Answer: %s" % (thisquestion.answer)
                else:
                        if not thisquestion.active:
                                thisquestion.getquestion()
                        if thisquestion.active:
                                thisquestion.increment()
                                xqcontext.command("ME Q%s: %s" % (str(thisquestion.number), thisquestion.question))
                                # And show the answer to the persons doing the asking
                                print "Answer: %s" % (thisquestion.answer)

        elif word[1] == 'show' and thisquiz.active:
                # Show the current question and answer to the persons doing the asking
                if thisquestion.active:
                        thisquestion.show()

        elif word[1] == 'hint' and thisquiz.active:
                # Get a hint for this question and show in channel
                if thisquestion.active and thisquestion.asked:
                        xchat.command('getstr "." SHOWHINT Hint:')
                else:
                        print "You haven't asked a question"

        elif word[1] == 'award' and thisquiz.active:
                # Award the question to the selected nick
                if len(word) < 3:
                        print "Please select a nick to award"
                        return xchat.EAT_ALL
                if not thisquestion.active or not thisquestion.asked:
                        print "You haven't asked the question yet"
                        return xchat.EAT_ALL
                nick = word[2]
                if thisquestion.active and thisquestion.asked:
                        thisranking.add(nick, thisquestion.points)
                        xqcontext.command("ME A%s: %s - Well done %s !!!" %  (str(thisquestion.number), thisquestion.answer, word[2]))
                        thisranking.show()
                thisquestion.nextquestion()

        elif word[1] == 'discard' and thisquiz.active:
                # Give the answer to the current question and go to the next one
                if thisquestion.active and thisquestion.asked:
                        xqcontext.command("ME A%s: %s - no winner" % (str(thisquestion.number), thisquestion.answer))
                thisquestion.nextquestion()

        elif word[1] == 'adjust' and thisquiz.active:
                # Adjust the score of the selected nick
                if len(word) < 3:
                        print "Please select a nick to adjust"
                        return xchat.EAT_ALL
                thisranking.currentnick = word[2]
                xchat.command('getstr "1" ADJUSTSCORE %s:' % (word[2]))

        elif word[1] == 'scores' and thisquiz.active and len(thisranking):
                thisranking.show()

        elif word[1] == 'final' and thisquiz.active:
                if(len(thisranking)):
                        xqcontext.command("ME THE WINNER IS: %s (%s points)!!! Well done all who played!" % (thisranking.ranking[0][1],str(thisranking.ranking[0][0])))
                        thisranking.final()
                else:
                        xqcontext.command("ME Game over")
                xqcontext.command("ME If you would like to ask a quiz, visit http://www.ircquiz.net/ and follow the link to 'Scripts'")
                thisquestion.end()
                thisranking.reset()
                thisquiz.end()

        elif word[1] == 'help':
                print "HELP FOR xquiz:"
                print "INTERFACE:"
                print "  Although it is possible to run a quiz from the command line,"
                print "  it is easier if you have User List Buttons enabled."
                print "  To enable User List Buttons, go to Settings...Preferences...Interface...User List"
                print "  and check 'User list buttons enabled'"
                print "QUESTION FILES:"
                print "  * Question files are plain text files with alternating questions and answers"
                print "  * They must contain a question on one line and the answer on the next"
                print "  * They must not contain any blank lines"
                print "  * There is no need to number the questions, or to put Q: or A:"
                print "    at the beginning of the lines"
                print "  * Put the question file(s) in your X-Chat directory"
                print "GETTING STARTED:"
                print "  * Make sure 'User List Buttons' is enabled"
                print "  * Click the 'Start' button and type in the name of your question file"
                print "  * A new set of buttons is displayed:"
                print "      Ask:     Asks a question in channel (and shows you the answer)"
                print "      Hint:    Allows you to type in a hint which is shown in channel"
                print "      Award:   When someone answers correctly, highlight their nick"
                print "               in the user list and click 'Award'"
                print "      Final:   When you have finished your quiz, click on 'Final'"
                print "               to show the final scoreboard"
                print "THE OTHER BUTTONS:"
                print "      Show:    Displays the current question and answer for the asker (not in channel)"
                print "      Discard: Moves to the next question."
                print "               Shows the answer to the current one first, if it has been asked."
                print "      Adjust:  Adjust the score for the selected nick"
                print "               (enter a negative number to deduct point(s))"
                print "      Start:   Starts a new quiz. If there is currently a question file loaded,"
                print "               it continues from where it left off in that file."
                print "      Unload:  Unloads the script"
                print "      Help:    Displays this help"
                print "COMMAND LINE USAGE:"
                print "  /loadqfile <file>:            load a new question file"
                print "  /adjustscore <nick> <points>: adjust the score of <nick> by <points> points"
                print "  /showhint <hint>:             display a hint for the current question in channel"
                print "  /xq start:         load a question file and commence a quiz"
                print "  /xq ask:           ask the current question in channel"
                print "  /xq hint:          give a hint for the current question"
                print "  /xq show:          show the current question and answer (only you can see it)"
                print "  /xq award <nick>:  award the answer to the currently highlighted nick"
                print "  /xq discard:       show the answer to the current question (if it's been asked)"
                print "                     and move to the next one"
                print "  /xq adjust <nick>: adjust the score of <nick>; you are prompted for the points to adjust by"
                print "  /xq scores:        show the current rankings"
                print "  /xq final:         finish the quiz and show the scoreboard"
                print "  /xq help:          show this help"

        elif word[1] == 'unload':
                xchat.command("py unload xquiz")

        else:
                print "Unrecognised command. Usage is XQ ask|hint|award|discard|show|scores|final|adjust|help"
        return xchat.EAT_ALL
xchat.hook_command("XQ",doquiz,help="Usage: XQ <command>")

def showhint(word, word_eol, userdata):
        if len(word) < 2:
                print "Please enter a hint"
                return xchat.EAT_ALL
        xqcontext.command("ME H%s: %s" % (str(thisquestion.number), word_eol[1]))
        return xchat.EAT_ALL
xchat.hook_command("SHOWHINT",showhint,help="Usage: SHOWHINT <text>")

def adjustscore(word, word_eol, userdata):
        if len(word) < 2:
                print "Please select a nick first and enter the points to be added/subtracted"
                return xchat.EAT_ALL
        if len(word) == 3:
                nick = word[1]
                points = int(word[2])
        else:
                nick = thisranking.currentnick
                points = int(word[1])
        thisranking.add(nick, points)
        xqcontext.command("ME Adjusts score for %s by %s point(s)" %  (nick, str(points)))
        return xchat.EAT_ALL
xchat.hook_command("ADJUSTSCORE",adjustscore,help="Usage: ADJUSTSCORE <nick> <points>")

def loadqfile(word, word_eol, userdata):
        if len(word) < 2:
                print "Please enter the filename of the question file"
                if thisquiz.qfile == '':
                        thisquiz.end()
                return xchat.EAT_ALL
        if thisqfile.active:
                thisqfile.qfclose()
        thisqfile.fname = word_eol[1]
        thisqfile.qfopen()
        if thisqfile.qflines > 0:
                thisquiz.start()
                print "Question file %s loaded" % (thisqfile.fname)
                # Load the first question
                thisquestion.getquestion()
                thisquestion.show()
        return xchat.EAT_ALL
xchat.hook_command("LOADQFILE",loadqfile,help="Usage: LOADQFILE <filename>")

def changenick(word, word_eol, userdata):
        if thisranking.scores.has_key(word[0]):
                oldnick = word[0]
                newnick = word[1]
                pointsnick = int(thisranking.scores[oldnick])
                thisranking.scores[newnick] = pointsnick
                thisranking.scores.__delitem__(oldnick)
        return xchat.EAT_NONE
xchat.hook_print("Change Nick",changenick)

def xqunload(userdata):
        thisquiz.delbuttons()
        thisqfile.qfclose()
        print "%s unloaded" % (__module_name__)
xchat.hook_unload(xqunload)

def saveoptions():
        f = open(xchatdir + "/xquiz.conf", "w")
        pickle.dump(options, f)
        f.close()

def loadoptions():
        global options
        try:
                f = open(xchatdir + "/xquiz.conf", "r")
                options = pickle.load(f)
                f.close()
        except IOError:
                print "error loading options"

# Start off with buttons to start the quiz or to unload the script
loadoptions()
thisquiz.end()
print "%s %s loaded" % (__module_name__, __module_version__)
print "X-Chat Version %s" % (xchat.get_info('version'))
print "Type /XQ help for help"
