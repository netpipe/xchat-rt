# You can place a config file in X-Chat's data directory named simple_display.cfg
# See http://xchat.org/faq/#q218 for the location of the data directory
#
# The format of the config file is
#     key:value
#
# Currently the only accepted keys are "search_path" and "create_aliases",
# there can be multiple entries for the search path.
# - Files are searched in the order listed in the config file.
# - X-Chat's data director and your home directory are searched if a file is
#   not found in the configured search paths.

use strict;
use warnings;
use Xchat qw(:all);
use File::Spec ();
use File::Glob ();
use Getopt::Long;
use Cwd ();

register( "Simple Display", "1.0.6" );

my $config = load_config( 0 );
my %currently_playing;

if( $config->{ "create_aliases" } ) {
	my $cmd_char = get_prefs "input_command_char";
	hook_command( "PLAY_NOTICE", sub {
		my @words = @{$_[0]};
		cmd_play( [ $words[0], "--notice", @words[ 1 .. $#words ] ] );

		return EAT_XCHAT;
	},
	{
		help_text => "Equivalent to ${cmd_char}PLAY --notice",
	});

	hook_command( "PLAY_ACTION", sub {
		my @words = @{$_[0]};
		cmd_play( [ $words[0], "--action", @words[ 1 .. $#words ] ] );

		return EAT_XCHAT;
	},
	{
		help_text => "Equivalent to ${cmd_char}PLAY --action",
	});
}

sub parse_options {
	my $specs = shift;
	my $remainder = shift;
	local @ARGV = @{shift()};

	my $result = GetOptions( %$specs );

	if( $result ) {
		@$remainder = @ARGV;
	}

	return $result;
}

hook_command( "PLAY", \&cmd_play, { help_text => usage_message(), } );
sub cmd_play {
	my @words = @{$_[0]};
	my ($interval, $file);
	my ($msg, $action, $notice);
	my $remainder = [];

	unless (parse_options( {
		"msg" => \$msg,
		"notice" => \$notice,
		"action" => \$action,
		"interval|i=i" => \$interval,
		"file|f=s" => \$file,
	}, $remainder, [ @words[ 1 .. $#words ] ] )) {
		warn "Option parsing failed";
		show_usage();
		return EAT_XCHAT;
	}

	if( not $interval and @$remainder ) {
		$interval = shift @$remainder;
	}

	if( not $file and @$remainder ) {
		$file = join " ", @$remainder;
		trim( $file );
	}

	if( (grep { $_ } ($msg, $action, $notice)) > 1 ) {
		warn "Only one of --msg, --notice, --action may be used at a time";
		return EAT_XCHAT;
	}

	my $type = "msg";
	if( $notice ) {
		$type = "notice";
	} elsif( $action ) {
		$type = "action";
	}

	_play( { interval => $interval, file => $file, type => $type } );

	return EAT_XCHAT;
}

sub _play {
	my $opts = shift;
	my $interval = $opts->{interval};
	my $file = $opts->{file};

	unless( $interval && $file ) {
		show_usage();
		return;
	}

	$file = resolve_file( $file );

	my $context = get_context;

	if( $currently_playing{ $context } ) {
		prnt "Already playing $currently_playing{ $context }{file}";
	} elsif( my $fh = get_fh( $file ) ) {
		$interval *= 1_000;
		$currently_playing{ $context } = {
			file => $file,
			hook => create_timer({
				interval => $interval,
				handle => $fh,
				type => $opts->{type},
			}),
		};
	}
	return;
}

hook_command( "STOP", \&cmd_stop );
sub cmd_stop {
	my $context = get_context;
	if( $currently_playing{ $context } ) {
		unhook( $currently_playing{ $context }{hook} );
		delete $currently_playing{ $context };
	}
	return EAT_XCHAT;
}

my %output_type = (
	msg => sub {
		my $line = shift;
		command( "SAY $line" );
	},
	notice => sub {
		my $line = shift;
		commandf( "NOTICE %s %s", get_info( "channel" ), $line );

	},
	action => sub {
		my $line = shift;
		command( "ME $line" );
	}
);

sub remove_nl {
	s/(?:\r|\n)+\z// for @_;
}

sub create_timer {
	my $opts = shift;
	my $interval = $opts->{interval};
	my $handle = $opts->{handle};
	my $type = $opts->{type};

	return hook_timer( $interval, sub {
		remove_nl( my $line = <$handle> );
		$output_type{ $type }->( $line ) if $line =~ /\S/;

		if( eof $handle ) {
			close $handle;
			delete $currently_playing{ get_context() };
			return REMOVE;
		} else {
			return KEEP;
		}
	});
}

sub show_usage {
	prnt usage_message();
}

sub usage_message {
	return <<"USAGE";
PLAY <seconds> <path to file>
PLAY [--msg | --notice | --action] <seconds> <path to file>
PLAY [--msg | --notice | --action] --interval <seconds> --file <path to file>
USAGE
}

sub resolve_file {
	my $file = expand_tilde( shift );

	if( File::Spec->file_name_is_absolute( $file ) ) {
		return $file;
	}

	my $abs_file;
	my @search_path = (
		map { expand_tilde( $_ ) } @{$config->{search_path}},
		get_info( "xchatdirfs" ),
		home_dir(),
	);

	SEARCH:
	for my $search_dir ( @search_path ) {
		my $abs_file = File::Spec->catfile( $search_dir, $file );

		if( -f $abs_file ) {
			$file = $abs_file;
			last SEARCH;
		}
	}

	return Cwd::abs_path( $file );
}

sub home_dir {
	return File::Glob::bsd_glob( "~" );
}

sub expand_tilde {
	my $path = shift;
	my $home_dir = home_dir();

	$path =~ s/^~/$home_dir/;
	return $path;
}

sub get_fh {
	my $path = shift;
	my $warn = shift;
	$warn = 1 unless defined $warn;

	if( open my $fh, "<", $path ) {
		return $fh;
	} else {
		warn "Unable to open '$path': $!" if $warn;
		return;
	}

}

sub get_config_fh {
	my $path = File::Spec->catfile(
		get_info( "xchatdirfs" ),
		"simple_display.cfg",
	);

	my $warn = shift;
	return get_fh( $path, $warn );
}

sub load_config {
	my $warn = shift;

	my %conf;
	if( my $config_fh = get_config_fh( $warn ) ) {

		while( my $line = <$config_fh> ) {
			chomp $line;
			my ($key, $value) = split /:/, $line;
			trim( $key, $value );
			push @{$conf{$key}}, $value;
		}

	}

	$conf{search_path} ||= [];
	$conf{create_aliases} ||= 1;
	return \%conf;
}

sub trim {
	s/^\s+//, s/\s+$// for @_;
}

hook_command( "SD", \&cmd_sd,
	{ help_text => sd_usage_message(), }
);

my %commands = (
	reload => sub { $config = load_config( 1 ) },
	active => sub {
		my $context = get_context;
		if( $currently_playing{ $context } ) {
			prnt "Currently playing $currently_playing{ $context }{file}";
		} else {
			prnt "Nothing is currently being played"
		}
	},
	stop => \&cmd_stop,
	play => \&_play,
);

sub cmd_sd {
	my @args = @{$_[0]};
	shift @args;

	my $cmd = lc( shift @args );

	my $action = $commands{ $cmd } || \&sd_usage;
	$action->(@args);

	return EAT_XCHAT;
}

sub sd_usage_message {
	my $cmd_char = get_prefs "input_command_char";
	return <<"USAGE";
Usage: SD reload # reload config from disk
       SD active # shows what file is currently being played
		 SD stop   # same as ${cmd_char}STOP
		 SD play   # same as ${cmd_char}PLAY
USAGE

}

sub sd_usage {
	prnt sd_usage_message();
}
