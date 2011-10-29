from __future__ import division

import itertools
import math
import random
import struct
import time

import ode
from twisted.internet import protocol, reactor, task
import pygame

import datachunker

import vector
from vector import v, V

inf = 1e1000
assert math.isinf(inf)


class AutoServerFactory(protocol.ServerFactory):
    def __init__(self, protocol, *args, **kwargs):
        self.protocol = protocol
        self.protocol_args = args
        self.protocol_kwargs = kwargs
    
    def buildProtocol(self, addr):
        p = self.protocol(*self.protocol_args, **self.protocol_kwargs)
        p.factory = self
        return p

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
                print 'embedded garbage', ''.join(garbage).encode('hex')
            
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
            data = ''.join(data)
            
            if data[-2:] != struct.pack('<H', crc16(data[:-2])):
                print 'embedded invalid', data.encode('hex')
                continue
            data = data[:-2]
            
            (pcaddress, devaddress, packetcount), data = struct.unpack('BBH', data[:4]), data[4:]
            
            self.packetReceived((pcaddress, devaddress, packetcount, data))
    
    def sendPacket(self, devaddress, pcaddress, typecode, data):
        data = struct.pack('BBHB', devaddress, pcaddress, self.packet_count % 2**16, typecode) + data
        data += struct.pack('H', crc16(data))
        
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
    def __init__(self, thruster_id):
        self.thruster_id = thruster_id
    
    def packetReceived(self, pkt):
        print 'thruster', self.thruster_id, pkt

class IMUProtocol(protocol.Protocol):
    def __init__(self, listener_set):
        self.listener_set = listener_set
    
    def connectionMade(self):
        print 'imu connection made'
        self.listener_set.add(self)
    
    def dataReceived(self, data):
        print 'imu', data.encode('hex')
    
    def sendUpdate(self, flags, supply_voltage, ang_rate, acceleration, mag_field, temperature, timestamp):
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

class DepthProtocol(EmbeddedProtocol):
    def __init__(self, listener_set):
        self.listener_set = listener_set
    
    def connectionMade(self):
        EmbeddedProtocol.connectionMade(self)
        print 'depth connection made'
        self.listener_set.add(self)
    
    def packetReceived(self, data):
        print 'depth', data
    
    def sendUpdate(self, tickcount, flags, depth, thermister_temp, humidity, humidity_sensor_temp):
        data = struct.pack('<HBHHHH',
            tickcount, flags, depth*2**10+.5, thermister_temp*2**8+.5,
            humidity+.5, humidity_sensor_temp+.5)
        assert len(data) == 11
        self.sendPacket(40, 21, 4, data)
        #print repr(data)
    
    def connectionLost(self, reason):
        self.listener_set.remove(self)

class ActuatorProtocol(protocol.Protocol):
    def connectionMade(self):
        print 'actuator connection made'
    
    def dataReceived(self, data):
        print 'actuator', data.encode('hex')

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

#class HydrophoneProtocol
#class MergeProtocol


depth_listeners = set()
imu_listeners = set()
dvl_listeners = set()

reactor.listenTCP(10004, AutoServerFactory(DepthProtocol, depth_listeners))
reactor.listenTCP(10025, AutoServerFactory(IMUProtocol, imu_listeners))
for i in xrange(8):
    reactor.listenTCP(10030 + i, AutoServerFactory(ThrusterProtocol, i))
reactor.listenTCP(10050, AutoServerFactory(DVLProtocol, dvl_listeners)) # hydrophone too?
#reactor.listenTCP(10060, AutoServerFactory(protocol=merge_factory))
reactor.listenTCP(10061, AutoServerFactory(ActuatorProtocol))

def send_imu_update(**update):
    for imu_listener in imu_listeners:
        imu_listener.sendUpdate(**update)
def send_depth_update(**update):
    for depth_listener in depth_listeners:
        depth_listener.sendUpdate(**update)
def send_dvl_update(**update):
    for dvl_listener in dvl_listeners:
        dvl_listener.sendHighResVel(**update)


clip = lambda x, (low, high): min(max(x, low), high)

