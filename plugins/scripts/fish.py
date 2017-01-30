###
#
# FiSH/Mircryption clone for X-Chat in 100% Python
#
# Requirements: PyCrypto, and Python 2.5+
#
# Copyright 2011 Nam T. Nguyen
# Released under the BSD license
#
# irccrypt module is copyright 2009 Bjorn Edstrom
# with modification from Nam T. Nguyen
#
# Changelog:
#
#   * 2.0:
#      + Suport network mask in /key command
#      + Alias key_exchange to keyx
#      + Support plaintext marker '+p '
#      + Support encrypted key store
#
#   * 1.0:
#      + Initial release
#
###
from __future__ import with_statement

__module_name__ = 'fish'
__module_version__ = '2.0'
__module_description__ = 'fish encryption in pure python'


import xchat
import os
xchatdir = xchat.get_info("xchatdir")
#inifile = os.path.join(xchatdir + "/home/weby/.xchat2/plugins/encryption/fishlim/python")
sys.path.append(os.path.join(xchatdir, "plugins/encryption/fishlim/python"))
#open(inifile)
#sys.path.append(xchatdir + "plugins/encryption/fishlim/python"))



import pickle

import threading
import irccrypt



PLAINTEXT_MARKER = '+p '

class KeyMap(dict):
	def __get_real_key(self, key):
		nick, server = (key[0], key[1].lower())
		# get all the keys for nick
		same_nick_keys = [k[1] for k in self.iterkeys() if k[0] == nick]
		# sort by network mask's length
		same_nick_keys.sort(key=lambda k: len(k), reverse=True)
		for k in same_nick_keys:
			if server.rfind(k) >= 0:
				return (nick, k)

	def __getitem__(self, key):
		return dict.__getitem__(self, self.__get_real_key(key))
	
	#def __setitem__(self, key, value):
	#	return dict.__setitem__(self, self.__get_real_key(key), value)

	def __contains__(self, key):
		return dict.__contains__(self, self.__get_real_key(key))

KEY_MAP = KeyMap()
LOCK_MAP = {}

class SecretKey(object):
	def __init__(self, dh, key=None):
		self.dh = dh
		self.key = key
		self.cbc_mode = False

def set_processing():
	id = xchat.get_info('server')
	LOCK_MAP[id] = True

def unset_processing():
	id = xchat.get_info('server')
	LOCK_MAP[id] = False
	
def is_processing():
	id = xchat.get_info('server')
	return LOCK_MAP.get(id, False)

def get_id(ctx):
	return (ctx.get_info('channel'), ctx.get_info('server'))

def get_nick(full):
	if full[0] == ':':
		full = full[1 : ]
	return full[ : full.index('!')]

def get_id_for(ctx, speaker):
	return (get_nick(speaker), ctx.get_info('server'))

def unload(userdata):
	tmp_map = KeyMap()
	encrypted_file = os.path.join(xchat.get_info('xchatdir'),
		'fish_secure.pickle')
	if os.path.exists(encrypted_file):
		return
	for id, key in KEY_MAP.iteritems():
		if key.key:
			tmp_map[id] = key
			key.dh = None
	with open(os.path.join(xchat.get_info('xchatdir'),
		'fish.pickle'), 'wb') as f:
		pickle.dump(tmp_map, f)
	print 'fish unloaded'

def decrypt(key, inp):
	decrypt_clz = irccrypt.Blowfish
	decrypt_func = irccrypt.blowcrypt_unpack
	if 3 <= inp.find(' *') <= 4:
		decrypt_clz = irccrypt.BlowfishCBC
		decrypt_func = irccrypt.mircryption_cbc_unpack
	b = decrypt_clz(key.key)
	return decrypt_func(inp, b)

def encrypt(key, inp):
	encrypt_clz = irccrypt.Blowfish
	encrypt_func = irccrypt.blowcrypt_pack
	if key.cbc_mode:
		encrypt_clz = irccrypt.BlowfishCBC
		encrypt_func = irccrypt.mircryption_cbc_pack
	b = encrypt_clz(key.key)
	return encrypt_func(inp, b)

