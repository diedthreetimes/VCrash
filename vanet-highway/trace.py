# File representing data contained in a single line of trace information.

from collections import namedtuple

MessageTraceData = namedtuple('TraceData','all_msgs forwarded ignored created')

class Message:
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

    def __lt__(self,other):
        return self.time < other.time

class Vehicle:
    data_delimeter = ','
    
    def __init__(self,line):
        data = line.split(self.data_delimeter)
        self.id = int(data[1])
    
    
