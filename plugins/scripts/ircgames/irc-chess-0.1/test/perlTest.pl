#!/usr/bin/perl

#a little program to help me brush up on my perl

$st="the boy went to the shop";

@list = ($st =~ /(the)/g);
$count=@list;
print("number of matches: $count\n");

#to parse messages
#first type of message:
#[username]Message
#
#second type of message:
#[username1]Message1<:=:>[username2]Message2<:=:>CommonMessage


$delimiter="<:=:>";
$_=$ARGV[0];

$numDelims=(@list=/$delimiter/g);

if ($numDelims==0)
{
	#simple message
	($username, $message)=/^\[(.+?)\](.*?)$/;
	print("username: $username and message: $message\n");
}
else
{
	#complex message with delimiters
	($user1, $msg1, $user2, $msg2, $commonMessage)=/^\[(.+?)\](.+?)$delimiter\[(.+?)\](.*?)$delimiter(.*?)$/;

	print("username1: $user1\n");
	print("message1: $msg1\n");
	print("username2: $user2\n");
	print("message2: $msg2\n");
	print("common message: $commonMessage\n");
}
