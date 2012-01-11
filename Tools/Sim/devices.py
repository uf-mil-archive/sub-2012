from __future__ import division

import itertools
import random
import struct

from twisted.internet import protocol, task
from twisted.python import log

import datachunker

def crc16(data, prev=0, mask=0x1021):
    crc = prev
    for char in data:
        c = ord(char)
        c = c << 8
        
        for j in xrange(8):
            if (crc ^ c) & 0x8000:
                crc = ((crc << 1) ^ mask) & 0xffff
            else:
                crc = (crc << 1) & 0xffff
            c = c << 1
    return crc

class EmbeddedProtocol(protocol.Protocol):
    def connectionMade(self):
        self.dataReceived = datachunker.DataChunker(self.dataReceiver())
        self.packet_count = 0
    
    def dataReceiver(self):
        while True:
            # wait for 7E
            garbage = []
            while True:
                b = yield 1
                if b == '\x7e':
                    break
                garbage.append(b)
            if garbage:
                print self, 'embedded garbage', ''.join(garbage).encode('hex')
            
            data = []
            while True:
                b = yield 1
                if b == '\x7e' and not data: # ignore leading 7E - zero length message will never happen, so we're out of sync
                    pass
                elif b == '\x7e' and data:
                    break
                elif b == '\x7d':
                    data.append(chr(ord((yield 1)) ^ 0x20))
                else:
                    data.append(b)
            data2 = ''.join(data)
            
            #print data2.encode('hex')
            
            if len(data2) < 2 or data2[-2:] != struct.pack('<H', crc16(data2[:-2])):
                print 'embedded invalid', data.encode('hex')
                continue
            data3 = data2[:-2]
            
            try:
                (dest_address, src_address, packetcount), data4 = struct.unpack('BBH', data3[:4]), data3[4:]
                
                if dest_address != self.local_address or src_address != self.remote_address:
                    print 'embedded address mismatch', (dest_address, src_address), (self.local_address, self.remote_address)
                    continue
                
                if data4:
                    (typecode,), data5 = struct.unpack('B', data4[:1]), data4[1:]
                else:
                    typecode, data5 = None, ''
                
                self.packetReceived(typecode, data5)
            except:
                print
                print self, "Error handling packet:", data3.encode('hex')
                log.err()
                print
    
    def sendPacket(self, typecode, contents):
        data_pre = struct.pack('BBHB', self.remote_address, self.local_address, self.packet_count % 2**16, typecode) + contents
        data = data_pre + struct.pack('H', crc16(data_pre))
        
        res = []
        res.append('\x7e')
        for char in data:
            if char == '\x7d' or char == '\x7e':
                res.append('\x7d')
                res.append(chr(ord(char) ^ 0x20))
            else:
                res.append(char)
        res.append('\x7e')
        self.transport.write(''.join(res))
        
        self.packet_count += 1

class ThrusterProtocol(EmbeddedProtocol):
    local_address = 21
    # remote_address set in __init__
    
    def __init__(self, thruster_id, thrusters):
        self.thruster_id = thruster_id
        self.thrusters = thrusters
        
        self.remote_address = 30 + thruster_id
        
        self.update_task = task.LoopingCall(self.sendUpdate)
    
    def connectionMade(self):
        EmbeddedProtocol.connectionMade(self)
        print 'thruster', self.thruster_id, 'connection made'
        self.update_task.start(.1)
    
    def sendUpdate(self):
        tickcount = 0
        flags = 1 # valid motor
        refinput = self.thrusters[self.thruster_id] # ???
        presentoutput = 0
        railvoltage = 0
        current = 0
        data = struct.pack('HHHHHH',
            tickcount,
            flags,
            (lambda x: 0x8000|-x if not 0 <= x < 0x8000 else x)(int(refinput * 2**8 + 0.5)),
            int(presentoutput * 2**10 + 0.5),
            int(railvoltage * 2**10 + 0.5),
            int(current * 2**12 + 0.5),
        )
        assert len(data) == 12
        self.sendPacket(0, data)
    
    def packetReceived(self, typecode, data):
        if typecode == 0 and data[0] == '\x03':
            x, = struct.unpack('<H', data[1:])
            if x & 0x8000:
                x = x ^ 0x8000
            else:
                x = -x
            x = x/100/2**8
            #print self.thruster_id, data.encode('hex')
            self.thrusters[self.thruster_id] = x
        else:
            print 'thruster', self.thruster_id, typecode, data.encode('hex')
    
    def connectionLost(self, reason):
        self.update_task.stop()
        print 'thruster', self.thruster_id, 'connection lost', reason

class IMUProtocol(protocol.Protocol):
    def __init__(self, listener_set):
        self.listener_set = listener_set
    
    def connectionMade(self):
        print 'imu connection made'
        self.listener_set.add(self)
    
    def dataReceived(self, data):
        print 'imu', data.encode('hex')
    
    def sendUpdate(self, flags, supply_voltage, ang_rate, acceleration, mag_field, temperature, timestamp):
        #print "M", mag_field
        data = ''.join([
            struct.pack('<HH', flags, supply_voltage/0.00242+.5),
            struct.pack('<3h', *(x/0.000872664626+.5 for x in ang_rate)),
            struct.pack('<3h', *(x/9.80665/0.0033+.5 for x in acceleration)),
            struct.pack('<3h', *(x/0.0005+.5 for x in mag_field)),
            struct.pack('<hq', (temperature - 25)/0.14+.5, timestamp*1e9+.5),
        ])
        assert len(data) == 32
        self.transport.write(data)
    
    def connectionLost(self, reason):
        self.listener_set.remove(self)
        print 'imu connection lost'

