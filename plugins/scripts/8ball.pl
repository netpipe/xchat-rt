#!/usr/bin/perl

# this piece of code is public domain
# written by jlh (jlh at gmx dot ch)

use strict;
use warnings;

my $script_name = '8ball';
my $script_version = '0.1.6';
my $script_description = "replies to '!8ball question?'";

# prefix for replies:
my $reply_prefix = '~ Magic 8-ball says: ';

# send a message in a delayed manner.  this is useful if you want
# things to be shown in correct order on your own screen.

sub msg_delayed {
	my ($to, $msg) = @_;
	my $context = Xchat::get_context();
	Xchat::hook_timer(0,
		sub {
			Xchat::set_context($context);
			Xchat::command("msg $to $msg");
			return 0;
		}
	);
}

my @eightball_decided = (
	"Signs point to yes.",
	"Yes.",
	"Most likely.",
	"Without a doubt.",
	"Yes - definitely.",
	"As I see it, yes.",
	"You may rely on it.",
	"Outlook good.",
	"It is certain.",
	"It is decidedly so.",

	"My sources say no.",
	"Very doubtful.",
	"My reply is no.",
	"Outlook not so good.",
	"Don't count on it.",
);

my @eightball_undecided = (
	"Reply hazy, try again.",
	"Better not tell you now.",
	"Ask again later.",
	"Concentrate and ask again.",
	"Cannot predict now.",
);

my @eightball_repeat = (
        "This question sounds familiar...",
        "Didn't you ask this already earlier?",
        "I'm having a deja vu...",
        "I told you that already.",
        "Are you repeating because my previous answer wasn't satisfying?",
        "Not telling you again.",
        "Pffft, I won't change my mind.",
        "Stop repeating.",
);

my @eightball_question = (
	"This doesn't look like a question to me.",
	"Questions usually have question marks at the end.",
	"And where's the question mark?",
	"I only answer questions and that isn't one.",
	"You need to put a question mark at the end.",
	"Let me introduce you to Mr. Grammar!  He lets you know that questions must end with a question mark.",
);

my @eightball_unhandled = (
	"I can't answer this type of question.",
	"Sorry, I don't know.",
	"That's a too difficult question.",
	"I have no clue about this.",
	"I can only handle yes/no questions.",
	"That doesn't look like a yes/no question.",
);

my @eightball_special = (
	"Infinitely #.",
	"Impressively #.",
	"Really really #.",
	"Remarkably #.",
	"Decidedly #.",
	"Very very #.",
	"Very #.",
	"Fairly #.",
	"Pretty #.",
	"Rather #.",
	"Not so #.",
	"Not very #.",
	"Not # at all.",
	"Laughably #.",
	"Humiliatingly #.",
);

my %eightball_memory;

# handle !8ball

