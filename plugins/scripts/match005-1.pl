# more information is available after __END__
package Xchat::b0at::UserMatch; # prevent collisions
use strict;
use warnings;
my $NAME    = 'User Match';
my $VERSION = '005-1';
Xchat::register($NAME, $VERSION, "Find users and clones");


my $PREFIX  = "\02$NAME\02\t"; # for identifying output from the script
my $Channel_Header = "\00314---\t\00311\02%s\017"; # network / channel_name
my $Cloned_Address = "@\t\00313\02%s\017"; # address
my $Cloned_Nick    = " % 16s ! %s "; # nick, user
my $Matched_User   = "%s\t \017% 10s @ %s "; # nick, user, address

## Commands
my %CMDS    = (
    'MATCH' =>
        "Usage: MATCH [<scope>] [<negation>] <patterns> [[<negation>] <patterns> [...]]\n".
        "       scope  -server, -all, or -chan:#list,#of,#channels (no spaces in list)\n".
        "              the current channel is default\n".
        "    negation  -n negates all following patterns (find users that do not match)\n".
        "    patterns  matched against nick!ident\@host, may contain wildcards (*?)\n".
        "              if a pattern starts with a hypen (-), preface it with -p:, as in -p:-foo*\n".
        "    examples  /MATCH -s -n *.com, match all hosts without .com in the current server\n".
        "              /MATCH -p:-all*, matches nicks that start with '-all' in the current channel",
    'CLONES' =>
        "Usage: CLONES [<scope>] [<negation>] [<method>]\n".
        "       scope  -server, -all, or -chan:#list,#of,#channels (no spaces in list)\n".
        "              the current channel is default\n".
        "    negation  -n (find unique addresses instead of clones)\n".
        "      method  -x to find clones between channels\n".
        "              default behavior finds clones within each channel\n",
);
Xchat::hook_command($_, \&{lc $_}, {help_text=>$CMDS{$_}}) foreach (keys %CMDS);
Xchat::print("\02$NAME $VERSION\02 by b0at (use /".join(', /',keys %CMDS).')');

