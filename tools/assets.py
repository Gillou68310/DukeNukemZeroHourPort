# This unused import makes the yaml library faster. don't remove
import pylibyaml  # pyright: ignore
import yaml
import struct

VERSION = 'us'

def write_bin(path, f):
    with open('DukeNukemZeroHour/assets/' + VERSION + '/' +path, 'rb') as a:
        data = a.read()
    f.write(struct.pack('<i', len(data)))
    f.write(data)

with open('DukeNukemZeroHour/versions/' + VERSION + '/dukenukemzerohour.yaml', 'r') as f:
    config = yaml.load(f.read(), Loader=yaml.SafeLoader)

assets = open('assets.' + VERSION + '.bin', 'wb')

for segment in config['segments']:
    if isinstance(segment, dict):
        name = segment.get('name')
        t = segment.get('type')
        d = segment.get('dir')
        subsegments = segment.get('subsegments')

        if t == 'group':
            assert(subsegments != None)
            for subsegment in subsegments:
                assert(subsegment[1] == 'bin')
                if d == None:
                    write_bin(subsegment[2]+'.'+subsegment[1], assets)
                else:
                    write_bin(d+'/'+subsegment[2]+'.'+subsegment[1], assets)

    elif isinstance(segment, list) and len(segment) > 1 and segment[1] == 'bin':
        write_bin(segment[2]+'.'+segment[1], assets)

assets.close()