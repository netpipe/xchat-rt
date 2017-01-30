#!/usr/bin/perl -w

# Agrega comandos del BitchX al [x]Chat!!!
#
# Comandos agregados:
# scano  -> Crea una lista de los operadores en el canal
# scanv  -> Crea una lista de los voices en el canal.
# scan   -> Crea una lista de los usuarios en el canal.
# scann  -> Crea una lista de NO-Operadores del canal.
# scani  -> Crea una lista de IRCops en el canal.
# clones -> Busca clones en el canal.
# join   -> Join con y sin #
#
# Nuevos: agregados por |{RaSH (GLUB member http://glub.ehu.es)
# scanh -> Crea una lista de los presentes en el canal.
# scana -> Crea una lista de los ausentes en el canal.
#
# Notas: El script fue escrito muy rapidamente y tiene cosas
# que se podrian optimizar... Espero tener tiempo para hacerlo 
# pronto.

my $contador_who = 0;
my $tipo_who = "";
my $contador_espacio = "";
my $Resultado = "";
my $previous_nick = "";
my $who_canal = "";
my $canal_gral = "";
my $who_servidor = "";

my $clones;
my $lista_clones;
my $clones_cont = 0;

IRC::register("BitchX","0.1","","");

IRC::add_message_handler("352", "raw_352_handler");
IRC::add_message_handler("315", "raw_315_handler");

IRC::add_command_handler("SCANO", "scano_handler");
IRC::add_command_handler("SCANV", "scanv_handler");
IRC::add_command_handler("SCAN", "scan_handler");
IRC::add_command_handler("SCANN", "scann_handler");
IRC::add_command_handler("SCANH", "scanh_handler");
IRC::add_command_handler("SCANA", "scana_handler");
IRC::add_command_handler("SCANI", "scani_handler");
IRC::add_command_handler("CLONES", "clones_handler");
IRC::add_command_handler("JOIN", "my_join");

