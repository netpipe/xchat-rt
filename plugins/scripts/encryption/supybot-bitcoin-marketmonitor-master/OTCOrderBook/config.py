###
# Copyright (c) 2010, Daniel Folkinshteyn
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
    conf.registerPlugin('OTCOrderBook', True)


OTCOrderBook = conf.registerPlugin('OTCOrderBook')
# This is where your configuration variables (if any) should go.  For example:
# conf.registerGlobalValue(OTCOrderBook, 'someConfigVariableName',
#     registry.Boolean(False, """Help for someConfigVariableName."""))

conf.registerGlobalValue(OTCOrderBook, 'orderExpiry',
    registry.NonNegativeInteger(604800, """Time until order expiry. Unless a user
    calls 'refresh', orders will expire after this many seconds. Set to 0 for no
    expiry. It's a good idea to have this set to avoid seeing your database
    overgrow with old cruft."""))

conf.registerGlobalValue(OTCOrderBook, 'minTrustForLongOrders',
    registry.NonNegativeInteger(15, """Minimum total level 1 and level 2
    trust from nanotube to be able to place long duration orders."""))

conf.registerGlobalValue(OTCOrderBook, 'longOrderDuration',
    registry.NonNegativeInteger(7776000, """Extra time on top of standard
    order expiry, allotted to long-duration orders. Time in seconds."""))

conf.registerGlobalValue(OTCOrderBook, 'maxUserOpenOrders',
    registry.NonNegativeInteger(4, """Only allow this many open orders per user.
    It's a good idea to have this on, to avoid order flooding from a rogue
    user."""))

conf.registerGlobalValue(OTCOrderBook, 'maxOrdersInBookList',
    registry.NonNegativeInteger(4, """Only allow this many orders in a currency
    order book to be spit out to channel. If more than that exist, suggest to
    visit the nice website."""))

# vim:set shiftwidth=4 tabstop=4 expandtab textwidth=79:
