import rti.connextdds as dds





def get_seq_type(size):
    return dds.SequenceType(dds.Int32Type(), size)

def test_addition():
    s1 = dds.DynamicData(get_seq_type(4))
    s2 = dds.DynamicData(get_seq_type(4))
    s1 = list(range(0,4))
    s2 = list(range(4,8))
    s3 = s1 + s2
    for i in range(0, 8):
        assert s3[i] == i

def test_addition_assignment():
    s1 = dds.DynamicData(get_seq_type(4))
    s2 = dds.DynamicData(get_seq_type(4))
    s1 = list(range(0,4))
    s2 = list(range(4,8))
    s1 += s2 
    for i in range(0, 8):
        s1[i] == i

def test_multiplication():
    pass

def test_multiplication_assignment():
    pass
