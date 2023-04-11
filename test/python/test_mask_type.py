 #
 # (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 #
 # RTI grants Licensee a license to use, modify, compile, and create derivative
 # works of the Software solely for use with RTI products.  The Software is
 # provided "as is", with no warranty of any type, including any warranty for
 # fitness for any purpose. RTI is under no obligation to maintain or support
 # the Software.  RTI shall not be liable for any incidental or consequential
 # damages arising out of the use or inability to use the software.
 #

import rti.connextdds as dds


def test_mask_operations():
    mask = (dds.StatusMask.DATA_AVAILABLE | dds.StatusMask.LIVELINESS_CHANGED)
    none = dds.StatusMask.NONE
    all = dds.StatusMask.ALL

    # __bool__ and __int__
    assert mask != 0
    assert mask
    assert all
    assert not none

    # test_xxx
    assert mask.test_any()
    assert none.test_none()
    assert all.test_all()
    assert not mask.test_all()
    assert not mask.test_none()

    # __getitem__
    assert not none[0]
    assert all[0]
    assert not none.test(1)
    assert all.test(1)

    # __contains__
    assert mask in all
    assert not (mask in none)

    # count, size
    assert mask.count == 2
    assert none.count == 0
    assert all.count == 32
    assert mask.size == 32

    # set, reset, flip
    mask2 = dds.StatusMask.NONE
    mask2.set(1, True)
    assert mask2.test(1)
    mask2.set()
    assert mask2 == all
    mask2.reset(1)
    assert not mask2.test(1)
    mask2.reset()
    assert mask2 == none
    mask2.flip(1)
    assert mask2.test(1)

    # __ior__, __iand__, __ixor__, __isub__
    mask2 = dds.StatusMask.OFFERED_DEADLINE_MISSED
    mask2 |= dds.StatusMask.DATA_ON_READERS
    assert mask2 == dds.StatusMask.DATA_ON_READERS | dds.StatusMask.OFFERED_DEADLINE_MISSED
    mask2 &= dds.StatusMask.DATA_ON_READERS
    assert mask2 == dds.StatusMask.DATA_ON_READERS
    mask2 ^= dds.StatusMask.DATA_ON_READERS
    assert mask2 == dds.StatusMask.NONE

    # shifts
    mask2 = dds.StatusMask.NONE
    mask2.flip(1)
    mask2 <<= 1
    assert mask2.test(2)
    assert not mask2.test(1)
    mask2 >>= 1
    assert mask2.test(1)
    assert not mask2.test(2)

    mask2 = dds.StatusMask.NONE
    mask2[1] = 1
    mask3 = mask2 << 1
    assert mask3.test(2)
    assert not mask3.test(1)
    mask3 = mask3 >> 1
    assert mask3.test(1)
    assert not mask3.test(2)

    # repr and str
    mask2 = dds.StatusMask.NONE
    mask2.flip(1)
    assert repr(mask2) == '00000000000000000000000000000010'
    assert str(mask2) == repr(mask2)




