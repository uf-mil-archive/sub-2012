from __future__ import division

import math
import random
import traceback

import ode
import pygame
from twisted.internet import protocol, reactor, task

import vector
import devices
import threed

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
merge_listeners = set()
thrusters = [0]*8

reactor.listenTCP(10004, AutoServerFactory(devices.DepthProtocol, depth_listeners))
reactor.listenTCP(10025, AutoServerFactory(devices.IMUProtocol, imu_listeners))
for i in xrange(8):
    reactor.listenTCP(10030 + i, AutoServerFactory(devices.ThrusterProtocol, i, thrusters))
reactor.listenTCP(10050, AutoServerFactory(devices.DVLProtocol, dvl_listeners)) # hydrophone too?
reactor.listenTCP(10060, AutoServerFactory(devices.MergeProtocol, merge_listeners))
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
def send_merge_update(**update):
    for merge_listener in merge_listeners:
        merge_listener.sendUpdate(**update)


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
M.setSphere(800, 0.4)
body.setMass(M)
body.setPosition((3, 3, -3))

space = ode.HashSpace()

body_geom = ode.GeomSphere(space, 0.4)
body_geom.setBody(body)

pool_mesh = threed.mesh_from_obj(open('scene/pool6_Scene.obj'))
pool_geom = ode.GeomTriMesh(pool_mesh.ode_trimeshdata, space)

def get_water_vel(pos):
    return (pos % v(0, 0, 1))*math.e**(-pos.mag()/3)

imu_pos = v(0.43115992, 0.0, -0.00165058)

def world_tick():
    global world_time
    
    water_vel = get_water_vel(V(body.getPosition()))
    
    body.addForceAtRelPos((0, 0, -buoyancy_force(body.getPosition()[2], 0.4)), (0, 0, -.1))
    body.addForce(-(1600 if body.getPosition()[2] >= 0 else 160) * (V(body.getLinearVel())-water_vel))
    body.addForce([random.gauss(0, 1) for i in xrange(3)])
    body.addTorque([random.gauss(0, 10) for i in xrange(3)])
    body.addTorque(-(200 if body.getPosition()[2] >= 0 else 20) * V(body.getAngularVel()))
    
    #print thrusters
    
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
        if body.getRelPointPos(relpos)[2] < 0: # skip thrusters that are out of the water
            continue
        body.addRelForceAtRelPos(reldir*thrusters[i]*(fwdforce if thrusters[i] > 0 else revforce), relpos)
        sub_model.vectors.append((relpos, relpos - .002*reldir*thrusters[i]*(fwdforce if thrusters[i] > 0 else revforce)))
    
    keys = pygame.key.get_pressed()
    for keycode, force in [
        (pygame.K_k, v(-1000, 0, 0)),
        (pygame.K_i, v(+1000, 0, 0)),
        (pygame.K_j, v(0, -1000, 0)),
        (pygame.K_l, v(0, +1000, 0)),
        (pygame.K_o, v(0, 0, -1000)),
        (pygame.K_m, v(0, 0, +1000)),
    ]:
        if keys[keycode]:
            body.addRelForce(force*(10 if keys[pygame.K_RSHIFT] else 1))
    for keycode, torque in [
        (pygame.K_COMMA, v(-200, 0, 0)),
        (pygame.K_u, v(+200, 0, 0)),
        (pygame.K_h, v(0, -200, 0)),
        (pygame.K_SEMICOLON, v(0, +200, 0)),
        (pygame.K_0, v(0, 0, -200)),
        (pygame.K_n, v(0, 0, +200)),
    ]:
        if keys[keycode]:
            body.addRelTorque(torque*(10 if keys[pygame.K_RSHIFT] else 1))
    
    last_vel = V(body.vectorFromWorld(body.getRelPointVel(imu_pos)))
    
    contactgroup = ode.JointGroup()
    near_pairs = []
    space.collide(None, lambda _, geom1, geom2: near_pairs.append((geom1, geom2)))
    for geom1, geom2 in near_pairs:
        for contact in ode.collide(geom1, geom2):
            contact.setBounce(0.2)
            contact.setMu(5000)
            j = ode.ContactJoint(world, contactgroup, contact)
            j.attach(geom1.getBody(), geom2.getBody())
    
    dt = 1/30
    world.step(dt)
    world_time += dt
    
    contactgroup.empty()
    
    a = (V(body.vectorFromWorld(body.getRelPointVel(imu_pos))) - last_vel)/dt - V(body.vectorFromWorld(world.getGravity()))
    
    imu_to_sub = v(0.012621022547474, 0.002181321593961, -0.004522523520991, 0.999907744947984)
    imu_gyro = imu_to_sub.conj().quat_rot(body.vectorFromWorld(body.getAngularVel()))
    imu_acc = imu_to_sub.conj().quat_rot(-a)
    
    north = v(0.241510, -0.22442, 0.405728)
    magCorrection = v(0.673081592748511, -0.207153644511562, -0.546990360033963, 0.452603671105992)
    magShift = v(0.722492613515378, -0.014544506498174, 0.283264416021074)
    magScale = v(0.963005126569852, 0.980211159628685, 1.000855176757894)
    
    imu_mag_prescale = imu_to_sub.conj().quat_rot(body.vectorFromWorld(north))
    imu_mag = magCorrection.conj().quat_rot(magCorrection.quat_rot(imu_mag_prescale).scale(magScale)) + magShift
    try:
        send_imu_update(
            flags=0,
            supply_voltage=random.gauss(12, .1),
            ang_rate=imu_gyro,
            acceleration=imu_acc,
            mag_field=imu_mag,
            temperature=random.gauss(25, .1),
            timestamp=world_time,
        )
    except:
        traceback.print_exc()
    
    reactor.callLater(max(0, world_time + dt - reactor.seconds()), world_tick)


i = threed.Interface()
i.init()
i.objs.append(threed.MeshDrawer(pool_mesh, (.4, .4, .4)))
sub_model = threed.Sub(body)
i.objs.append(sub_model)
i.objs.append(threed.VectorField(get_water_vel))
def _():
    try:
        i.step()
    except SystemExit:
        reactor.stop()
task.LoopingCall(_).start(1/24)

world_tick()

def dvl_task():
    dvl_to_sub = v(0.0, 0.923879532511287, 0.382683432365090, 0.0)
    dvl_vel = dvl_to_sub.conj().quat_rot(body.vectorFromWorld(body.getRelPointVel(imu_pos)))
    try:
        send_dvl_update(
            bottom_vel=list(dvl_vel) + [0.1], # XXX fourth is error(?)
            bottom_dist=[0, 0, 0, 0],
            water_vel=[0, 0, 0, 0],
            water_dist=[0, 0, 0, 0],
            speed_of_sound=100,
        )
    except:
        traceback.print_exc()
task.LoopingCall(dvl_task).start(1/5)

def depth_task():
    try:
        send_depth_update(
            tickcount=0,
            flags=0,
            depth=max(0, body.getRelPointPos(imu_pos)[2]),
            thermister_temp=random.gauss(25, .1),
            humidity=random.gauss(10, .3),
            humidity_sensor_temp=random.gauss(25, .3),
        )
    except:
        traceback.print_exc()
task.LoopingCall(depth_task).start(1/5)

def merge_task():
    try:
        send_merge_update(
            tickcount=0,
            flags=0,
            current16=random.gauss(10, 0.1),
            voltage16=random.gauss(16, 0.1),
            current32=random.gauss(10, 0.1),
            voltage32=random.gauss(32, 0.1),
        )
    except:
        traceback.print_exc()
task.LoopingCall(merge_task).start(1/5)

reactor.run()
