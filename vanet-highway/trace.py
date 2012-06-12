# File representing data contained in a single line of trace information.

class DataTrace:
    field_delimeter = ': '
    label_delimeter = '$'
    data_delimeter = ','

    def __init__(self,line):
        data = line.split(self.label_delimeter)[1]
        data = data.split(self.data_delimeter)
        for d in data:
            if 'T:' in d:
                self.time = long(d.split(self.field_delimeter)[1])
            elif 'ID:' in d:
                self.id = int(d.split(self.field_delimeter)[1])
            elif 'TTL:' in d:
                self.ttl = int(d.split(self.field_delimeter)[1])
            elif 'Xpos:' in d:
                self.x = float(d.split(self.field_delimeter)[1])
            elif 'Ypos:' in d:
                self.y = float(d.split(self.field_delimeter)[1])

class VehicleTrace:
    data_delimeter = ','
    
    def __init__(self,line):
        data = line.split(self.data_delimeter)
        self.id = data[1]
    
    