def decrypt_print(word, word_eol, userdata):
	if is_processing():
		return xchat.EAT_NONE
	ctx = xchat.get_context()
	id = get_id(ctx)
	if id not in KEY_MAP:
		return xchat.EAT_NONE
	speaker, message = word[0], word_eol[1]
	# if there is mode char, remove it from the message
	if len(word_eol) >= 3:
		message = message[ : -(len(word_eol[2]) + 1)]
	if message.startswith('+OK ') or message.startswith('mcps '):
		message = decrypt(KEY_MAP[id], message)
		set_processing()
		ctx.emit_print(userdata, speaker, message)
		unset_processing()
		return xchat.EAT_XCHAT
	else:
		return xchat.EAT_NONE

def encrypt_privmsg(word, word_eol, userdata):
	message = word_eol[0]
	ctx = xchat.get_context()
	id = get_id(ctx)
	if id not in KEY_MAP:
		return xchat.EAT_NONE
	key = KEY_MAP[id]
	if not key.key or message.startswith(PLAINTEXT_MARKER):
		return xchat.EAT_NONE
	cipher = encrypt(key, message)
	xchat.command('PRIVMSG %s :%s' % (id[0], cipher))
	xchat.emit_print('Your Message', xchat.get_info('nick'), message)
	return xchat.EAT_ALL

def key(word, word_eol, userdata):
	ctx = xchat.get_context()
	target = ctx.get_info('channel')
	if len(word) >= 2:
		target = word[1]
	server = ctx.get_info('server')
	if len(word) >= 4:
		if word[2] == '--network':
			server = word[3]
	id = (target, server)
	try:
		key = KEY_MAP[id]
	except KeyError:
		key = SecretKey(None)
	if len(word) >= 3 and word[2] != '--network':
		key.key = word_eol[2]
		KEY_MAP[id] = key
	elif len(word) >= 5 and word[2] == '--network':
		key.key = word_eol[4]
		KEY_MAP[id] = key
	print 'Key for', id, 'set to', key.key
	return xchat.EAT_ALL

def key_exchange(word, word_eol, userdata):
	ctx = xchat.get_context()
	target = ctx.get_info('channel')
	if len(word) >= 2:
		target = word[1]
	id = (target, ctx.get_info('server'))
	dh = irccrypt.DH1080Ctx()
	KEY_MAP[id] = SecretKey(dh)
	ctx.command('NOTICE %s %s' % (target, irccrypt.dh1080_pack(dh)))
	return xchat.EAT_ALL

def dh1080_finish(word, word_eol, userdata):
	ctx = xchat.get_context()
	speaker, command, target, message = word[0], word[1], word[2], word_eol[3]
	id = get_id_for(ctx, speaker)
	print 'dh1080_finish', id
	if id not in KEY_MAP:
		return xchat.EAT_NONE
	key = KEY_MAP[id]
	irccrypt.dh1080_unpack(message[1 : ], key.dh)
	key.key = irccrypt.dh1080_secret(key.dh)
	print 'Key for', id[0], 'set to', key.key
	return xchat.EAT_ALL

def dh1080_init(word, word_eol, userdata):
	ctx = xchat.get_context()
	speaker, command, target, message = word[0], word[1], word[2], word_eol[3]
	id = get_id_for(ctx, speaker)
	key = SecretKey(None)
	dh = irccrypt.DH1080Ctx()
	irccrypt.dh1080_unpack(message[1 : ], dh)
	key.key = irccrypt.dh1080_secret(dh)
	xchat.command('NOTICE %s %s' % (id[0], irccrypt.dh1080_pack(dh)))
	KEY_MAP[id] = key
	print 'Key for', id[0], 'set to', key.key
	return xchat.EAT_ALL

def dh1080(word, word_eol, userdata):
	if word_eol[3].startswith(':DH1080_FINISH'):
		return dh1080_finish(word, word_eol, userdata)
	elif word_eol[3].startswith(':DH1080_INIT'):
		return dh1080_init(word, word_eol, userdata)
	return xchat.EAT_NONE

def load():
	global KEY_MAP
	try:
		with open(os.path.join(xchat.get_info('xchatdir'),
			'fish.pickle'), 'rb') as f:
			KEY_MAP = pickle.load(f)
	except IOError:
		pass
	print 'fish loaded'


