import rti.connextdds as dds


class CustomFilterType(dds.DynamicData.ContentFilter):
    def __index__(self):
        pass

    def compile(self, expression, parameters, type_code, type_class_name, old_compile_data):
        if not expression.startswith('%0 %1 '):
            raise ValueError('Invalid filter expression')
        if len(expression) < 7:
            raise ValueError('Invalid filter expression length')
        if len(parameters) < 2:
            raise ValueError('Invalid number of filter parameters')

        if parameters[1] == 'greater-than':
            cd = (int(parameters[0]), lambda sample_data, p: p > sample_data)
        elif parameters[1] == 'divides':
            cd = (int(parameters[0]), lambda sample_data, p: sample_data % p == 0)
        else:
            raise ValueError('Invalid filter operation')

        return cd

    def evaluate(self, compile_data, sample, meta_data):
        return compile_data[1](sample['x'], compile_data[0])

    def finalize(self, compile_data):
        pass

