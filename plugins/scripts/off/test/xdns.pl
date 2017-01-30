#####################################################################################
### Name:                                                                    Xdns ###
### Author:                                                                EdgeX- ###
###                                               http://www.bleedingedgetech.net ###
###                                                              irc.malvager.com ###
### Versoin:                                                                  1.0 ###
### Date:                                                                10/03/09 ###
### Requirements:                              Application: Xchat 2 or Higher     ###
### Usage:                                     Load the module in xChat and type  ###
###                                            /xdns_info for usage instructions  ###
###                                            Contact me using the contact tab   ###
###                                            my blog for more help!             ###
#####################################################################################
## Importing Needed Modules ##
use Socket;


## Registering the Plugins and Hooking Commands ##
Xchat::register("Xdns", "1.0", "A simple DNS script for Xchat");
Xchat::hook_command("ip", xdns_resolve_hostname);
Xchat::hook_command("hostname", xdns_resolve_ip);
Xchat::hook_command("ip_pub", xdns_resolve_hostname_pub);
Xchat::hook_command("hostname_pub", xdns_resolve_ip_pub);
Xchat::hook_command("xdns_info", xdns_info);

## Functions ##
sub xdns_load {
	Xchat::command("ECHO ---");
	Xchat::command("ECHO Xdns 1.0 by EdgeX- loaded");
	Xchat::command("ECHO For HELP type /xdns_info");
	Xchat::command("ECHO ---");
	return Xchat::EAT_ALL;
}

sub xdns_resolve_ip {
	$origip = $_[0][1];
    $iaddr = inet_aton($origip);
	if (defined($iaddr)) {
		$name = gethostbyaddr($iaddr, AF_INET);
	}else{
		$name = "An Error has Occured";
	}
	Xchat::command("ECHO 05Xdns [IP -> Hostname]: $origip = $name");
	return Xchat::EAT_ALL;
}

sub xdns_resolve_ip_pub {
	$origip = $_[0][1];
    $iaddr = inet_aton($origip);
	if (defined($iaddr)) {
		$name = gethostbyaddr($iaddr, AF_INET);
	}else{
		$name = "An Error has Occured";
	}
	Xchat::command("SAY 05Xdns [IP -> Hostname]: $origip = $name");
	return Xchat::EAT_ALL;
}

sub xdns_resolve_hostname {
	$orig_hostname = $_[0][1];
	$packed_ip = gethostbyname($orig_hostname);
    if (defined $packed_ip) {
		$ip_address = inet_ntoa($packed_ip);
	}else{
		$ip_address = "An Error has Occured";
		undef($packed_ip);
	}
	Xchat::command("ECHO 05Xdns [Hostname -> IP]: $orig_hostname = $ip_address");
	return Xchat::EAT_ALL;
}

sub xdns_resolve_hostname_pub {
	$orig_hostname = $_[0][1];
	$packed_ip = gethostbyname($orig_hostname);
    if (defined $packed_ip) {
		$ip_address = inet_ntoa($packed_ip);
	}else{
		$ip_address = "An Error has Occured";
		undef($packed_ip);
	}
	Xchat::command("SAY 05Xdns [Hostname -> IP]: $orig_hostname = $ip_address");
	return Xchat::EAT_ALL;
}
sub xdns_info {
	Xchat::command("ECHO Name: Xdns");
	Xchat::command("ECHO Author: EdgeX-");
	Xchat::command("ECHO          http://www.bleedingedgetech.net");
	Xchat::command("ECHO Version: 1.0");
	Xchat::command("ECHO Date: 10/03/09");
	Xchat::command("ECHO Usage: ip <hostname> [Finds out the hostname of the entered IP]");
	Xchat::command("ECHO        ip_pub <hostname> [Finds out the hostname of the entered IP and prints it in the channel]");
	Xchat::command("ECHO        hostname <ip> [Finds out the IP address of the entered hostname]");
	Xchat::command("ECHO        hostname_pub <ip> [Finds out the IP address of the entered hostname and prints it in the channel]");
	Xchat::command("ECHO        xdns_info [Show this Information Message]");
}

## And Finally- Load the Script ##
xdns_load();
