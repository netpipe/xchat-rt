    * #!/usr/bin/perl -w
    * # Away Script pour x-chat basé sur le script Hyperion
    * # Version 1.0 du 14/07/2005
    *
    * ###############################################################################################"
    * # CONFIGURATION
    * ###############################################################################################"
    *
    * # chemin du fichier conservant les logs
    * $hlog_file = "~/.hyperion.awaylog";
    *
    * # complément du pseudo à utiliser lors du passage en absence
    * $haway_nick = "[away]";
    *
    * # raison par defaut de l'absence
    * $hdefreason = "parti, donc plus la c'est à dire absent !";
    *
    * # effacer les logs apres les avoir lus
    * $hautoclear = 1;
    *
    * ###############################################################################################"
    *
    * sub make_my_time;
    * sub hyp_log;
    * sub wide_msg;
    * sub join_handler;
    * sub privmsg_handler;
    * sub goaway_handler;
    * sub comeback_handler;
    * sub vlog_handler;
    * sub clog_handler;
    *
    * $hreason=$hdefreason;
    *
    * IRC::register("Away", "1.00", "", "");
    *
    * IRC::print "\002\0034:: Chargement du script: Away v1.0 ::\n";
    *
    * IRC::add_command_handler("away", "goaway_handler");
    * IRC::add_command_handler("back", "comeback_handler");
    * IRC::add_command_handler("vlog", "vlog_handler");
    * IRC::add_command_handler("clog", "clog_handler");
    *
    * IRC::add_message_handler("PRIVMSG", "privmsg_handler");
    * IRC::add_print_handler("Join", "join_handler");
    *
    * IRC::print "\002\0034Away - Les commandes\n\n";
    * IRC::print "\002\0033/away [raison] -- Se mettre absent\n";
    * IRC::print "\002\0033/back -- Etre de retour\n";
    * IRC::print "\002\0033/vlog -- Voir les logs\n";
    * IRC::print "\002\0033/clog -- Effacer les logs\n";
    *
    * sub make_my_time {
    * ($sec, $mun, $hr, $mday, $mon, $yr, $wday, $yday, $isdist) = localtime(time);
    * return sprintf("%02d:%02d:%02d %02d/%02d/%04d", $hr, $min, $sec, $mon + 1, $mday, $yr + 1900);
    * }
    *
    * sub hyp_log {
    * open HALOG, ">>$hlog_file";
    * printf HALOG "%s: %s\n", make_my_time(), @_;
    * close HALOG;
    * }
    *
    * sub wide_msg {
    * my $text = join(' ', @_);
    * my @channels = IRC::channel_list();
    * while (@channels) {
    * $nick = pop(@channels);
    * $serv = pop(@channels);
    * $chan = pop(@channels);
    * $es = IRC::get_info(3);
    * $deb = substr($chan, 0, 1);
    * IRC::send_raw "PRIVMSG $chan :$text\n";
    * if ($deb eq "#") {
    * IRC::command_with_server(sprint_f("/notice %s %s %s", $chan, $nick, $text), $serv);
    * }
    * }
    * }
    *
    * sub join_handler {
    * $line = join(' ', @_);
    * $line =~ s/\s+/ /g;
    * @ar = split ' ', $line;
    * }
    *
    * sub privmsg_handler
    * {
    * if ($haway == 0) {
    * return 0;
    * }
    * my $line = shift(@_);
    * my $n = IRC::get_info(1);
    * $line =~ /:(.*)!(.*@.*) .*:(.*)/;
    * my $u= $1;
    * my $l= $2;
    * my $m= $3;
    * $n.= "\$";
    * if($l =~ / $n/) {
    * hyp_log "[$u] $m";
    * IRC::send_raw "PRIVMSG $u : Je suis absent ($hreason) mais vos messages en privé seront enregistrés.\n";
    * return 1;
    * }
    * return 0;
    * }
    *
    * sub goaway_handler
    * {
    * $hreason = shift(@_);
    * if ($haway == 1) {
    * IRC::print "\002\0034:: VOUS ETES DEJA ABSENT ::\n";
    * return 1;
    * }
    * hyp_log "-- ABSENCE --";
    * IRC::print "\002\0034:: PASSAGE EN ABSENCE ::\n";
    * if(!$hreason) {
    * $hreason = $hdefreason;
    * }
    * wide_msg "s'absente ($hreason) mais vos messages en privé seront enregistrés.";
    * $hold_nick = IRC::get_info(1);
    * @servers = IRC::server_list();
    * foreach (@servers) {
    * IRC::command_with_server(sprintf("/nick %s%s", $hold_nick, $haway_nick), "$_");
    * }
    * $haway = 1;
    * return 1;
    * }
    *
    * sub comeback_handler
    * {
    * if($haway == 0) {
    * IRC::print "\002\0034:: VOUS N'ETES PAS ABSENT ::\n";
    * return 1;
    * }
    * IRC::print "\002\0034:: RETOUR DE VOTRE ABSENCE ::\n";
    * @servers = IRC::server_list();
    * foreach (@servers) {
    * IRC::command_with_server(sprintf("/nick %s", $hold_nick), "$_");
    * }
    * $haway = 0;
    * wide_msg "est de retour!";
    * hyp_log "-- RETOUR --";
    * return 1;
    * }
    *
    * sub vlog_handler {
    * IRC::print "\n\002\0034:: MESSAGES EN ABSENCE ::\n";
    * open HVLOG, "<$hlog_file";
    * while(<HVLOG>) {
    * IRC::print $_;
    * }
    * close HVLOG;
    * IRC::print "\002\0034:: FIN DES MESSAGES ::\n\n";
    * if($hautoclear == 1) {
    * clog_handler;
    * }
    * return 1;
    * }
    *
    * sub clog_handler {
    * open HVLOG, ">$hlog_file";
    * close HVLOG;
    * IRC::print "\002\0034:: LES MESSAGES EN ABSENCE ONT ETE SUPPRIMES ::\003\n";
    * return 1;
    * }

