#!/usr/bin/env python

from __future__ import division

from OpenGL.GL import *
from OpenGL.GLU import *

import pygame
import random
import time
import math
import sys

import numpy
from vector import v, V

# this is really not important
def angleaxis_matrix(angle, (x, y, z)):
    s = math.sin(angle)
    c = math.cos(angle)
    one_c = 1. - c
    xx = x * x
    yy = y * y
    zz = z * z
    xy = x * y
    yz = y * z
    zx = z * x
    xs = x * s
    ys = y * s
    zs = z * s
    m = numpy.array([
        [(one_c * xx) + c, (one_c * xy) - zs, (one_c * zx) + ys, 0.],
        [(one_c * xy) + zs, (one_c * yy) + c, (one_c * yz) - xs, 0.],
        [(one_c * zx) - ys, (one_c * yz) + xs, (one_c * zz) + c, 0.],
        [0., 0., 0., 1.],
    ])
    return m


def euler_matrix(yaw, pitch, roll):
    return numpy.dot(angleaxis_matrix(roll, (1, 0, 0)), numpy.dot(angleaxis_matrix(pitch, (0, 1, 0)), angleaxis_matrix(yaw, (0, 0, 1))))
def rotate_vec(vec, m):
    x = numpy.dot((vec[0], vec[1], vec[2], 1), m)
    return V(x[:3])/x[3]

def mesh_from_obj(file):
    vertices = []
    texcoords = []
    normals = []
    indices = []
    for line in file:
        line = line.strip()
        if '#' in line:
            line = line[:line.index("#")]
        if not line: continue
        line = line.split(' ')
        if line[0] == "v":
            vertices.append(V(float(x) for x in line[1:]))
        if line[0] == "vt":
            texcoords.append(V(float(x) for x in line[1:]))
        if line[0] == "vn":
            normals.append(V(float(x) for x in line[1:]))
        elif line[0] == "f":
            indices.append(tuple(tuple(int(y)-1 if y else None for y in x.split('/')) for x in line[1:]))
    return Mesh(vertices, texcoords, normals, indices)

class Mesh(object):
    def __init__(self, vertices, texcoords, normals, indices):
        self.vertices = vertices
        self.texcoords = texcoords
        self.normals = normals
        self.indices = indices
    
    def draw(self):
        glBegin(GL_TRIANGLES)
        for triangle in self.indices:
            for vert_index, tex_index, normal_index in triangle:
                if tex_index is not None:
                    glTexCoord2f(*self.texcoords[tex_index])
                if normal_index is not None:
                    glNormal3f(*self.normals[normal_index])
                glVertex3f(*self.vertices[vert_index])
        glEnd()
    
    @property
    def ode_trimeshdata(self):
        import ode
        x = ode.TriMeshData()
        x.build(self.vertices, [[p[0] for p in t] for t in self.indices])
        return x


def rotate_to_body(body, inv=False):
    R = body.getRotation()
    p = body.getPosition()
    rot = [[R[0], R[3], R[6], 0.],
        [R[1], R[4], R[7], 0.],
        [R[2], R[5], R[8], 0.],
        [p[0], p[1], p[2], 1.]]
    if inv:
        rot = numpy.linalg.inv(rot)
    rot = list(rot[0])+list(rot[1])+list(rot[2])+list(rot[3])
    glMultMatrixd(rot)

class Sub(object):
    def __init__(self, body):
        self.body = body
        self.vectors = []
    
    def draw(self):
        glPushMatrix()
        rotate_to_body(self.body)
        glTranslate(.2, 0, 0)
        q = gluNewQuadric()
        glColor3f(0, 1, 0)
        gluSphere(q, .5, 20, 20)
        glTranslate(-.4, 0, 0)
        glColor3f(1, 0, 0)
        gluSphere(q, .5, 20, 20)
        
        glDisable(GL_DEPTH_TEST)
        glBegin(GL_LINES)
        for start, end in self.vectors:
            glColor3f(0, 0, 0)
            glVertex3f(*start)
            glColor3f(1, 1, 1)
            glVertex3f(*end)
        glEnd()
        glEnable(GL_DEPTH_TEST)
        
        glPopMatrix()


