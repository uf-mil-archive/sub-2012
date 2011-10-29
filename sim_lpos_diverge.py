from __future__ import division

import struct
import time
import random

from twisted.internet import protocol, reactor, task

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
    
    def dataReceiver(self):
        while True:
            # wait for 7E
            while True:
                b = yield 1
                if b == '\x7e':
                    break
            
            data = []
            while True:
                b = yield 1
                if b == '\x7e' and data: # accept ignore leading 7E - zero length message will never happen, so we're out of sync
                    break
                elif b == '\x7d':
                    data.append(chr(ord((yield 1)) ^ 0x20))
                else:
                    data.append(b)
            data = ''.join(data)
            
            if data[-2:] != struct.pack("<H", crc16(data[:-2])):
                print "invalid", data[-2:].encode('hex'), struct.pack("<H", crc16(data[:-2])).encode('hex')
                continue
            data = data[:-2]
            
            (pcaddress, devaddress, packetcount), data = struct.unpack("BBH", data[:4]), data[4:]
            
            self.packetReceived((pcaddress, devaddress, packetcount, data))
    
    def sendPacket(self, devaddress, pcaddress, typecode, token, data):
        # XXX
        data = struct.pack('BBHB', devaddress, pcaddress, 0, typecode,) + data
        data += struct.pack('H', crc16(data))
        print "sending", data.encode('hex')
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

class ThrusterReceiver(protocol.DatagramProtocol):
    def __init__(self, thruster_id):
        self.thruster_id = thruster_id
    
    def datagramReceived(self, datagram, addr):
        print self.thruster_id, datagram.encode('hex')

imu_listeners = set()
class IMUInterface(protocol.Protocol):
    def connectionMade(self):
        print "imu connection made"
        imu_listeners.add(self)
    
    def dataReceived(self, data):
        print "imu", data.encode('hex')
    
    def sendUpdate(self, flags, supply_voltage, ang_rate, acceleration, mag_field, temperature, timestamp):
        data = ''.join([
            struct.pack("<HH", flags, supply_voltage/0.00242+.5),
            struct.pack("<3h", *(x/0.000872664626+.5 for x in ang_rate)),
            struct.pack("<3h", *(x/0.0033+.5 for x in acceleration)),
            struct.pack("<3h", *(x/0.0005+.5 for x in mag_field)),
            struct.pack("<hq", (temperature - 25)/0.14+.5, timestamp*1e9+.5),
        ])
        assert len(data) == 32
        self.transport.write(data)
    
    def connectionLost(self, reason):
        imu_listeners.remove(self)

depth_listeners = set()
class DepthInterface(EmbeddedProtocol):
    def connectionMade(self):
        EmbeddedProtocol.connectionMade(self)
        print "depth connection made"
        depth_listeners.add(self)
    
    def packetReceived(self, data):
        print "depth", data
    
    def sendUpdate(self, tickcount, flags, depth, thermister_temp, humidity, humidity_sensor_temp):
        data = struct.pack("<HBHHHH",
            tickcount, flags, depth*2**10+.5, thermister_temp*2**8+.5,
            humidity+.5, humidity_sensor_temp+.5)
        assert len(data) == 11
        self.sendPacket(40, 21, 4, 0, data)
        #print repr(data)
    
    def connectionLost(self, reason):
        depth_listeners.remove(self)

class Actuator(protocol.Protocol):
    def connectionMade(self):
        print "actuator connection made"
    
    def dataReceived(self, data):
        print "actuator", data.encode('hex')

dvl_listeners = set()
class Hydrophone(protocol.Protocol):
    def connectionMade(self):
        print "hydrophone/dvl connection made"
        dvl_listeners.add(self)
    
    def dataReceived(self, data):
        print "hydrophone/dvl", repr(data)
    
    def sendHighResVel(self, bottom_vel, bottom_dist, water_vel, water_dist, speed_of_sound):
        self.sendUpdate(['\x03\x58' +
            struct.pack('16i', *[x*1e5+.5 for x in list(bottom_vel) + list(bottom_dist) + list(water_vel) + list(water_dist)]) +
            struct.pack('i', speed_of_sound*1e6+.5)
        ])
    
    def sendUpdate(self, msgs):
        header_length = 1 + 1 + 2 + 1 + 1 + 2 * len(msgs)
        data = ''.join([
            '\x7f\x7f',
            struct.pack("<HBB", header_length + sum(map(len, msgs)) + 2, random.randrange(2**8), len(msgs)),
            ''.join(struct.pack("<H", header_length + sum(map(len, msgs[:i]))) for i in xrange(len(msgs))),
            ''.join(msgs),
            struct.pack("<H", 0*random.randrange(2**16)),
        ])
        data_with_checksum = data + struct.pack("<H", sum(map(ord, data)))
        print data_with_checksum.encode('hex')
        self.transport.write(data_with_checksum)
    
    def connectionLost(self, reason):
        dvl_listeners.remove(self)

for i in xrange(8):
    reactor.listenUDP(10030 + i, ThrusterReceiver(i))

depth_factory = protocol.ServerFactory()
depth_factory.protocol = DepthInterface
reactor.listenTCP(10004, depth_factory)

imu_factory = protocol.ServerFactory()
imu_factory.protocol = IMUInterface
reactor.listenTCP(10025, imu_factory)

# merge 60

actuator_factory = protocol.ServerFactory()
actuator_factory.protocol = Actuator
reactor.listenTCP(10061, actuator_factory)

hydrophone_factory = protocol.ServerFactory()
hydrophone_factory.protocol = Hydrophone
reactor.listenTCP(10050, hydrophone_factory)

def send_imu_update():
    update = dict(
        flags=0,
        supply_voltage=random.gauss(12, .1),
        ang_rate=[random.gauss(0, .1) for i in xrange(3)],
        acceleration=[random.gauss(0, .1) for i in xrange(3)],
        mag_field=[random.gauss(0, .1) for i in xrange(3)],
        temperature=random.gauss(25, .1),
        timestamp=time.time(),
    )
    for imu_listener in imu_listeners:
        imu_listener.sendUpdate(**update)
task.LoopingCall(send_imu_update).start(1/20)

def send_depth_update():
    update = dict(
        tickcount=0,
        flags=0,
        depth=random.gauss(10, .1),
        thermister_temp=random.gauss(25, .1),
        humidity=random.gauss(10, .1),
        humidity_sensor_temp=random.gauss(25, .1),
    )
    for depth_listener in depth_listeners:
        depth_listener.sendUpdate(**update)
task.LoopingCall(send_depth_update).start(1/10)

def send_dvl_update():
    for dvl_listener in dvl_listeners:
        dvl_listener.sendHighResVel([random.gauss(0, .1), 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0], 100)
task.LoopingCall(send_dvl_update).start(1/10)

reactor.run()
