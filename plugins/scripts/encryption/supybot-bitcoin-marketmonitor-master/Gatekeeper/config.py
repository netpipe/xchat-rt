###
# Copyright (c) 2011, Daniel Folkinshteyn
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   * Redistributions of source code must retain the above copyright notice,
#     this list of conditions, and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions, and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#   * Neither the name of the author of this software nor the name of
#     contributors to this software may be used to endorse or promote products
#     derived from this software without specific prior written consent.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

###

import supybot.conf as conf
import supybot.registry as registry

def configure(advanced):
    # This will be called by supybot to configure this module.  advanced is
    # a bool that specifies whether the user identified himself as an advanced
    # user or not.  You should effect your configuration by manipulating the
    # registry as appropriate.
    from supybot.questions import expect, anything, something, yn
    conf.registerPlugin('Gatekeeper', True)


Gatekeeper = conf.registerPlugin('Gatekeeper')
# This is where your configuration variables (if any) should go.  For example:
# conf.registerGlobalValue(Gatekeeper, 'someConfigVariableName',
#     registry.Boolean(False, """Help for someConfigVariableName."""))

conf.registerGlobalValue(Gatekeeper, 'targetChannel',
    registry.String("#bitcoin-otc", """Target channel for gatekeeper
    management"""))

conf.registerGlobalValue(Gatekeeper, 'ratingThreshold',
    registry.NonNegativeInteger(0, """Minimum rating to be allowed in."""))

conf.registerGlobalValue(Gatekeeper, 'accountAgeThreshold',
    registry.PositiveInteger(604800, """Minimum account age, in seconds,
    to be allowed in."""))

conf.registerGlobalValue(Gatekeeper, 'invite',
    registry.Boolean(False, """Should the bot invite the user to channel?"""))

conf.registerGlobalValue(Gatekeeper, 'msgOnJoinVoice',
    registry.String("Join #bitcoin-otc-foyer and see channel topic for instructions on getting voice on #bitcoin-otc.",
    """Message to send to unauthed users with instructions on 
    how to get voice in channel."""))

conf.registerGlobalValue(Gatekeeper, 'msgOnJoinIdent',
    registry.String("#bitcoin-otc: \x02Watch out for fraudsters!\x02 Always check authentication with the \x02ident\x02 command before trading, otherwise you could be dealing with an \x02impostor\x02. If in doubt, ask in channel. More info: http://bit.ly/YCGOI3",
    """Message to send to unauthed users with instructions on 
    checking auth and avoiding fraudsters."""))

conf.registerGlobalValue(Gatekeeper, 'talkInChanOnlyForAuthedUsers',
    registry.Boolean(True, """Should the bot only respond in channel for
    authed users?"""))


# vim:set shiftwidth=4 tabstop=4 expandtab textwidth=79:
