#
# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

import rti.connextdds as dds

from test_utils.fixtures import *
from rti.types.builtin import String, KeyedString


def test_on_invalid_local_identity_status_advance_notice_can_be_set():

	class listener(dds.NoOpDomainParticipantListener):
		def on_invalid_local_identity_status_advance_notice(self, _dp, _s):
			pass

	l1 = listener()
	l2 = listener()
	with dds.DomainParticipant(
		get_test_domain(),
		get_test_participant_qos(),
		l1,
		dds.StatusMask.INVALID_LOCAL_IDENTITY_ADVANCE_NOTICE) as p:
		assert p.listener == l1
		assert p.listener != l2
		p.listener = l2
		assert p.listener != l1
		assert p.listener == l2
