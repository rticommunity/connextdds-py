import rti.connextdds as dds


def test_addition():
    s1 = dds.Int32Seq(range(0, 4))
    s2 = dds.Int32Seq(range(4, 8))
    s3 = s1 + s2
    for i in range(0, 8):
        assert s3[i] == i


def test_addition_assignment():
    s1 = dds.Int32Seq(range(0, 4))
    s2 = dds.Int32Seq(range(4, 8))
    s1 += s2
    for i in range(0, 8):
        assert s1[i] == i


def test_multiplication():
    s1 = dds.Int32Seq(range(0, 4))
    s2 = s1 * 2
    for i in range(0, 4):
        assert s2[i] == i
        assert s2[i + 4] == i


def test_multiplication_assignment():
    s1 = dds.Int32Seq(range(0, 4))
    s1 *= 2
    for i in range(0, 4):
        assert s1[i] == i
        assert s1[i + 4] == i
