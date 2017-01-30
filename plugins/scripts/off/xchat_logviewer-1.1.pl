#!/usr/bin/perl
# Copyright (c) 2005, ongeboren <ongeboren@gmail.com>
# Homepage: http://unborn.ludost.net/
# IRC: ongeboren @ irc.lirex.com (UniBG)
# This software is distributed under the GPL license.
Xchat::register( "Log viewer by ongeboren", "1.1",
 "Displays the log file, split into conversations in a special window");
Xchat::hook_command( "viewlog", view_log);
use Gtk2 -init;
use Gtk2::SimpleList;
my $verbose = 0;
sub view_log {
	my $window = Gtk2::Window->new ('toplevel');
	my $vbox = Gtk2::VBox->new (0,0);
	my $hbox = Gtk2::HBox->new (0,0);
	my $hbox2 = Gtk2::HBox->new (0,10);
	my $slist = Gtk2::SimpleList->new ("Date of conversation", "text");
	my $tview = Gtk2::TextView->new;
	my $scrol = Gtk2::ScrolledWindow->new (undef, undef);
	my $scrol2 = Gtk2::ScrolledWindow->new (undef, undef);
	my $vsptr = Gtk2::VSeparator->new;
	my $butt_box = Gtk2::VButtonBox->new;
	my $butto = Gtk2::Button->new_with_label ("Close");
	my $label = Gtk2::Label->new("double-click on a conversation");
	my $buff = Gtk2::TextBuffer->new;
	$window->set_default_size (1200,900);
	$window->set_title ("Log viewer v1.1 by ongeboren");
	$window->signal_connect (destroy => sub { close_event($window) } );
	$slist->set_data_array ( [process_the_log_file()] );
  $slist->signal_connect (row_activated => sub {
   my ($sl, $path, $column) = @_;
	 my $row_ref = $sl->get_row_data_from_path ($path);
	 my $buf = Gtk2::TextBuffer->new;
	 $buf->set_text ( $log_hash{$$row_ref[0]} );
	 $tview->set_buffer ( $buf );
	});
	$buff->set_text ( $log_hash{ $slist->{data}[0][0] } );
	$tview->set_editable (0);
	$tview->set_wrap_mode ("word");
	$tview->set_left_margin (6);
	$tview->set_buffer ( $buff );
	$scrol->set_policy (GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	$scrol->add_with_viewport ( $tview );
	$scrol2->set_policy (GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	$scrol2->add_with_viewport ( $slist );
  $butto->signal_connect (clicked => sub{ close_event($window) });
	$hbox->add ( $scrol2 );
	$hbox->add ( $vsptr );
	$hbox->add ( $scrol );
	$butt_box->add ($butto);
	$hbox2->add ( $label );
	$hbox2->add ( $butt_box );
	$vbox->add ( $hbox );
	$vbox->add ( $hbox2 );
	$window->add ( $vbox );
	$hbox->set_child_packing ($scrol2,0,0,6,start);
	$hbox->set_child_packing ($vsptr,0,0,0,start);
	$hbox->set_child_packing ($scrol,1,1,6,end);
	$hbox2->set_child_packing ($butt_box,0,0,6,end);
	$hbox2->set_child_packing ($butto,0,0,0,start);
	$hbox2->set_child_packing ($label,0,0,6,start);
	$vbox->set_child_packing ($hbox2,0,0,6,end);
	$vbox->set_child_packing ($hbox,1,1,6,start);
	$window->show_all;
	return Xchat::EAT_ALL;
}
sub construct_fname {
	my $logmask = Xchat::get_prefs("irc_logmask");
	my $fname = Xchat::get_info("xchatdir") ."/xchatlogs/";
	my $ii = 0;
	for ($ii = 0; $ii < length($logmask); $ii++) {
		if (substr($logmask,$ii,1) eq '%') {
			if (substr($logmask,++$ii,1) eq 'c') {
				$fname .= lc(Xchat::get_info("channel"));
			} elsif (substr($logmask,$ii,1) eq 'n') {
				$fname .= Xchat::get_info("network");
			} elsif (substr($logmask,$ii,1) eq 's') {
				$fname .= Xchat::get_info("server");
			}
		} else {
			$fname .= substr($logmask,$ii,1);
		}
	}
	return $fname;				
}
sub process_the_log_file {
	my $fname = construct_fname();
	my $conv = 0;
	my $text = "";
	my @ret;
	open (IN, $fname) or die "can't open the file ($fname): $!\n";	
	while (<IN>) {
		chomp($_);
		if (/^\*\*\*\* BEGIN LOGGING AT (.*)$/) {
			if ($conv) {
				Xchat::print "warning: conversation ($conv) not ended. "
				            ."Forcing end.. Log: $fname\n" if ($verbose);
				$log_hash{$conv} = $text;
				unshift(@ret, $conv);
			}
			$conv = $1;
			$text = "$_\n";
		} elsif (/^\*\*\*\* ENDING LOGGING AT (.*)$/) {
			if (!$conv) {
				Xchat::print "error: ending an empty conversation. "
				            ."Ignoring.. Log: $fname\n" if ($verbose);
			} else {
				$text .= "$_\n";
				$log_hash{$conv} = $text;
				unshift(@ret, $conv);
				$conv = 0;
				$text = "";
			}
		} else { $text .= "$_\n" if ($conv); }
	}
	close(IN);
	return @ret;
}
sub close_event {
	my ($w) = @_;
	undef %log_hash;
	$w->destroy();
}
Xchat::print("View log v1.1 by ongeboren loaded.");