## Subroutines
sub match
{ # /match
    my @args = @{ $_[0] };
    shift @args; # MATCH
    
    my $scope    = ''; # ultimate scope
    my $negate   = 0;  # negation status while parsing args
    my @patterns = ( [], [], ); # 0-> not negated, 1-> negated
    foreach (@args)
    {
        if( /^-n(?:e(?:g(?:a(?:t(?:e)?)?)?)?)?$/i )
        {
            $negate = !$negate; # $negate= ($negate+1)%2;
        }
        elsif( /^[^-]/ or s/^-p://i )
        {
            push @{ $patterns[$negate] }, $_;
        }
        elsif( /^-(
                    a(?:l(?:l)?)?                        |
                    s(?:(?:e(?:r(?:v(?:e(?:r)?)?)?)?)?)? |
                    (?:c(?:(?:h(?:a(?:n)?)?)?)?:.+)
                )$/xi )
        {
            $scope = lc $1;
        }
        else
        {
            Xchat::print("$PREFIX Unknown option '$_' encountered and ignored.");
        }
    }
    
    if( scalar @{$patterns[0]} < 1 and scalar @{$patterns[1]} < 1 )
    {
        Xchat::print( $CMDS{ uc $_[0][0] } );
        return Xchat::EAT_ALL;
    }
    
    my %users = get_users( $scope );
    
    for $negate (0..1)
    {
        for my $ptn (wildcard(@{ $patterns[$negate] }))
        {
            foreach (keys %users)
            {
                my $matches = filter($ptn, $negate, $users{$_});
                if( $#$matches > -1 )
                    { $users{$_} = $matches; }  # save
                else{ delete $users{$_}; }      # trim
                
            }
        }
    }
    
    display( 
        "Matching".
            (scalar @{$patterns[0]} ? " '" .join("', '" , @{$patterns[0]})."'" :'').
            (scalar @{$patterns[1]} ? " -'".join("-', '", @{$patterns[1]})."'" :''),
        'matches', 
        %users 
    );
    
    return Xchat::EAT_ALL;
}

sub clones
{ # /clones
    my @args = @{ $_[0] };
    shift @args; # CLONES
    
    my $scope    = '';
    my $negate   = 0;
    my $method   = '';
    foreach (@args)
    {
        if( /^-n(?:e(?:g(?:a(?:t(?:e)?)?)?)?)?$/i )
        {
            $negate = 1;
        }
        elsif( /^-x$/i )
        {
            $method = 'x';
        }
        elsif( /^-(
                    a(?:l(?:l)?)?                        |
                    s(?:(?:e(?:r(?:v(?:e(?:r)?)?)?)?)?)? |
                    (?:
                        c(?:(?:h(?:a(?:n)?)?)?)?
                        :.+
                    )
                )$/xi )
        {
            $scope = lc $1;
        }
        else
        {
            Xchat::print("$PREFIX Unknown option '$_'.");
        }
    }
    
    my $display = ($negate?'Unique Addresses':'Clones');
    
    my %users = get_users( $scope, $method );
    
    my $cloneresults;
    if( not $negate )
    {
        foreach (keys %users)
        {
            $cloneresults = desinglefy($users{$_});
            if( $#$cloneresults > -1 )
                { $users{$_} = $cloneresults; }
            else{ delete $users{$_}; }
        }
    }
    else
    {
        foreach (keys %users)
        {
            $cloneresults = declone($users{$_});
            if( $#$cloneresults > -1 )
                { $users{$_} = $cloneresults; }
            else{ delete $users{$_}; }
        }
    }
    
    display( $display, "\L$display", %users );
    
    return Xchat::EAT_ALL;
}

sub get_users
{ # return lists of users hashed by context or scope
    my ($scope, $method) = @_;
    my @users = ();
    my %users = ();
    
    if( not $scope )
    { # current channel
        my $name = Xchat::get_info('server').' / '.Xchat::get_info('channel');
        @users = map
            {
                my ($idnt, $addr) = split '@', $_->{host};
                {
                    host=>$_->{host},
                    nick=>$_->{nick},
                    idnt=>$idnt,
                    addr=>$addr,
                };
            } 
            #Xchat::get_list('users');
            get_user_list();
        %users = ( $name => \@users);
        return %users;
    }
    
    my @channels = ();
    
    if( $scope =~ /^a/ )
    { # add all channels
        $_->{type}!=2 
        || push @channels,$_ foreach (Xchat::get_list('channels'));
    }
    
    elsif( $scope =~ /^s/ )
    { # add all channels on the current server
        my $server = Xchat::get_info('server');
        ($_->{server} ne $server || $_->{type}!=2)
        || push @channels,$_ foreach (Xchat::get_list('channels'));
    }
    
    elsif( $scope =~ /c(?:(?:h(?:a(?:n))))?\:(.+)/ )
    { # add listed channels if they exist
        my %chanscope = map { $_ => 1,} split ',', $1;
        (not exists $chanscope{lc $_->{channel}})
        || push @channels,$_ foreach (Xchat::get_list('channels'));
    }
    
    else
    {
        Xchat::print("$PREFIX Unrecognized scope '$scope'.");
        return;
    }
        
    my $saved_context = Xchat::get_context(); # maintain this for the user
       
    if( defined $method and $method eq 'x' )
    { # cross-channel, stick all users in the same list
        my @title = ();
        my @users = ();
        
        for my $chan (@channels)
        {
            Xchat::set_context( $chan->{context} ) or next; # try to get to it
            my $loc = $chan->{server}.'/'.$chan->{channel};
            push @title, $loc;
            # add loc field for better cross-chan output
            push @users, map { $_->{loc} = $loc; $_; } get_user_list(); #Xchat::get_list('users');
            #foreach ( Xchat::get_list('users') )
            #{
            #    $_->{loc} = $loc;
            #    push @users, $_;
            #}
        }
        $users{ join(', ', @title) } = \@users;
    }
    else
    { # keep channel lists separate
        for my $chan (@channels)
        {
            Xchat::set_context( $chan->{context} ) or next;
            $users{ "$chan->{server} / $chan->{channel}" } =
                #[ Xchat::get_list('users') ];
                 [ get_user_list()              ];
        }
    }
    Xchat::set_context($saved_context); # put it back where it was
    
    return %users;
}

sub wildcard 
{ # convert wildcard pattern to regex for matching
    my @patterns = @_;
        
    for (@patterns) 
    {   
        s/^\s+//; s/\s+$//; # trim spaces
        
        # special characters allowed in nicks
        s/([\$\%\@\\\][(){}|^"`'.])/\\$1/g;
        
        s/\?/./g; # one character
        
        # should the '*' wildcard be 
        #s/\*/.*/g; # greedy?
        s/\*/.*?/g; # or non-greedy?
    }
    
    return( wantarray? @patterns : $patterns[0] );
}

sub filter
{ # filter users by the pattern
    my ($pattern, $positive, $users) = @_;
        
    my @match = 
        map
        {
            my ($idnt, $addr) = split '@', $_->{host};
            {
                nick=>$_->{nick}, 
                host=>$_->{host},
                idnt=>$idnt, 
                addr=>$addr,
            };
        } 
        grep { "$_->{nick}!$_->{host}" =~ /^$pattern$/i ? not $positive : $positive } #xor
        @$users;
    
    return \@match;
}

sub desinglefy
{ # return users with non-unique addresses (clones)
    my ($users) = @_;
    my %clones = by_count( @$users );
    $clones{$_}->{num} > 1 || delete $clones{$_} foreach (keys %clones);
    return [map { $clones{$_}->{info} } (keys %clones)];
}

sub declone
{ # return users with unique addresses
    my ($users) = @_;
    my %unique = by_count( @$users );    
    $unique{$_}->{num} < 2 || delete $unique{$_} foreach (keys %unique);
    return [map { $unique{$_}->{info} } (keys %unique)];
}

sub by_count
{ # return users hashed by address and with a frequency count
    my @users = @_;
    my %saw   = ();
    map 
    {
        my ($idnt, $addr) = split '@', $_->{host}; ##
        $saw{$addr}->{num}++;
        
        my %usr = %$_;
        $usr{idnt} = $idnt;
        $usr{addr} = $addr;
        
        push @{ $saw{$addr}->{info} }, \%usr;
    } @users;
    return %saw;
}

sub display
{ # print out results
    my ($head, $type, %results) = @_;
    
    #use Data::Dump qw(dump);
    #Xchat::print(dump(\@_));
    
    
    if( $type eq 'matches' )
    {
        Xchat::print("$PREFIX$head...");
        for my $chan (keys %results)
        {
            my $matches = $results{$chan}; # list within $chan
            
            Xchat::print(sprintf($Channel_Header, $chan));
            Xchat::print(
                sprintf($Matched_User ,$_->{nick}, $_->{idnt}, $_->{addr})
            ) foreach (@$matches);
        }
        Xchat::print($PREFIX."End $type.");
    }
    elsif( $type eq 'clones' )
    {
        Xchat::print("$PREFIX$head...");
        for my $chan (keys %results)
        {
            my $groups = $results{$chan}; # list of lists
            
            Xchat::print(sprintf($Channel_Header, $chan));
            for my $clones (@$groups)
            {
                Xchat::print(sprintf($Cloned_Address, $clones->[0]->{addr}));
                Xchat::print(
                    sprintf($Cloned_Nick, $_->{nick}, $_->{idnt})
                ) foreach (@$clones);
            }
        }
        Xchat::print($PREFIX."End $type.");
    }
}

sub strip_codes
{
    my $s = shift;
    $s =~ 
      s!\cB|    # Bold
        \cC\d{0,2}(?:,\d{0,2})?| # Color
        \cG|    # Beep
        \cO|    # Reset
        \cV|    # Reverse
        \c_     # Underline
       !!gx;
    return $s;
}

sub get_user_list
{
    return map { $_->{nick} = strip_codes($_->{nick}); $_; } 
        Xchat::get_list('users');
}

1;
__END__

 author: b0at
license: public domain

Changes from 004.5:
	- fixed case in channel scope checking (LifeIsPain)

changes from 004:
    - strips color codes from nicks
