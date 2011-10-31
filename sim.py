from __future__ import division

import math
import random
import time

import ode
import pygame
from twisted.internet import protocol, reactor, task

import vector
import devices

v, V = vector.v, vector.V

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


depth_listeners = set()
imu_listeners = set()
dvl_listeners = set()
thrusters = [0]*8

reactor.listenTCP(10004, AutoServerFactory(devices.DepthProtocol, depth_listeners))
reactor.listenTCP(10025, AutoServerFactory(devices.IMUProtocol, imu_listeners))
for i in xrange(8):
    reactor.listenTCP(10030 + i, AutoServerFactory(devices.ThrusterProtocol, i, thrusters))
reactor.listenTCP(10050, AutoServerFactory(devices.DVLProtocol, dvl_listeners)) # hydrophone too?
reactor.listenTCP(10060, AutoServerFactory(devices.MergeProtocol))
reactor.listenTCP(10061, AutoServerFactory(devices.ActuatorProtocol))
reactor.listenTCP(10255, AutoServerFactory(devices.HeartbeatProtocol))

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

def lerp((low, high), x):
    return low + (high - low) * x

world, world_time = ode.World(), reactor.seconds()
world.setGravity((0, 0, 9.81))
body = ode.Body(world)
M = ode.Mass()
M.setSphere(800, 0.5)
body.setMass(M)
body.setPosition((0, 0, -3))

def world_tick():
    global world_time
    
    body.addForceAtRelPos((0, 0, -buoyancy_force(body.getPosition()[2], 0.5)), (0, 0, -.1))
    body.addForce(-1600 * V(body.getLinearVel()))
    body.addForce([random.gauss(0, 1) for i in xrange(3)])
    body.addTorque([random.gauss(0, 10) for i in xrange(3)])
    body.addTorque(-200 * V(body.getAngularVel()))
    
    sub_model.vectors = []
    for i, (reldir, relpos, fwdforce, revforce) in enumerate([
        (v(0, 0, 1), v( 11.7103,  5.3754, -1.9677)*.0254, 500, 500), # FRV
        (v(0, 0, 1), v( 11.7125, -5.3754, -1.9677)*.0254, 500, 500), # FLV
        (v(0,-1, 0), v( 22.3004,  1.8020,  1.9190)*.0254, 500, 500), # FS
        (v(0, 0, 1), v(-11.7125, -5.3754, -1.9677)*.0254, 500, 500), # RLV
        (v(1, 0, 0), v(-24.9072, -4.5375, -2.4285)*.0254, 500, 500), # LFOR
        (v(1, 0, 0), v(-24.9072,  4.5375, -2.4285)*.0254, 500, 500), # RFOR
        (v(0, 1, 0), v(-20.8004, -1.8020,  2.0440)*.0254, 500, 500), # RS
        (v(0, 0, 1), v(-11.7147,  5.3754, -1.9677)*.0254, 500, 500), # RRV
    ]):
        body.addRelForceAtRelPos(reldir*thrusters[i]*(fwdforce if thrusters[i] > 0 else revforce), relpos)
        sub_model.vectors.append((relpos, relpos - .002*reldir*thrusters[i]*(fwdforce if thrusters[i] > 0 else revforce)))
    
    keys = pygame.key.get_pressed()
    for keycode, force in [
        (pygame.K_o, v(0, 0, -1500)),
        (pygame.K_m, v(0, 0, +1500)),
        (pygame.K_i, v(+1500, 0, 0)),
        (pygame.K_k, v(-1500, 0, 0)),
        (pygame.K_j, v(0, -1500, 0)),
        (pygame.K_l, v(0, +1500, 0)),
    ]:
        if keys[keycode]:
            body.addForce(force)
    for keycode, torque in [
        (pygame.K_h, v(0, 0, -500)),
        (pygame.K_SEMICOLON, v(0, 0, +500)),
    ]:
        if keys[keycode]:
            body.addTorque(torque)
    
    last_vel = V(body.vectorFromWorld(body.getLinearVel()))
    
    dt = 1/30
    world.step(dt)
    world_time += dt
    
    a = (V(body.vectorFromWorld(body.getLinearVel())) - last_vel)/dt - V(body.vectorFromWorld(world.getGravity()))
    
    imu_to_sub = v(0.012621022547474, 0.002181321593961, -0.004522523520991, 0.999907744947984)
    imu_gyro = imu_to_sub.conj().quat_rot(body.vectorFromWorld(body.getAngularVel()))
    imu_acc = imu_to_sub.conj().quat_rot(-a)
    
    north = v(0.241510, -0.22442, 0.405728)
    magCorrection = v(0.673081592748511, -0.207153644511562, -0.546990360033963, 0.452603671105992)
    magShift = v(0.722492613515378, -0.014544506498174, 0.283264416021074)
    magScale = v(0.963005126569852, 0.980211159628685, 1.000855176757894)

    imu_mag_prescale = imu_to_sub.conj().quat_rot(body.vectorFromWorld(north))
    imu_mag = magCorrection.conj().quat_rot(magCorrection.quat_rot(imu_mag_prescale).scale(magScale)) + magShift
    
    send_imu_update(
        flags=0,
        supply_voltage=random.gauss(12, .1),
        ang_rate=imu_gyro,
        acceleration=imu_acc,
        mag_field=imu_mag,
        temperature=random.gauss(25, .1),
        timestamp=world_time,
    )
    
    reactor.callLater(max(0, world_time + dt - reactor.seconds()), world_tick)


if 1:
    import threed
    i = threed.Interface()
    i.init()
    sub_model = threed.Sub(body)
    i.objs.append(sub_model)
    task.LoopingCall(i.step).start(1/24)

world_tick()

def dvl_task():
    dvl_to_sub = v(0.0, 0.923879532511287, 0.382683432365090, 0.0)
    dvl_vel = dvl_to_sub.conj().quat_rot(body.vectorFromWorld(body.getLinearVel()))
    send_dvl_update(
        bottom_vel=list(dvl_vel) + [0.1], # XXX fourth is error(?)
        bottom_dist=[0, 0, 0, 0],
        water_vel=[0, 0, 0, 0],
        water_dist=[0, 0, 0, 0],
        speed_of_sound=100,
    )
task.LoopingCall(dvl_task).start(1/5)

def depth_task():
    send_depth_update(
        tickcount=0,
        flags=0,
        depth=max(0, body.getPosition()[2]),
        thermister_temp=random.gauss(25, .1),
        humidity=random.gauss(10, .3),
        humidity_sensor_temp=random.gauss(25, .3),
    )
task.LoopingCall(depth_task).start(1/5)


reactor.run()