def fish_unload_secure(word, word_eol, userdata):
	global KEY_MAP
	decrypted = pickle.dumps(KEY_MAP)
	algo = irccrypt.BlowfishCBC(word_eol[1])
	encrypted = irccrypt.mircryption_cbc_pack(decrypted, algo)
	try:
		with open(os.path.join(xchat.get_info('xchatdir'),
			'fish_secure.pickle'), 'wb') as f:
			f.write(encrypted)
	except IOError:
		pass
	print len(KEY_MAP), 'secure key(s) dumped'
	return xchat.EAT_ALL

def fish_load_secure(word, word_eol, userdata):
	global KEY_MAP
	try:
		with open(os.path.join(xchat.get_info('xchatdir'),
			'fish_secure.pickle'), 'rb') as f:
			encrypted = f.read()
	except IOError:
		pass
	algo = irccrypt.BlowfishCBC(word_eol[1])
	try:
		decrypted = irccrypt.mircryption_cbc_unpack(encrypted, algo)
		tmp_map = pickle.loads(decrypted)
	except:
		tmp_map = {}
	KEY_MAP.update(tmp_map)
	print len(tmp_map), 'secure key(s) loaded'
	return xchat.EAT_ALL

def key_list(word, word_eol, userdata):
	print 'Found', len(KEY_MAP), 'key(s)'
	for id, key in KEY_MAP.iteritems():
		print id, key.key, bool(key.cbc_mode)
	return xchat.EAT_ALL

def key_remove(word, word_eol, userdata):
	id = (word[1], xchat.get_info('server'))
	if id not in KEY_MAP and len(word) > 2:
		id = (word[1], word[2])
	try:
		del KEY_MAP[id]
	except KeyError:
		print 'Key not found'
	else:
		print 'Key removed'
	return xchat.EAT_ALL

def key_cbc(word, word_eol, userdata):
	id = (word[1], xchat.get_info('server'))
	try:
		KEY_MAP[id].cbc_mode = int(word[2])
		print 'CBC mode', bool(KEY_MAP[id].cbc_mode)
	except KeyError:
		print 'Key not found'
	return xchat.EAT_ALL

# handle topic line
def server_332(word, word_eol, userdata):
	if is_processing():
		return xchat.EAT_NONE
	id = get_id(xchat.get_context())
	if id not in KEY_MAP:
		return xchat.EAT_NONE
	key = KEY_MAP[id]
	server, cmd, nick, channel, topic = word[0], word[1], word[2], word[3], word_eol[4]
	if topic[0] == ':':
		topic = topic[1 : ]
	if not (topic.startswith('+OK ') or topic.startswith('mcps ')):
		return xchat.EAT_NONE
	topic = decrypt(key, topic)
	set_processing()
	xchat.command('RECV %s %s %s %s :%s' % (server, cmd, nick, channel, topic))
	unset_processing()
	return xchat.EAT_ALL

def change_nick(word, word_eol, userdata):
	old, new = word[0], word[1]
	ctx = xchat.get_context()
	old_id = (old, xchat.get_info('server'))
	new_id = (new, xchat.get_info('server'))
	try:
		KEY_MAP[new_id] = KEY_MAP[old_id]
		del KEY_MAP[old_id]
	except KeyError:
		pass
	return xchat.EAT_NONE

import xchat
xchat.hook_command('key', key, help='show information or set key, /key <nick> [<--network> <network>] [new_key]')
xchat.hook_command('key_exchange', key_exchange, help='exchange a new key, /key_exchange <nick>')
xchat.hook_command('keyx', key_exchange, help='exchange a new key, /keyx <nick>')
xchat.hook_command('key_list', key_list, help='list keys, /key_list')
xchat.hook_command('key_remove', key_remove, help='remove key, /key_remove <nick>')
xchat.hook_command('key_cbc', key_cbc, help='set cbc mode, /key_cbc <nick> <0|1>')
xchat.hook_command('fish_load_secure', fish_load_secure, help='load fish_secure.pickle, /fish_load_secure <passphrase>')
xchat.hook_command('fish_unload_secure', fish_unload_secure, help='dump fish_secure.pickle, /fish_unload_secure <passphrase>')
xchat.hook_server('notice', dh1080)
xchat.hook_print('Channel Message', decrypt_print, 'Channel Message')
xchat.hook_print('Change Nick', change_nick)
xchat.hook_print('Private Message to Dialog', decrypt_print, 'Private Message to Dialog')
xchat.hook_server('332', server_332)
xchat.hook_command('', encrypt_privmsg)
xchat.hook_unload(unload)
load()