class DepthProtocol(EmbeddedProtocol):
    local_address = 21
    remote_address = 40
    
    def __init__(self, listener_set):
        self.listener_set = listener_set
    
    def connectionMade(self):
        EmbeddedProtocol.connectionMade(self)
        print 'depth connection made'
        self.listener_set.add(self)
    
    def packetReceived(self, typecode, data):
        if typecode == 100:
            assert not data, data.encode('hex')
        else:
            print 'depth', typecode, data.encode('hex')
    
    def sendUpdate(self, tickcount, flags, depth, thermister_temp, humidity, humidity_sensor_temp):
        data = struct.pack('<HBHHHH',
            tickcount, flags, depth*2**10+.5, thermister_temp*2**8+.5,
            humidity+.5, humidity_sensor_temp+.5)
        assert len(data) == 11
        self.sendPacket(4, data)
        #print repr(data)
    
    def connectionLost(self, reason):
        self.listener_set.remove(self)

class ActuatorProtocol(protocol.Protocol):
    def __init__(self):
        self.update_task = task.LoopingCall(self.sendUpdate)
    
    def connectionMade(self):
        print 'actuator connection made'
        self.update_task.start(.1)
    
    def dataReceived(self, data):
        for byte in data:
            self.packetReceived(ord(byte))
    
    def packetReceived(self, data):
        print 'actuator', data
    
    def sendUpdate(self):
        self.transport.write('\x00')
    
    def connectionLost(self, reason):
        self.update_task.stop()
        print 'actuator connection lost'

class DVLProtocol(protocol.Protocol):
    def __init__(self, listener_set):
        self.listener_set = listener_set
    
    def connectionMade(self):
        print 'dvl connection made'
        self.listener_set.add(self)
    
    def dataReceived(self, data):
        print 'dvl', repr(data)
    
    def sendHighResVel(self, bottom_vel, bottom_dist, water_vel, water_dist, speed_of_sound):
        if not all(len(x) == 4 for x in [bottom_vel, bottom_dist, water_vel, water_dist]):
            raise TypeError('first four arguments must each have a length of four')
        self.sendUpdate([''.join([
            '\x03\x58',
            struct.pack('16i', *(int(x*1e5+.5) for x in itertools.chain(bottom_vel, bottom_dist, water_vel, water_dist))),
            struct.pack('i', int(speed_of_sound*1e6+.5)),
        ])])
    
    def sendUpdate(self, msgs):
        header_length = 1 + 1 + 2 + 1 + 1 + 2 * len(msgs)
        data = ''.join([
            '\x7f\x7f',
            struct.pack('<HBB', header_length + sum(map(len, msgs)) + 2, random.randrange(2**8), len(msgs)),
            ''.join(struct.pack('<H', header_length + sum(map(len, msgs[:i]))) for i in xrange(len(msgs))),
            ''.join(msgs),
            struct.pack('<H', random.randrange(2**16)),
        ])
        data_with_checksum = data + struct.pack('<H', sum(map(ord, data)))
        self.transport.write(data_with_checksum)
        #print data_with_checksum.encode('hex')
    
    def connectionLost(self, reason):
        self.listener_set.remove(self)
        print 'dvl connection lost'

class HydrophoneProtocol(protocol.Protocol):
    pass

class MergeProtocol(EmbeddedProtocol):
    local_address = 21
    remote_address = 60
    
    def __init__(self, listener_set):
        self.listener_set = listener_set
    
    def connectionMade(self):
        EmbeddedProtocol.connectionMade(self)
        print 'merge connection made'
        self.listener_set.add(self)
    
    def packetReceived(self, typecode, data):
        print "merge", typecode, data.encode('hex')
    
    def sendUpdate(self, tickcount, flags, current16, voltage16, current32, voltage32):
        data = struct.pack('<HBHHHH', tickcount, flags,
            int(current16 * 2**10 + 0.5),
            int(voltage16 * 2**10 + 0.5),
            int(current32 * 2**10 + 0.5),
            int(voltage32 * 2**10 + 0.5),
        )
        assert len(data) == 11
        self.sendPacket(6, data)
    
    def connectionLost(self, reason):
        self.listener_set.remove(self)
        print 'merge connection lost'


class HeartbeatProtocol(EmbeddedProtocol):
    local_address = 21
    remote_address = 255
    
    def connectionMade(self):
        EmbeddedProtocol.connectionMade(self)
        print 'heartbeat connection made'
    
    def packetReceived(self, typecode, data):
        if typecode is None:
            return # XXX??
        if typecode == 100:
            assert not data
        else:
            print "heartbeat", typecode, data.encode('hex')
    
    def connectionLost(self, reason):
        print 'heartbeat connection lost'