def buoyancy_force(depth, r):
    sphere_antiderivative = lambda h: -h**3*math.pi/3 + h*math.pi*r**2
    sphere_true_antiderivative = lambda h: sphere_antiderivative(clip(h, (-r, r)))
    vol_submerged = sphere_true_antiderivative(depth) - sphere_true_antiderivative(-inf)
    return 1000 * 9.81 * vol_submerged

'''import pygame
d = pygame.display.set_mode((640, 480))
d_pos = 0'''

world, world_time = ode.World(), None
world.setGravity((0, 0, -9.81))
body = ode.Body(world)
M = ode.Mass()
M.setSphere(800, 0.5)
body.setMass(M)
body.setPosition((0, 0, 5))
body.setAngularVel((0, 0, 1))
def world_tick():
    global world_time
    
    last_vel = V(body.vectorFromWorld(body.getLinearVel()))
    
    
    body.addForceAtRelPos((0, 0, buoyancy_force(-body.getPosition()[2], 0.5)), (0, 0, .1))
    body.addForce(-500 * V(body.getLinearVel()))
    body.addForce([random.gauss(0, 10) for i in xrange(3)])
    body.addTorque([random.gauss(0, 100) for i in xrange(3)])
    body.addTorque(-50 * V(body.getAngularVel()))
    
    for keycode, force in [
        (pygame.K_o, v(0, 0, +1500)),
        (pygame.K_m, v(0, 0, -1500)),
        (pygame.K_i, v(+1500, 0, 0)),
        (pygame.K_k, v(-1500, 0, 0)),
        (pygame.K_j, v(0, +1500, 0)),
        (pygame.K_l, v(0, -1500, 0))
    ]:
        if pygame.key.get_pressed()[keycode]:
            body.addForce(force)
    
    new_time = reactor.seconds()
    if world_time is None:
        world_time = new_time-.001
    dt = new_time - world_time
    dt = 1/30
    world.step(dt)
    world_time += dt
    
    #print body.getPosition()
    # addforceatrelpos
    a = (V(body.vectorFromWorld(body.getLinearVel())) - last_vel)/dt - V(body.vectorFromWorld(world.getGravity()))
    #print a
    
    '''global d_pos
    d.set_at((int(d_pos*100), int(240 - 50 * body.getPosition()[2])), (255, 0, 0))
    d.set_at((int(d_pos*100), 240), (0, 255, 0))
    d_pos += dt
    pygame.display.update()
    return'''
    
    dvl_vel = vector.axisangle_to_quat((0, 0, 1), -math.pi/4).quat_rot(body.vectorFromWorld(body.getLinearVel()))
    send_dvl_update(
        bottom_vel=list(dvl_vel) + [0.1], # XXX fourth is error(?)
        bottom_dist=[0, 0, 0, 0],
        water_vel=[0, 0, 0, 0],
        water_dist=[0, 0, 0, 0],
        speed_of_sound=100,
    )
    
    imu_gyro = vector.axisangle_to_quat(v(-1, 1, 0), math.pi).quat_rot(body.vectorFromWorld(body.getAngularVel()))
    imu_acc = vector.axisangle_to_quat(v(-1, 1, 0), math.pi).quat_rot(-a)
    imu_mag = vector.axisangle_to_quat(v(-1, 1, 0), math.pi).quat_rot(body.vectorFromWorld((1, 0, 0)))
    send_imu_update(
        flags=0,
        supply_voltage=random.gauss(12, .1),
        ang_rate=imu_gyro,
        acceleration=imu_acc,
        mag_field=imu_mag,
        temperature=random.gauss(25, .1),
        timestamp=time.time(),
    )
    
    send_depth_update(
        tickcount=0,
        flags=0,
        depth=max(0, -body.getPosition()[2]),
        thermister_temp=random.gauss(25, .1),
        humidity=random.gauss(10, .3),
        humidity_sensor_temp=random.gauss(25, .3),
    )


pygame.display.init()    

task.LoopingCall(world_tick).start(1/30)


if 1:
    import threed
    i = threed.Interface()
    i.init()
    i.objs.append(threed.Sub(body))
    while True:
        i.step()
        reactor.iterate()
else:
    reactor.run()
