#!/usr/bin/python
import pymysql
import os
import time
import sys
import pygame


from pygame import mixer
from pygame.locals import *

flag = 0

while True:
    db = pymysql.connect(host="sgcs1416.cafe24.com",    # your host, usually localhost
                     user="kinkin21c",         # your username
                     passwd="jenjen21c",  # your password
                     db="kinkin21c")        # name of the data base

# you must create a Cursor object. It will let
# you execute all the queries you need
    cur = db.cursor()

    #print(cur.execute("SELECT * FROM temp"))
    cnt = cur.execute("SELECT * FROM temp")
    #if cnt >=10 :

    for row in cur.fetchall():
        if row[1] >= 30:
            flag = 1
            
        while flag:  
            pygame.mixer.init()
            pygame.mixer.music.load("floor1.mp3")
            pygame.mixer.music.play()
            time.sleep(9)


   

db.close()