class Interface(object):
    def init(self):
        global display, clock, pitch, yaw, roll, grabbed
        self.display = pygame.display.set_mode((700, 400), pygame.DOUBLEBUF|pygame.OPENGL)
        self.clock = pygame.time.Clock()

        self.pitch = self.yaw = self.roll = 0
        self.grabbed = False

        self.t = 0

        glEnable(GL_DEPTH_TEST)

        self.pos = v(-10,0,-2)
        
        self.pool_mesh = mesh_from_obj(open('pool5_Scene.obj'))
        
        self.objs = []

    def step(self):
        dt = self.clock.tick()/1000
        self.t += dt
        
        for event in pygame.event.get():
            if event.type == pygame.MOUSEMOTION:
                if self.grabbed:
                    self.yaw += -event.rel[0]/100
                    # y is reversed since opengl coordinates start from 0,0 at bottom left
                    # while Xorg starts from 0,0 at top left
                    self.pitch += event.rel[1]/100
                    # caps it to a quarter turn up or down
                    self.pitch = min(max(self.pitch, -math.pi/2), math.pi/2)
            elif event.type == pygame.MOUSEBUTTONDOWN:
                pass
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_TAB:
                    self.grabbed = not self.grabbed
                    pygame.event.set_grab(self.grabbed)
                    pygame.mouse.set_visible(not self.grabbed)
                elif event.key == pygame.K_q:
                    sys.exit()
        
        rot_matrix = euler_matrix(self.yaw, self.pitch, self.roll)
        
        forward = rotate_vec(v(1,0,0), rot_matrix)
        left = rotate_vec(v(0,-1,0), rot_matrix)
        local_up = rotate_vec(v(0,0,-1), rot_matrix)
        
        keys = pygame.key.get_pressed()
        speed = 10
        if keys[pygame.K_LSHIFT]: speed *= 10
        
        if keys[pygame.K_w]: self.pos += forward*dt*speed
        if keys[pygame.K_s]: self.pos += -forward*dt*speed
        if keys[pygame.K_a]: self.pos += left*dt*speed
        if keys[pygame.K_d]: self.pos += -left*dt*speed
        if keys[pygame.K_SPACE]: self.pos += v(0, 0, -1)*dt*speed
        if keys[pygame.K_c]: self.pos += v(0, 0, 1)*dt*speed
        
        self.pos += v(0,0,-1)*max(0, -(-self.pos[2] - 1))
        self.pos += v(0,0,1)*max(0, (-self.pos[2] - 100))
        
        # clears the color buffer (what you see)
        # and the z-buffer (the distance into the screen of every pixel)
        glClearColor(0,0,1,1)
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
        
        # it switches into manipulating the projection matrix
        glMatrixMode(GL_PROJECTION)
        # then clears it
        glLoadIdentity()
        # then sets it using the utility function to a perspective with a fov of 90
        # an aspect ratio of the correct ratio
        # a near distance of .1
        # a far distance of 1000
        # things outside of that distance range won't be drawn
        # lets decrease 1000 to 20 to see
        gluPerspective(90, self.display.get_width()/self.display.get_height(), .1, 1000) # fov, aspect ratio, near distance, far distance
        
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        # rotates into the FRD coordinate system
        glMultMatrixf([
            [ 0., 0.,-1., 0.],
            [ 1., 0., 0., 0.],
            [ 0.,-1., 0., 0.],
            [ 0., 0., 0., 1.]
        ])
        # after that, +x is forward, +y is right, and +z is down
        
        # rotates the scene
        #print rot_matrix
        glMultMatrixf(rot_matrix.T)
        #glRotate(math.degrees(roll), 1, 0, 0)
        #glRotate(math.degrees(pitch), 0, 1, 0)
        #glRotate(math.degrees(yaw), 0, 0, 1)
        
        # moves the camera to (10,10,10) by moving the scene (-10,-10,-10)
        glTranslate(*-self.pos)
        
        glEnable ( GL_LIGHTING )
        glEnable(GL_LIGHT0)
        glEnable(GL_COLOR_MATERIAL)
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, [0, 0, 0, 1])
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, [0, 0, 0, 1])
        glLightfv(GL_LIGHT0, GL_POSITION, [math.sin(self.t/5)*100, math.cos(self.t/5)*100, -100, 1])
        glLightfv(GL_LIGHT0, GL_AMBIENT, [0, 0, 0, 1])
        glLightfv(GL_LIGHT0, GL_DIFFUSE, [.5, .5, .5, 1])
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, [.5, .5, .5])
        
        self.draw()
        
        pygame.display.flip()
    
    def draw(self):
        glEnable(GL_CULL_FACE)
        
        glColor3f(.4, .4, .4)
        
        glPushMatrix()
        glRotate(180, 1, 0, 0)
        self.pool_mesh.draw()
        glPopMatrix()
        
        for obj in self.objs:
            obj.draw()
        
        glPushMatrix()
        glTranslate(math.sin(self.t/5)*100, math.cos(self.t/5)*100, -100)
        q = gluNewQuadric()
        glDisable(GL_LIGHTING)
        glColor3f(1, 1, 1)
        gluSphere(q, 30, 20, 20)
        glEnable(GL_LIGHTING)
        glPopMatrix()
        
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glBegin(GL_QUADS)
        glNormal3f(0, 0, 1)
        glColor4f(0, 0, 1, .5)
        glVertex3f(-125, -125, 0)
        glVertex3f(-125, +125, 0)
        glVertex3f(+125, +125, 0)
        glVertex3f(+125, -125, 0)
        glEnd()
        glDisable(GL_BLEND)
        
        return
        
        glBegin(GL_QUADS)
        # sets the normal vector to +z (up, remember?)
        glNormal3f(0, 0, 1)
        # for each corner, sets the texture coordinate
        # texture coordinates go from 0,0 for the bottom left to 1,1 for top right
        # ones outside that range wrap around
        # so this quad will have 200x200 of the texture repeated
        # the vertex coordinates are the position
        # they are all on the x-y plane (the ground)
        glTexCoord2f(-100, -100);glVertex3f(-1000+self.pos[0]-self.pos[0]%10, -1000+self.pos[1]-self.pos[1]%10, 0)
        glTexCoord2f(100, -100);glVertex3f(1000+self.pos[0]-self.pos[0]%10, -1000+self.pos[1]-self.pos[1]%10, 0)
        glTexCoord2f(100, 100);glVertex3f(1000+self.pos[0]-self.pos[0]%10, 1000+self.pos[1]-self.pos[1]%10, 0)
        glTexCoord2f(-100, 100);glVertex3f(-1000+self.pos[0]-self.pos[0]%10, 1000+self.pos[1]-self.pos[1]%10, 0)
        # ends the list of quads
        glEnd()
        
        glPushMatrix()
        t2 = self.t % 1
        glTranslate(0, 0, 9.8/2*(-t2**2+t2)+1)
        glRotate(self.t*180, 0, 0, 1)
        for i in xrange(8):
            
            q = gluNewQuadric() # this makes a quadric object, which is used to draw spheres
            gluQuadricTexture(q, True) # this tells the object to generate texture coordinates
            glColor3f(1, 0, 0)
            gluSphere(q, 1, 20, 20) # this draws a sphere at the local origin (but it is translated) with a radius of 1, and 10 lines of longitude and latitude
            glTranslate(2,0,0)
        glPopMatrix()
        
        glTranslate(0, 0, 5)
        glBegin(GL_LINES)
        for a in xrange(3):
            glColor3d(0,0,0)
            glVertex3d(0,0,0)
            glColor3d(*[1 if a==i else 0 for i in xrange(3)])
            glVertex3d(*[1 if a==i else 0 for i in xrange(3)])
        glEnd()

        
        # finished drawing, now flip

if __name__ == '__main__':
    i = Interface()
    i.init()
    while True:
        i.step()