sub eightball {
	my ($reply_to, $str) = @_;
	my $t = time;

	# clean up %eightball_memory
	for (keys %eightball_memory) {
		delete $eightball_memory{$_}
			if ($eightball_memory{$_}{t} + 600 < $t);
	}

	my $id = "$reply_to $str";

	# question mark is required
	if ($str !~ /\?/) {
		&msg_delayed($reply_to, $reply_prefix . $eightball_question[int rand @eightball_question]);
		return;
	}

	if (exists $eightball_memory{$id}) {
		&msg_delayed($reply_to, $reply_prefix . $eightball_repeat[int rand @eightball_repeat]);
		$eightball_memory{$id}{t} = $t;
		return;
	}

	my $special = undef;

	# default especially handles these:
	# ^am i\b ^are\b ^can\b ^did\b
	# ^do\b ^does\b ^has\b ^have\b
	# ^is\b ^r\b ^should\b ^shuld\b
	# ^shud\b ^shod\b ^u\b ^want\b
	# ^wanna\b ^was\b ^were\b ^will\b
	# ^may\b

	if ($str =~ /^how (?:big|small|huge|tiny)\b/i) {
		$special = 'big,small,huge,tiny';
	} elsif ($str =~ /^how (?:long|short)\b/i) {
		$special = 'long,short';
	} elsif ($str =~ /^how (?:wide|narrow)\b/i) {
		$special = 'wide,narrow';
	} elsif ($str =~ /^how (?:good|bad)\b/i) {
		$special = 'good,bad';
	} elsif ($str =~ /^how (?:old|young)\b/i) {
		$special = 'old,young';
	} elsif ($str =~ /^how (much|many|sexy|happy|far|tall|hard|lame)\b/i) {
		$special = $1
	} elsif ($str =~ /^(?:am I|is \w+) (much|many|sex(?:y|i)|happ(?:y|i)|far|tall|hard|lame)(?:er|r)?\b/i) {
		$special = $1
	} elsif ($str =~ /^(?:what|where|who|how|why|whence|whereabout|whose)\b/i) {
		$special = '';
	}

	if (not defined $special) {
		my $n = int rand(@eightball_decided + @eightball_undecided);
		my $s = $n < @eightball_decided ? $eightball_decided[$n] : $eightball_undecided[$n - @eightball_decided];
		&msg_delayed($reply_to, $reply_prefix . $s);
		if ($n < @eightball_decided) {
			$eightball_memory{$id} = { t => $t, s => $s };
		}
	} elsif ($special eq '') {
		my $s = $eightball_unhandled[int rand @eightball_unhandled];
		&msg_delayed($reply_to, $reply_prefix . $s);
		$eightball_memory{$id} = { t => $t, s => $s };
	} else {
		my @w = split /,/, $special;
		my $w = $w[int rand @w];
		my $n = int rand(@eightball_special + @eightball_undecided);
		my $s = $n < @eightball_special ? $eightball_special[$n] : $eightball_undecided[$n - @eightball_special];
		$s =~ s/#/$w/;
		&msg_delayed($reply_to, $reply_prefix . $s);
		if ($n < @eightball_special) {
			$eightball_memory{$id} = { t => $t, s => $s };
		}
	}
}

# handle !decide

sub decide {
	my ($reply_to, $str) = @_;
	my @opts;

	while (1) {
		$str =~ s/^\s*//;
		last if ($str eq '');

		if ($str =~ s/^"(.*?)"//) {
			push @opts, $1;
		} else {
			$str =~ s/^(\S+)//;
			push @opts, $1;
		}
	}

	my $n = $#opts + 1;
	return unless ($n >= 2);

	&msg_delayed($reply_to, $reply_prefix . $opts[int(rand $n)]);
}

my $last_check = 0;
my $question_points_max = 10; 
my $question_points = $question_points_max;
my $did_warn = 0;

sub update_question_points {
	my $time = time;
	my $new += int(($time - $last_check) / 60);
	$question_points += $new;

	if ($question_points >= $question_points_max) {
		$question_points = $question_points_max;
		$did_warn = 0;
		$last_check = $time;
	} else {
		$last_check += $new * 60;
	}
}

sub check_question_points {
	my $reply_to = shift;
	&update_question_points;
	if ($question_points <= 0) {
		if (!$did_warn) {
			&msg_delayed($reply_to, $reply_prefix . "Wisdom resources expired, please wait until I recover.");
			$did_warn = 1;
		}
		return 0;
	}
	$question_points--;
	return 1;
}

# handle a message line

sub check {
	my ($reply_to, $msg) = @_;

	$msg = Xchat::strip_code($msg);

	if ($msg =~ /^!8ball,?\s+(.+?)\s*$/) {
		return unless &check_question_points($reply_to);
		&eightball($reply_to, $1);
	} elsif ($msg =~ /^!decide\s+(.*?)\s*$/) {
		return unless &check_question_points($reply_to);
		&decide($reply_to, $1);
	}
}

# hand over other people's messages to &check()

sub cb_privmsg {
	my ($from, undef, $to, $msg) = split(/ +/, $_[1][0], 4);
	# extract nick
	$from =~ s/^:([^!]*)!.*$/$1/;
	my $reply_to = ($to =~ /^#/ ? $to : $from);
	$msg =~ s/^://;
	&check($reply_to, $msg);
	return Xchat::EAT_NONE;
}

# hand over our own messages to &check()

sub cb_input {
	# note that get_info('channel') returns a nick if we're in a private conversation
	&check(Xchat::get_info('channel'), $_[1][0]);
	return Xchat::EAT_NONE;
}

Xchat::register($script_name, $script_version, $script_description);
Xchat::hook_server('PRIVMSG', 'cb_privmsg');
Xchat::hook_command('', "cb_input");

Xchat::print("$script_name $script_version loaded");

return 1;