sub clones_handler {
	my $canal = shift;
	$who_canal = IRC::get_info(2);
        my $who_servidor = IRC::get_info(3);

	if ($canal) {
	        my @char_canal = split(//, $canal);
		if ($char_canal[0] eq "\#") {
	                $tipo_who = "CLONES";
			$canal_gral = $canal;
                	IRC::command("/who $canal");
	        }
	} else {
		my @char_canal = split(//, $who_canal);
		if ($char_canal[0] eq "\#") {
			$tipo_who = "CLONES";
			$canal_gral = $who_canal;
                        IRC::command("/who $who_canal");
		}
	}
        return 1;
}

sub scano_handler {
	my $canal = shift;
	$who_canal = IRC::get_info(2);
	my $who_servidor = IRC::get_info(3);

	if ($canal) {
		my @char_canal = split(//, $canal);
		if ($char_canal[0] eq "\#") {
			$tipo_who = "SCANO";
			$canal_gral = $canal;
			IRC::command("/who $canal");
		}
	} else {
		my @char_canal = split(//, $who_canal);
		if ($char_canal[0] eq "\#") {
			$tipo_who = "SCANO";
			$canal_gral = $who_canal;
                        IRC::command("/who $who_canal");
		}
	}
	return 1;
}

sub scanv_handler {
	my $canal = shift;
        $who_canal = IRC::get_info(2);
        my $who_servidor = IRC::get_info(3);

	if ($canal) {
	        my @char_canal = split(//, $canal);
		if ($char_canal[0] eq "\#") {
        	        $tipo_who = "SCANV";
			$canal_gral = $canal;
        	        IRC::command("/who $canal");
	        }
	} else {
		my @char_canal = split(//, $who_canal);
		if ($char_canal[0] eq "\#") {
			$tipo_who = "SCANV";
			$canal_gral = $who_canal;
                        IRC::command("/who $who_canal");
		}
	}
        return 1;
}

sub scan_handler {
	my $canal = shift;
	$who_canal = IRC::get_info(2);
        my $who_servidor = IRC::get_info(3);

	if ($canal) {
	        my @char_canal = split(//, $canal);
		if ($char_canal[0] eq "\#") {
        	        $tipo_who = "SCAN";
			$canal_gral = $canal;
			IRC::command("/who $canal");
        	}
	} else {
		my @char_canal = split(//, $who_canal);
		if ($char_canal[0] eq "\#") {
			$tipo_who = "SCAN";
			$canal_gral = $who_canal;
                        IRC::command("/who $who_canal");
		}
	}
        return 1;
}

sub scanh_handler {
	my $canal = shift;
	$who_canal = IRC::get_info(2);
        my $who_servidor = IRC::get_info(3);

	if ($canal) {
	        my @char_canal = split(//, $canal);
		if ($char_canal[0] eq "\#") {
        	        $tipo_who = "SCANH";
			$canal_gral = $canal;
	                IRC::command("/who $canal");
        	}
	} else {
		my @char_canal = split(//, $who_canal);
		if ($char_canal[0] eq "\#") {
			$tipo_who = "SCANH";
			$canal_gral = $who_canal;
                        IRC::command("/who $who_canal");
		}
	}
        return 1;
}

sub scana_handler {
	my $canal = shift;
	$who_canal = IRC::get_info(2);
        my $who_servidor = IRC::get_info(3);

	if ($canal) {
	        my @char_canal = split(//, $canal);
		if ($char_canal[0] eq "\#") {
        	        $tipo_who = "SCANA";
			$canal_gral = $canal;
	                IRC::command("/who $canal");
        	}
	} else {
		my @char_canal = split(//, $who_canal);
		if ($char_canal[0] eq "\#") {
			$tipo_who = "SCANA";
			$canal_gral = $who_canal;
                        IRC::command("/who $who_canal");
		}
	}
        return 1;
}

sub scani_handler {
	my $canal = shift;
	$who_canal = IRC::get_info(2);
        my $who_servidor = IRC::get_info(3);

	if ($canal) {
	        my @char_canal = split(//, $canal);
		if ($char_canal[0] eq "\#") {
        	        $tipo_who = "SCANI";
			$canal_gral = $canal;
	                IRC::command("/who $canal");
        	}
	} else {
		my @char_canal = split(//, $who_canal);
		if ($char_canal[0] eq "\#") {
			$tipo_who = "SCANI";
			$canal_gral = $who_canal;
                        IRC::command("/who $who_canal");
		}
	}
        return 1;
}

sub scann_handler {
	my $canal = shift;
	$who_canal = IRC::get_info(2);
        my $who_servidor = IRC::get_info(3);

	if ($canal) {
	        my @char_canal = split(//, $canal);
		if ($char_canal[0] eq "\#") {
        	        $tipo_who = "SCANN";
			$canal_gral = $canal;
	                IRC::command("/who $canal");
        	}
	} else {
		my @char_canal = split(//, $who_canal);
		if ($char_canal[0] eq "\#") {
			$tipo_who = "SCANN";
			$canal_gral = $who_canal;
                        IRC::command("/who $who_canal");
		}
	}
        return 1;
}

sub scan_tab_handler {
        my $line= shift;
	@line = split(/\ /, $line);
	$nick = $line[0];
	$tipo_indice = $line[1];
	
        my $long_nick=length($nick);
        if ($long_nick > 9) {
                my @nick_char = split(//,$nick);
                $nick = "";
                for ($x=0; $x<=8; $x++) {
                        $nick = "$nick$nick_char[$x]";
                }
        }
        if ($long_nick < 9) {
                for ($x=(9-$long_nick); $x>=1; $x--) {
                        $nick = "$nick ";
                }
        }
	if ($tipo_indice eq "V") {
		$Resultado = "$Resultado\00314[\00313v\00304$nick \00314]";
	} else {
		if ($tipo_indice eq "O") {
			$Resultado = "$Resultado\00314[\00308\@\00304$nick \00314]";
		} else {
			if ($tipo_indice eq "I") {
				$Resultado = "$Resultado\00314[\00308\*\00304$nick \00314]";
			} else {
		        	$Resultado = "$Resultado\00314[\00304 $nick \00314]";
			}
		}
	}
}

sub raw_352_handler {
	my $line = shift;
	$line =~ /:(.+) 352 (.+) (.+) (.+) (.+) (.+) (.+) (.+) :(.+) (.*)/;
	my $servidor = $1;
	my $mi_nick = $2;
	my $chan = $3;
	my $username = $4;
	my $addr = $5;
	my $server = $6;
	my $nick = $7;
	my $flags1 = $8;
	my $flags2 = $9;
	my $Real_name = $10;
	my $flag_found = "";

	if ($tipo_who) 
	{
		# Vamos a separar los flags
		if ($tipo_who eq "SCANO") {
			if ($flags1 =~ /\@/) {
				if ($contador_espacio eq "") {
					$contador_espacio = 0;
				} else {
					$contador_espacio++;
				}
				if ($contador_espacio == 5) {
					$Resultado = "$Resultado\n\00312-\003\:\00312-\011\003 ";
					$contador_espacio = 0;
				} else {
					$Resultado = "$Resultado ";
				}
				scan_tab_handler "$nick O" ;
				$contador_who++;
			}
		} 
		if ($tipo_who eq "SCANV") {
       	                if ($flags1 =~ /\+/) {
                       	        if ($contador_espacio eq "") {
	                                $contador_espacio = 0;
                                } else {
                                        $contador_espacio++;
                                }
                                if ($contador_espacio == 5) {
                                        $Resultado = "$Resultado\n\00312-\003\:\00312-\011\003 ";
                                        $contador_espacio = 0;
                                } else {
                                        $Resultado = "$Resultado ";
                                }
                                scan_tab_handler "$nick V";
                                $contador_who++;
                        }
                }
		if ($tipo_who eq "SCAN") {
			if ($contador_espacio eq "") {
                        	$contador_espacio = 0;
                        } else {
                                $contador_espacio++;
                        }
			if ($contador_espacio == 5) {
                                $Resultado = "$Resultado\n\00312-\003\:\00312-\011\003 ";
                                $contador_espacio = 0;
                        } else {
				$Resultado = "$Resultado ";
                        }
			if ($flags1 =~ /\@/) {
				scan_tab_handler "$nick O";
			} else {
				if ($flags1 =~ /\+/) {
					scan_tab_handler "$nick V";
				} else {
					scan_tab_handler "$nick N";
				}
			}
			$contador_who++;
		}
		if ($tipo_who eq "SCANA") {
			if ($flags1 =~ /G/) {
				if ($contador_espacio eq "") {
	                                $contador_espacio = 0;
	                        } else {
	                                $contador_espacio++;
	                        }
	                        if ($contador_espacio == 5) {
	                                $Resultado = "$Resultado\n\00312-\003\:\00312-\011\003 ";
	                                $contador_espacio = 0;
	                        } else {
					$Resultado = "$Resultado ";
	                        }
				if ($flags1 =~ /\@/) {
					scan_tab_handler "$nick O";
				} else {
					if ($flags1 =~ /\+/) {
						scan_tab_handler "$nick V";
					} else {
						scan_tab_handler "$nick N";
					}
				}
				$contador_who++;
			}
		}
		if ($tipo_who eq "SCANH") {
			if ($flags1 =~ /H/) {
				if ($contador_espacio eq "") {
                                        $contador_espacio = 0;
                                } else {
                                        $contador_espacio++;
                                }
                                if ($contador_espacio == 5) {
                                        $Resultado = "$Resultado\n\00312-\003\:\00312-\011\003 ";
                                        $contador_espacio = 0;
                                } else {
					$Resultado = "$Resultado ";
                                }
                                if ($flags1 =~ /\@/) {
                                        scan_tab_handler "$nick O";
                                } else {
					if ($flags1 =~ /\+/) { 
	                                        scan_tab_handler "$nick V";
					} else {
						scan_tab_handler "$nick N";
					}
				}
				$contador_who++;
			}
		}
		if ($tipo_who eq "SCANI") {
			if ($flags1 =~ /\*/) {
				if ($contador_espacio eq "") {
                                        $contador_espacio = 0;
                                } else {
                                        $contador_espacio++;
                                }
                                if ($contador_espacio == 5) {
                                        $Resultado = "$Resultado\n\00312-\003\:\00312-\011\003 ";
                                        $contador_espacio = 0;
                                } else {
                                        $Resultado = "$Resultado ";
                                }
				scan_tab_handler "$nick I";
				$contador_who++;
			}
		}
		if ($tipo_who eq "SCANN") {
			if (!($flags1 =~ /\@/)) {
				if ($contador_espacio eq "") {
	                                $contador_espacio = 0;
	                        } else {
	                                $contador_espacio++;
	                        }
				if ($contador_espacio == 5) {
	                                $Resultado = "$Resultado\n\00312-\003\:\00312-\011\003 ";
	                                $contador_espacio = 0;
	                        } else {
	                                $Resultado = "$Resultado ";
	                        }
				if ($flags1 =~ /\+/) {
                                        scan_tab_handler "$nick V";
                                } else {
                                        scan_tab_handler "$nick N";
                                }
				$contador_who++;
			}
		}
		if ($tipo_who eq "CLONES") {
			my $clones_bool = 0;
			my $clones_count = 0;
			my $check_ip;

			$lista_clones[$clones_cont] = "$nick $username\@$addr";
			$clones_cont++;
			if ($clones[0]) {
				foreach $check_ip (@clones) {
					if ($check_ip =~ /$addr/) {
						$clones[$clones_count] = "$nick $clones[$clones_count]";
						$clones_bool = 1;
					}
					$clones_count++;
				}
				if ($clones_bool == 0) {
					$clones[$clones_count] = "$nick\:$addr";
				}
			} else {
				$clones[0] = "$nick\:$addr";
			}
		}
		return 1;
	} else {
		return 0;
	}
	
}
sub raw_315_handler {
	if ($tipo_who ne "") {
		if ($tipo_who eq "SCANO") {
			IRC::print_with_channel("\00312-\003\:\00312-\011\00314[\0039ChanOps\00314(\0033$canal_gral\00314:\00303$contador_who\00314)]\003", $who_canal, $who_servidor);
		}
		if ($tipo_who eq "SCANV") {
			IRC::print_with_channel("\00312-\003\:\00312-\011\00314[\00313VoiceUsers\00314(\0036$canal_gral\00314:\00306$contador_who\00314)]\003", $who_canal, $who_servidor);
		}
		if ($tipo_who eq "SCAN") {
			IRC::print_with_channel("\00312-\003\:\00312-\011\00314[\0039Users\00314(\0033$canal_gral\00314:\00303$contador_who\00314)]\003", $who_canal, $who_servidor);
		}
		if ($tipo_who eq "SCANN") {
			IRC::print_with_channel("\00312-\003\:\00312-\011\00314[\0039NonChanOps\00314(\0033$canal_gral\00314:\00303$contador_who\00314)]\003", $who_canal, $who_servidor);
		}
		if ($tipo_who eq "SCANA") {
			IRC::print_with_channel("\00312-\003\:\00312-\011\00314[\00313AwayUsers\00314(\0036$canal_gral\00314:\00306$contador_who\00314)]\003", $who_canal, $who_servidor);
		}
		if ($tipo_who eq "SCANH") {
                        IRC::print_with_channel("\00312-\003\:\00312-\011\00314[\0039PresentUsers\00314(\0033$canal_gral\00314:\00303$contador_who\00314)]\003", $who_canal, $who_servidor);
		}
		if ($tipo_who eq "SCANI") {
			IRC::print_with_channel("\00312-\003\:\00312-\011\00314[\0039IrcOps\00314(\0033$canal_gral\00314:\00303$contador_who\00314)]\003", $who_canal, $who_servidor);
		}
		if ($tipo_who eq "CLONES") {
			my $found_clones;
			my $nick_found;
			my $clones_count = 0;
		
			foreach $clones_line (@clones) {
				if ($clones_line =~ / /) {
				# HAY CLONES!
					@clones_line = split(/:/, $clones_line);
					$found_clones[$clones_count] = "$clones_line[0]";
					$clones_count++;
					@clones_line = "";
				}
			}
			$clones_count = 0;
			# OK. Aqui ya tenemos un listado de clones.
			# Vamos a preparar la impresion
			foreach $clones_line (@found_clones) {
				@clones_line = split(/ /, $clones_line);
				foreach $find_clone_nick (@clones_line) {
					# Hasta aqui va bien.	
					foreach $find_clone_list (@lista_clones) {
						@nick_found = split(/ /, $find_clone_list);
						if ($nick_found[0] eq $find_clone_nick) {
							$clones_count++;
							my $leng_de_nick = length($nick_found[0]);
							if ($leng_de_nick < 10) {
								for ($x=10;$x>$leng_de_nick;$x--) {
									$nick_found[0] = " $nick_found[0]";
								}
							}
							if ($Resultado eq "") {
								$Resultado = "\00312-\003\:\00312-\011\003 $nick_found[0]\00311 $nick_found[1]";
							} else {
								$Resultado="$Resultado\n\00312-\003\:\00312-\011\003 $nick_found[0]\00311 $nick_found[1]";
							}
						}
					}
					@nick_found = "";
				}
				$Resultado = "$Resultado\n\00312-\003\:\00312-\011\003";
			}
			IRC::print_with_channel("\00312-\003\:\00312-\011\003Clones detected on $canal_gral\n\00312-\003\:\00312-\011\003\n", $who_canal, $who_servidor);
			IRC::print_with_channel("$Resultado", $who_canal, $who_servidor);
			IRC::print_with_channel("\00312-\003\:\00312-\011\003Found $clones_count clones", $who_canal, $who_servidor);
			@lista_clones = "";
			@clones_line = "";
			@clones = "";
			@found_clones = "";
			@nick_found = "";
			$clones_cont = 0;
		}
		$tipo_who = "";
		if ($contador_who ne 0) {
			IRC::print_with_channel("\00312-\003\:\00312-\011\003$Resultado", $who_canal, $who_servidor);
		}
		$Resultado = "";
		$previous_nick = "";
		$contador_espacio = "";
		$contador_who = 0;
		return 1;
	}
	return 0;
}

sub my_join
{
        @chan = split(/ /,$_[0]);
        $i = 0;
        while ($chan[$i]){
                $chan[$i] =~ s/#//;
                $chan[$i] =  "#" . $chan[$i];
                IRC::send_raw "JOIN $chan[$i]\n";
                $i++;
        }
        return 1;
}
