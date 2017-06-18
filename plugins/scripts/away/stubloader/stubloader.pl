# Stubloader v1.0
# (c) Weevil 2002, altho its not really worth it.
#
# Loads xchat scripts found in places other than data dir (handy for
# development)

IRC::register("Stubloader","1.0","","");

my $myname = "stubloader.pl";

open(SCRIPTS,"stubloader.conf");
my @scripts;
while(<SCRIPTS>) {
	my $line = $_;
	$line =~ s/^\s*(.*?)\s*$/$1/;
	if($line =~ /^#/ || $line =~ /^\s*$/) { next };

	my @files = ($line);
	while(@files) {
		my $line = shift(@files);
		if(-d $line && $line ne "." && $line ne "..") {
			push(@files,<$line/*>);
		}
		elsif($line =~ /\.pl$/) {
			$line =~ /([^\/]+)$/;
			my $file = $1;
			if(grep($_ =~ /$file$/i, @scripts)) {
				IRC::print("Stubloader\tSkipping: $line (duplicate)\n");
			} else {
				push(@scripts,$line)
			}
		}
	}
}
close(SCRIPTS);

if(!@scripts) {
	IRC::print("Stubloader\tNo scripts found\n");
}
while(@scripts) {
	my $file = shift(@scripts);
	$file =~ s/\/+/\//g;
	if($file !~ /$myname$/i) {
		IRC::print("Stubloader\tLoading : $file\n");
		IRC::command("/LOAD $file\n");
	}
	else {
		IRC::print("Stubloader\tSkipping: $file (stubloader)\n");
	}
}
