#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Mechanical Clock Power Board v3.0 — 30x SG90 Servo / 10A
Generates EasyEDA Standard Edition JSON files for LCEDA Pro import.
"""

import json
import zipfile
import os
import sys

# Force UTF-8 on Windows
if sys.platform == 'win32':
    sys.stdout.reconfigure(encoding='utf-8')

OUT_DIR = os.path.dirname(os.path.abspath(__file__))

# ============================================================
# Design Parameters
# ============================================================
BOARD_W = 65.0   # mm
BOARD_H = 50.0   # mm
COPPER_OZ = 2
TRACK_SERVO = 5.0   # mm, servo rail (10A)
TRACK_LOGIC = 1.0   # mm, logic rail
TRACK_3V3 = 0.6     # mm, 3.3V rail

# Coordinate conversion: 1 unit = 0.254mm = 10mil
def mm2u(mm):
    return round(mm / 0.254)

# ============================================================
# Schematic Generation
# ============================================================

def gen_schematic():
    shapes = []
    gid = 1

    def add_shape(s):
        nonlocal gid
        shapes.append(s)
        gid += 1

    # --- J1: 5V_IN screw terminal KF128-2P ---
    x, y = 180, 380
    add_shape(
        f"LIB~{x}~{y}~package`CONN-2`nameAlias`KF128-2P`Value`5V_IN`pre`J1`~~0~"
        f"gge{gid}#@$T~N~{x-40}~{y-40}~0~#000080~Arial~~~~~comment~5V_IN (LRS +V/-V)~1~start~"
        f"gge{gid+1}#@$T~P~{x-40}~{y-50}~0~#000080~Arial~~~~~comment~J1~1~start~"
        f"gge{gid+2}#@$R~{x-15}~{y-20}~0~0~30~40~#800000~1~0~#FFFFFF~"
        f"gge{gid+3}#@$P~show~0~1~{x-15}~{y-10}~0~"
        f"gge{gid+4}^^{x-15}~{y-10}^^M {x-15} {y-10} h -20~#880000^^1~{x-21}~{y-5}~0~1~end~~^^0~{x-13}~{y-13}~0~1~start~~^^0~{x-12}~{y-10}^^0~"
        f"#@$P~show~0~2~{x-15}~{y+10}~0~"
        f"gge{gid+5}^^{x-15}~{y+10}^^M {x-15} {y+10} h -20~#880000^^1~{x-21}~{y+15}~0~1~end~~^^0~{x-13}~{y+7}~0~1~start~~^^0~{x-12}~{y+10}^^0~"
    ); gid += 6

    # --- L1: 10uH/10A toroidal inductor ---
    x, y = 320, 380
    add_shape(
        f"LIB~{x}~{y}~package`INDUCTOR`nameAlias`TOROID-10A`Value`10uH/10A`pre`L1`~~0~"
        f"gge{gid}#@$T~N~{x-20}~{y-30}~0~#000080~Arial~~~~~comment~10uH/10A~1~start~"
        f"gge{gid+1}#@$T~P~{x-20}~{y-40}~0~#000080~Arial~~~~~comment~L1~1~start~"
        f"gge{gid+2}#@$PT~M {x-25},{y-5} A 7.5,7.5 0 0 1 {x-10},{y+5}~#880000~1~0~none~"
        f"gge{gid+3}#@$PT~M {x-10},{y-5} A 7.5,7.5 0 0 1 {x+5},{y+5}~#880000~1~0~none~"
        f"gge{gid+4}#@$PT~M {x+5},{y-5} A 7.5,7.5 0 0 1 {x+20},{y+5}~#880000~1~0~none~"
        f"gge{gid+5}#@$PT~M {x+20},{y-5} A 7.5,7.5 0 0 1 {x+35},{y+5}~#880000~1~0~none~"
        f"gge{gid+6}#@$P~show~0~1~{x-30}~{y}~0~"
        f"gge{gid+7}^^{x-30}~{y}^^M {x-30} {y} h -20~#880000^^1~{x-36}~{y+5}~0~1~end~~^^0~{x-28}~{y-3}~0~1~start~~^^0~{x-27}~{y}^^0~"
        f"#@$P~show~0~2~{x+40}~{y}~0~"
        f"gge{gid+8}^^{x+40}~{y}^^M {x+40} {y} h 20~#880000^^1~{x+46}~{y+5}~0~1~start~~^^0~{x+38}~{y-3}~0~1~end~~^^0~{x+43}~{y}^^0~"
    ); gid += 9

    # --- C1: 470uF/16V electrolytic ---
    x, y = 460, 380
    add_shape(
        f"LIB~{x}~{y}~package`CAP-DIP`nameAlias`D10-P5`Value`470uF/16V`pre`C1`~~0~"
        f"gge{gid}#@$T~N~{x-20}~{y-30}~0~#000080~Arial~~~~~comment~470uF/16V~1~start~"
        f"gge{gid+1}#@$T~P~{x-20}~{y-40}~0~#000080~Arial~~~~~comment~C1~1~start~"
        f"gge{gid+2}#@$PT~M {x-10},{y-15} A 5,15 0 0 1 {x-10},{y+15}~#880000~1~0~none~"
        f"gge{gid+3}#@$PL~{x+10} {y-15} {x+10} {y+15}~#880000~1~0~none~"
        f"gge{gid+4}#@$PL~{x+13} {y-18} {x+13} {y-22}~#0000FF~1~0~none~"
        f"gge{gid+5}#@$PL~{x+10} {y-20} {x+16} {y-20}~#0000FF~1~0~none~"
        f"gge{gid+6}#@$P~show~0~1~{x-15}~{y}~0~"
        f"gge{gid+7}^^{x-15}~{y}^^M {x-15} {y} h -20~#880000^^1~{x-21}~{y+5}~0~1~end~~^^0~{x-13}~{y-3}~0~1~start~~^^0~{x-12}~{y}^^0~"
        f"#@$P~show~0~2~{x+15}~{y}~0~"
        f"gge{gid+8}^^{x+15}~{y}^^M {x+15} {y} h 20~#880000^^1~{x+21}~{y+5}~0~1~start~~^^0~{x+13}~{y-3}~0~1~end~~^^0~{x+18}~{y}^^0~"
    ); gid += 9

    # --- J2: 5V_SERVO screw terminal KF128-2P ---
    x, y = 600, 380
    add_shape(
        f"LIB~{x}~{y}~package`CONN-2`nameAlias`KF128-2P`Value`5V_SERVO`pre`J2`~~0~"
        f"gge{gid}#@$T~N~{x-40}~{y-40}~0~#000080~Arial~~~~~comment~5V_SERVO (PCA9685x2 舵机轨)~1~start~"
        f"gge{gid+1}#@$T~P~{x-40}~{y-50}~0~#000080~Arial~~~~~comment~J2~1~start~"
        f"gge{gid+2}#@$R~{x-15}~{y-20}~0~0~30~40~#800000~1~0~#FFFFFF~"
        f"gge{gid+3}#@$P~show~0~1~{x-15}~{y-10}~0~"
        f"gge{gid+4}^^{x-15}~{y-10}^^M {x-15} {y-10} h -20~#880000^^1~{x-21}~{y-5}~0~1~end~~^^0~{x-13}~{y-13}~0~1~start~~^^0~{x-12}~{y-10}^^0~"
        f"#@$P~show~0~2~{x-15}~{y+10}~0~"
        f"gge{gid+5}^^{x-15}~{y+10}^^M {x-15} {y+10} h -20~#880000^^1~{x-21}~{y+15}~0~1~end~~^^0~{x-13}~{y+7}~0~1~start~~^^0~{x-12}~{y+10}^^0~"
    ); gid += 6

    # --- J5: 5V_SPARE pin header ---
    x, y = 740, 380
    add_shape(
        f"LIB~{x}~{y}~package`CONN-2`nameAlias`HDR-2P`Value`5V_SPARE`pre`J5`~~0~"
        f"gge{gid}#@$T~N~{x-40}~{y-40}~0~#000080~Arial~~~~~comment~5V_SPARE~1~start~"
        f"gge{gid+1}#@$T~P~{x-40}~{y-50}~0~#000080~Arial~~~~~comment~J5~1~start~"
        f"gge{gid+2}#@$R~{x-15}~{y-20}~0~0~30~40~#800000~1~0~#FFFFFF~"
        f"gge{gid+3}#@$P~show~0~1~{x-15}~{y-10}~0~"
        f"gge{gid+4}^^{x-15}~{y-10}^^M {x-15} {y-10} h -20~#880000^^1~{x-21}~{y-5}~0~1~end~~^^0~{x-13}~{y-13}~0~1~start~~^^0~{x-12}~{y-10}^^0~"
        f"#@$P~show~0~2~{x-15}~{y+10}~0~"
        f"gge{gid+5}^^{x-15}~{y+10}^^M {x-15} {y+10} h -20~#880000^^1~{x-21}~{y+15}~0~1~end~~^^0~{x-13}~{y+7}~0~1~start~~^^0~{x-12}~{y+10}^^0~"
    ); gid += 6

    # --- J4: 5V_LOGIC pin header ---
    x, y = 180, 270
    add_shape(
        f"LIB~{x}~{y}~package`CONN-2`nameAlias`HDR-2P`Value`5V_LOGIC`pre`J4`~~0~"
        f"gge{gid}#@$T~N~{x-40}~{y-40}~0~#000080~Arial~~~~~comment~5V_LOGIC (ASRPRO+PCA9685逻辑)~1~start~"
        f"gge{gid+1}#@$T~P~{x-40}~{y-50}~0~#000080~Arial~~~~~comment~J4~1~start~"
        f"gge{gid+2}#@$R~{x-15}~{y-20}~0~0~30~40~#800000~1~0~#FFFFFF~"
        f"gge{gid+3}#@$P~show~0~1~{x-15}~{y-10}~0~"
        f"gge{gid+4}^^{x-15}~{y-10}^^M {x-15} {y-10} h -20~#880000^^1~{x-21}~{y-5}~0~1~end~~^^0~{x-13}~{y-13}~0~1~start~~^^0~{x-12}~{y-10}^^0~"
        f"#@$P~show~0~2~{x-15}~{y+10}~0~"
        f"gge{gid+5}^^{x-15}~{y+10}^^M {x-15} {y+10} h -20~#880000^^1~{x-21}~{y+15}~0~1~end~~^^0~{x-13}~{y+7}~0~1~start~~^^0~{x-12}~{y+10}^^0~"
    ); gid += 6

    # --- C2: 100uF/16V ---
    x, y = 320, 320
    add_shape(
        f"LIB~{x}~{y}~package`CAP-DIP`nameAlias`D6.3-P2.5`Value`100uF/16V`pre`C2`~~0~"
        f"gge{gid}#@$T~N~{x-20}~{y-30}~0~#000080~Arial~~~~~comment~100uF/16V~1~start~"
        f"gge{gid+1}#@$T~P~{x-20}~{y-40}~0~#000080~Arial~~~~~comment~C2~1~start~"
        f"gge{gid+2}#@$PT~M {x-10},{y-15} A 5,15 0 0 1 {x-10},{y+15}~#880000~1~0~none~"
        f"gge{gid+3}#@$PL~{x+10} {y-15} {x+10} {y+15}~#880000~1~0~none~"
        f"gge{gid+4}#@$PL~{x+13} {y-18} {x+13} {y-22}~#0000FF~1~0~none~"
        f"gge{gid+5}#@$PL~{x+10} {y-20} {x+16} {y-20}~#0000FF~1~0~none~"
        f"gge{gid+6}#@$P~show~0~1~{x-15}~{y}~0~"
        f"gge{gid+7}^^{x-15}~{y}^^M {x-15} {y} h -20~#880000^^1~{x-21}~{y+5}~0~1~end~~^^0~{x-13}~{y-3}~0~1~start~~^^0~{x-12}~{y}^^0~"
        f"#@$P~show~0~2~{x+15}~{y}~0~"
        f"gge{gid+8}^^{x+15}~{y}^^M {x+15} {y} h 20~#880000^^1~{x+21}~{y+5}~0~1~start~~^^0~{x+13}~{y-3}~0~1~end~~^^0~{x+18}~{y}^^0~"
    ); gid += 9

    # --- U1: AMS1117-3.3V module ---
    x, y = 440, 320
    add_shape(
        f"LIB~{x}~{y}~package`HDR-2.54-3P-F`nameAlias`Model`Model`AMS1117-3.3V`pre`U1`~~0~"
        f"gge{gid}#@$T~N~{x-30}~{y-70}~0~#000080~Arial~~~~~comment~AMS1117-3.3V~1~start~"
        f"gge{gid+1}#@$T~P~{x-30}~{y-80}~0~#000080~Arial~~~~~comment~U1~1~start~"
        f"gge{gid+2}#@$R~{x-30}~{y-45}~0~0~60~50~#800000~1~0~#FFFFFF~"
        f"gge{gid+3}#@$T~N~{x-5}~{y-35}~0~#880000~Arial~~~~~comment~IN~1~start~"
        f"gge{gid+4}#@$T~N~{x-5}~{y-20}~0~#880000~Arial~~~~~comment~GND~1~start~"
        f"gge{gid+5}#@$T~N~{x-5}~{y-5}~0~#880000~Arial~~~~~comment~OUT~1~start~"
        f"gge{gid+6}#@$P~show~0~1~{x+15}~{y-35}~0~"
        f"gge{gid+7}^^{x+15}~{y-35}^^M {x+15} {y-35} h 20~#880000^^1~{x+21}~{y-30}~0~1~start~~^^0~{x+13}~{y-38}~0~1~end~~^^0~{x+18}~{y-35}^^0~"
        f"#@$P~show~0~2~{x+15}~{y-20}~0~"
        f"gge{gid+8}^^{x+15}~{y-20}^^M {x+15} {y-20} h 20~#880000^^1~{x+21}~{y-15}~0~1~start~~^^0~{x+13}~{y-23}~0~1~end~~^^0~{x+18}~{y-20}^^0~"
        f"#@$P~show~0~3~{x+15}~{y-5}~0~"
        f"gge{gid+9}^^{x+15}~{y-5}^^M {x+15} {y-5} h 20~#880000^^1~{x+21}~{y}~0~1~start~~^^0~{x+13}~{y-8}~0~1~end~~^^0~{x+18}~{y-5}^^0~"
    ); gid += 10

    # --- C3: 10uF/10V ---
    x, y = 560, 320
    add_shape(
        f"LIB~{x}~{y}~package`CAP-DIP`nameAlias`D5-P2`Value`10uF/10V`pre`C3`~~0~"
        f"gge{gid}#@$T~N~{x-20}~{y-30}~0~#000080~Arial~~~~~comment~10uF/10V~1~start~"
        f"gge{gid+1}#@$T~P~{x-20}~{y-40}~0~#000080~Arial~~~~~comment~C3~1~start~"
        f"gge{gid+2}#@$PT~M {x-10},{y-15} A 5,15 0 0 1 {x-10},{y+15}~#880000~1~0~none~"
        f"gge{gid+3}#@$PL~{x+10} {y-15} {x+10} {y+15}~#880000~1~0~none~"
        f"gge{gid+4}#@$PL~{x+13} {y-18} {x+13} {y-22}~#0000FF~1~0~none~"
        f"gge{gid+5}#@$PL~{x+10} {y-20} {x+16} {y-20}~#0000FF~1~0~none~"
        f"gge{gid+6}#@$P~show~0~1~{x-15}~{y}~0~"
        f"gge{gid+7}^^{x-15}~{y}^^M {x-15} {y} h -20~#880000^^1~{x-21}~{y+5}~0~1~end~~^^0~{x-13}~{y-3}~0~1~start~~^^0~{x-12}~{y}^^0~"
        f"#@$P~show~0~2~{x+15}~{y}~0~"
        f"gge{gid+8}^^{x+15}~{y}^^M {x+15} {y} h 20~#880000^^1~{x+21}~{y+5}~0~1~start~~^^0~{x+13}~{y-3}~0~1~end~~^^0~{x+18}~{y}^^0~"
    ); gid += 9

    # --- J3: 3.3V pin header ---
    x, y = 680, 320
    add_shape(
        f"LIB~{x}~{y}~package`CONN-2`nameAlias`HDR-2P`Value`3.3V`pre`J3`~~0~"
        f"gge{gid}#@$T~N~{x-40}~{y-40}~0~#000080~Arial~~~~~comment~3.3V (STM32 VDD)~1~start~"
        f"gge{gid+1}#@$T~P~{x-40}~{y-50}~0~#000080~Arial~~~~~comment~J3~1~start~"
        f"gge{gid+2}#@$R~{x-15}~{y-20}~0~0~30~40~#800000~1~0~#FFFFFF~"
        f"gge{gid+3}#@$P~show~0~1~{x-15}~{y-10}~0~"
        f"gge{gid+4}^^{x-15}~{y-10}^^M {x-15} {y-10} h -20~#880000^^1~{x-21}~{y-5}~0~1~end~~^^0~{x-13}~{y-13}~0~1~start~~^^0~{x-12}~{y-10}^^0~"
        f"#@$P~show~0~2~{x-15}~{y+10}~0~"
        f"gge{gid+5}^^{x-15}~{y+10}^^M {x-15} {y+10} h -20~#880000^^1~{x-21}~{y+15}~0~1~end~~^^0~{x-13}~{y+7}~0~1~start~~^^0~{x-12}~{y+10}^^0~"
    ); gid += 6

    # --- J6: 3.3V_SPARE pin header ---
    x, y = 810, 320
    add_shape(
        f"LIB~{x}~{y}~package`CONN-2`nameAlias`HDR-2P`Value`3.3V_SPARE`pre`J6`~~0~"
        f"gge{gid}#@$T~N~{x-40}~{y-40}~0~#000080~Arial~~~~~comment~3.3V_SPARE~1~start~"
        f"gge{gid+1}#@$T~P~{x-40}~{y-50}~0~#000080~Arial~~~~~comment~J6~1~start~"
        f"gge{gid+2}#@$R~{x-15}~{y-20}~0~0~30~40~#800000~1~0~#FFFFFF~"
        f"gge{gid+3}#@$P~show~0~1~{x-15}~{y-10}~0~"
        f"gge{gid+4}^^{x-15}~{y-10}^^M {x-15} {y-10} h -20~#880000^^1~{x-21}~{y-5}~0~1~end~~^^0~{x-13}~{y-13}~0~1~start~~^^0~{x-12}~{y-10}^^0~"
        f"#@$P~show~0~2~{x-15}~{y+10}~0~"
        f"gge{gid+5}^^{x-15}~{y+10}^^M {x-15} {y+10} h -20~#880000^^1~{x-21}~{y+15}~0~1~end~~^^0~{x-13}~{y+7}~0~1~start~~^^0~{x-12}~{y+10}^^0~"
    ); gid += 6

    # --- C4: 0.1uF MLCC (AMS1117 IN) ---
    x, y = 440, 270
    add_shape(
        f"LIB~{x}~{y}~package`CAP-0805`nameAlias`0805`Value`0.1uF`pre`C4`~~0~"
        f"gge{gid}#@$T~N~{x-20}~{y-30}~0~#000080~Arial~~~~~comment~0.1uF~1~start~"
        f"gge{gid+1}#@$T~P~{x-20}~{y-40}~0~#000080~Arial~~~~~comment~C4~1~start~"
        f"gge{gid+2}#@$PL~{x-8} {y-15} {x-8} {y+15}~#880000~1~0~none~"
        f"gge{gid+3}#@$PL~{x+8} {y-15} {x+8} {y+15}~#880000~1~0~none~"
        f"gge{gid+4}#@$P~show~0~1~{x-12}~{y}~0~"
        f"gge{gid+5}^^{x-12}~{y}^^M {x-12} {y} h -20~#880000^^1~{x-18}~{y+5}~0~1~end~~^^0~{x-10}~{y-3}~0~1~start~~^^0~{x-9}~{y}^^0~"
        f"#@$P~show~0~2~{x+12}~{y}~0~"
        f"gge{gid+6}^^{x+12}~{y}^^M {x+12} {y} h 20~#880000^^1~{x+18}~{y+5}~0~1~start~~^^0~{x+10}~{y-3}~0~1~end~~^^0~{x+15}~{y}^^0~"
    ); gid += 7

    # --- C5: 0.1uF MLCC (AMS1117 OUT) ---
    x, y = 540, 270
    add_shape(
        f"LIB~{x}~{y}~package`CAP-0805`nameAlias`0805`Value`0.1uF`pre`C5`~~0~"
        f"gge{gid}#@$T~N~{x-20}~{y-30}~0~#000080~Arial~~~~~comment~0.1uF~1~start~"
        f"gge{gid+1}#@$T~P~{x-20}~{y-40}~0~#000080~Arial~~~~~comment~C5~1~start~"
        f"gge{gid+2}#@$PL~{x-8} {y-15} {x-8} {y+15}~#880000~1~0~none~"
        f"gge{gid+3}#@$PL~{x+8} {y-15} {x+8} {y+15}~#880000~1~0~none~"
        f"gge{gid+4}#@$P~show~0~1~{x-12}~{y}~0~"
        f"gge{gid+5}^^{x-12}~{y}^^M {x-12} {y} h -20~#880000^^1~{x-18}~{y+5}~0~1~end~~^^0~{x-10}~{y-3}~0~1~start~~^^0~{x-9}~{y}^^0~"
        f"#@$P~show~0~2~{x+12}~{y}~0~"
        f"gge{gid+6}^^{x+12}~{y}^^M {x+12} {y} h 20~#880000^^1~{x+18}~{y+5}~0~1~start~~^^0~{x+10}~{y-3}~0~1~end~~^^0~{x+15}~{y}^^0~"
    ); gid += 7

    # ==============================
    # Wires
    # ==============================
    # Wire format: W~x1 y1 x2 y2 x3 y3...~color~width~0~none~id

    # 5V_RAW: J1(+) to branch point
    shapes.append(f"W~195 390 320 380~#008800~1~0~none~gge{gid}"); gid+=1
    # Branch point to C2(+)
    shapes.append(f"W~320 380 320 320 335 320~#008800~1~0~none~gge{gid}"); gid+=1
    # Branch point to L1
    shapes.append(f"W~320 380 350 380~#008800~1~0~none~gge{gid}"); gid+=1
    # L1 to C1(+) and J2(+)
    shapes.append(f"W~360 380 460 380~#008800~1~0~none~gge{gid}"); gid+=1
    shapes.append(f"W~460 380 615 390~#008800~1~0~none~gge{gid}"); gid+=1
    shapes.append(f"W~460 380 475 380~#008800~1~0~none~gge{gid}"); gid+=1
    # C1(+) to J2(+)
    shapes.append(f"W~615 390 755 390~#008800~1~0~none~gge{gid}"); gid+=1
    # 5V_RAW to U1 IN (via C2)
    shapes.append(f"W~320 380 320 320 425 320 425 285 455 285~#008800~1~0~none~gge{gid}"); gid+=1
    shapes.append(f"W~320 320 455 320~#008800~1~0~none~gge{gid}"); gid+=1
    # 5V_RAW to J4(+)
    shapes.append(f"W~195 390 195 280~#008800~1~0~none~gge{gid}"); gid+=1
    # U1 OUT to C3(+) and J3(+)
    shapes.append(f"W~455 315 560 315 560 320 575 320~#008800~1~0~none~gge{gid}"); gid+=1
    shapes.append(f"W~575 320 695 330~#008800~1~0~none~gge{gid}"); gid+=1
    # C3(+) to J3(+)
    shapes.append(f"W~695 330 825 330~#008800~1~0~none~gge{gid}"); gid+=1

    # GND connections (all to GND bus at y=480)
    gnd_conns = [
        (195, 390),   # J1(-)
        (475, 380),   # C1(-)
        (615, 400),   # J2(-)
        (755, 400),   # J5(-)
        (195, 280),   # J4(-)
        (335, 320),   # C2(-)
        (455, 300),   # U1 GND
        (575, 320),   # C3(-)
        (695, 340),   # J3(-)
        (825, 340),   # J6(-)
        (452, 270),   # C4(-)
        (552, 270),   # C5(-)
    ]
    for gx, gy in gnd_conns:
        shapes.append(f"W~{gx} {gy} {gx} 480~#008800~1~0~none~gge{gid}"); gid+=1

    # GND bus
    shapes.append(f"W~120 480 860 480~#000000~1~0~none~gge{gid}"); gid+=1

    # GND labels
    gnd_xs = [140, 260, 380, 500, 620, 740]
    for gx in gnd_xs:
        shapes.append(
            f"F~part_netLabel_gnD~{gx}~480~0~gge{gid}^^{gx}~480^^GND~#000080~{gx-11}~467~0~start~0~Times New Roman~9pt^^"
            f"PL~{gx} 490 {gx} 480~#000000~1~0~none~gge{gid+1}^^"
            f"PL~{gx-10} 490 {gx+9} 490~#000000~1~0~none~gge{gid+2}^^"
            f"PL~{gx-6} 492 {gx+7} 492~#000000~1~0~none~gge{gid+3}^^"
            f"PL~{gx-4} 494 {gx+3} 494~#000000~1~0~none~gge{gid+4}^^"
            f"PL~{gx-1} 496 {gx+1} 496~#000000~1~0~none~gge{gid+5}"
        ); gid += 6

    # Junction dots
    shapes.append(f"J~320~380~2.5~#CC0000~gge{gid}"); gid+=1
    shapes.append(f"J~460~380~2.5~#CC0000~gge{gid}"); gid+=1
    shapes.append(f"J~560~320~2.5~#CC0000~gge{gid}"); gid+=1

    # Net labels
    shapes.append(f"N~230~365~0~#000080~5V_RAW (10A)~gge{gid}~start~232~365~Verdana~7pt"); gid+=1
    shapes.append(f"N~420~365~0~#000080~5V_FILT (10A)~gge{gid}~start~422~365~Verdana~7pt"); gid+=1
    shapes.append(f"N~590~305~0~#000080~3.3V (~100mA)~gge{gid}~start~592~305~Verdana~7pt"); gid+=1

    return {
        "head": "1~1.11.3~~",
        "canvas": "CA~960~560~#FFFFFF~yes~#CCCCCC~10~960~560~line~10~pixel~5~400~300",
        "shape": shapes,
        "BBox": {"x": 120, "y": 260, "width": 740, "height": 260},
        "colors": {}
    }


# ============================================================
# PCB Generation
# ============================================================

def gen_pcb():
    shapes = []
    gid = 1

    BW = mm2u(BOARD_W)   # ~256
    BH = mm2u(BOARD_H)   # ~197
    M = mm2u(1.0)        # 4 unit margin

    T_S = mm2u(TRACK_SERVO)   # 5mm = 20 units
    T_L = mm2u(TRACK_LOGIC)   # 1mm = 4 units
    T_3 = mm2u(TRACK_3V3)     # 0.6mm = 2 units
    L1_LAYER = 1

    ps_kf = mm2u(5.08)      # KF128 pin spacing
    ps_hd = mm2u(2.54)      # 2.54mm header spacing

    # --- Row 1 (y=47): J1 → L1 → C1 → J2 → J5 (servo rail) ---
    j1_x = mm2u(5.0);  j1_y = mm2u(12.0)
    l1_x = mm2u(22.0); l1_y = mm2u(12.0)
    c1_x = mm2u(40.0); c1_y = mm2u(12.0)
    j2_x = mm2u(55.0); j2_y = mm2u(12.0)
    j5_x = mm2u(68.0); j5_y = mm2u(12.0)

    # J1: 5V_IN KF128-2P
    shapes.append(
        f"LIB~{j1_x}~{j1_y}~package`KF128-2P`pre`J1`~0~0~gge{gid}~2#"
        f"@$PAD~RECT~{j1_x-mm2u(2.0)}~{j1_y}~{mm2u(4.0)}~{mm2u(4.0)}~11~GND~1~2~~0~gge{gid+1}~~~Y#"
        f"@$PAD~RECT~{j1_x+ps_kf-mm2u(2.0)}~{j1_y}~{mm2u(4.0)}~{mm2u(4.0)}~11~5V_RAW~2~2~~0~gge{gid+2}~~~Y#"
        f"@$RECT~{j1_x-mm2u(4.0)}~{j1_y-mm2u(7.0)}~{mm2u(16.0)}~{mm2u(14.0)}~3~gge{gid+3}#"
        f"@$T~L~{j1_x-mm2u(4.0)}~{j1_y-mm2u(10.0)}~1~0~none~3~~3~5V_IN 10A~~gge{gid+4}"
    ); gid += 5

    # L1: 10uH/10A toroidal inductor
    shapes.append(
        f"LIB~{l1_x}~{l1_y}~package`IND-TOROID-16MM`pre`L1`~0~0~gge{gid}~2#"
        f"@$PAD~OVAL~{l1_x-mm2u(2.0)}~{l1_y}~{mm2u(4.0)}~{mm2u(4.0)}~11~5V_RAW~1~2~~0~gge{gid+1}~~~Y#"
        f"@$PAD~OVAL~{l1_x+mm2u(2.0)}~{l1_y}~{mm2u(4.0)}~{mm2u(4.0)}~11~5V_FILT~2~2~~0~gge{gid+2}~~~Y#"
        f"@$CIRCLE~{l1_x}~{l1_y}~{mm2u(9.0)}~1~3~gge{gid+3}#"
        f"@$T~L~{l1_x-mm2u(6.0)}~{l1_y-mm2u(11.0)}~1~0~none~3~~3~10uH/10A~~gge{gid+4}"
    ); gid += 5

    # C1: 470uF/16V
    shapes.append(
        f"LIB~{c1_x}~{c1_y}~package`CAP-DIP-10X16MM`pre`C1`~0~0~gge{gid}~2#"
        f"@$PAD~OVAL~{c1_x-mm2u(1.0)}~{c1_y}~{mm2u(3.0)}~{mm2u(3.0)}~11~5V_FILT~1~2~~0~gge{gid+1}~~~Y#"
        f"@$PAD~OVAL~{c1_x+mm2u(1.0)}~{c1_y}~{mm2u(3.0)}~{mm2u(3.0)}~11~GND~2~2~~0~gge{gid+2}~~~Y#"
        f"@$CIRCLE~{c1_x}~{c1_y}~{mm2u(6.0)}~1~3~gge{gid+3}#"
        f"@$T~L~{c1_x-mm2u(5.0)}~{c1_y-mm2u(10.0)}~1~0~none~3~~3~470uF/16V~~gge{gid+4}"
    ); gid += 5

    # J2: 5V_SERVO KF128-2P
    shapes.append(
        f"LIB~{j2_x}~{j2_y}~package`KF128-2P`pre`J2`~0~0~gge{gid}~2#"
        f"@$PAD~RECT~{j2_x-mm2u(2.0)}~{j2_y}~{mm2u(4.0)}~{mm2u(4.0)}~11~GND~1~2~~0~gge{gid+1}~~~Y#"
        f"@$PAD~RECT~{j2_x+ps_kf-mm2u(2.0)}~{j2_y}~{mm2u(4.0)}~{mm2u(4.0)}~11~5V_FILT~2~2~~0~gge{gid+2}~~~Y#"
        f"@$RECT~{j2_x-mm2u(4.0)}~{j2_y-mm2u(7.0)}~{mm2u(16.0)}~{mm2u(14.0)}~3~gge{gid+3}#"
        f"@$T~L~{j2_x-mm2u(4.0)}~{j2_y-mm2u(10.0)}~1~0~none~3~~3~5V_SERVO 10A~~gge{gid+4}"
    ); gid += 5

    # J5: 5V_SPARE pin header 2P
    shapes.append(
        f"LIB~{j5_x}~{j5_y}~package`HDR-2.54-2P`pre`J5`~0~0~gge{gid}~2#"
        f"@$PAD~OVAL~{j5_x}~{j5_y}~{mm2u(1.6)}~{mm2u(1.6)}~11~GND~1~2~~0~gge{gid+1}~~~Y#"
        f"@$PAD~OVAL~{j5_x+ps_hd}~{j5_y}~{mm2u(1.6)}~{mm2u(1.6)}~11~5V_FILT~2~2~~0~gge{gid+2}~~~Y#"
        f"@$RECT~{j5_x-mm2u(2.0)}~{j5_y-mm2u(4.0)}~{mm2u(8.0)}~{mm2u(8.0)}~3~gge{gid+3}#"
        f"@$T~L~{j5_x-mm2u(2.0)}~{j5_y-mm2u(7.0)}~1~0~none~3~~3~5V_SPARE~~gge{gid+4}"
    ); gid += 5

    # --- Row 2 (y~118): J4 → C2 → U1 → C3 → J3 → J6 (logic rail) ---
    j4_x = mm2u(5.0);  j4_y = mm2u(30.0)
    c2_x = mm2u(15.0); c2_y = mm2u(30.0)
    u1_x = mm2u(26.0); u1_y = mm2u(30.0)
    c3_x = mm2u(41.0); c3_y = mm2u(30.0)
    j3_x = mm2u(53.0); j3_y = mm2u(30.0)
    j6_x = mm2u(63.0); j6_y = mm2u(30.0)

    # J4: 5V_LOGIC
    shapes.append(
        f"LIB~{j4_x}~{j4_y}~package`HDR-2.54-2P`pre`J4`~0~0~gge{gid}~2#"
        f"@$PAD~OVAL~{j4_x}~{j4_y}~{mm2u(1.6)}~{mm2u(1.6)}~11~GND~1~2~~0~gge{gid+1}~~~Y#"
        f"@$PAD~OVAL~{j4_x+ps_hd}~{j4_y}~{mm2u(1.6)}~{mm2u(1.6)}~11~5V_RAW~2~2~~0~gge{gid+2}~~~Y#"
        f"@$RECT~{j4_x-mm2u(2.0)}~{j4_y-mm2u(4.0)}~{mm2u(8.0)}~{mm2u(8.0)}~3~gge{gid+3}#"
        f"@$T~L~{j4_x-mm2u(2.0)}~{j4_y-mm2u(7.0)}~1~0~none~3~~3~5V_LOGIC~~gge{gid+4}"
    ); gid += 5

    # C2: 100uF/16V
    shapes.append(
        f"LIB~{c2_x}~{c2_y}~package`CAP-DIP-6.3X11MM`pre`C2`~0~0~gge{gid}~2#"
        f"@$PAD~OVAL~{c2_x-mm2u(0.5)}~{c2_y}~{mm2u(2.5)}~{mm2u(2.5)}~11~5V_RAW~1~2~~0~gge{gid+1}~~~Y#"
        f"@$PAD~OVAL~{c2_x+mm2u(0.5)}~{c2_y}~{mm2u(2.5)}~{mm2u(2.5)}~11~GND~2~2~~0~gge{gid+2}~~~Y#"
        f"@$CIRCLE~{c2_x}~{c2_y}~{mm2u(4.5)}~1~3~gge{gid+3}#"
        f"@$T~L~{c2_x-mm2u(4.0)}~{c2_y-mm2u(8.0)}~1~0~none~3~~3~100uF/16V~~gge{gid+4}"
    ); gid += 5

    # U1: AMS1117-3.3V module (1x3P female header)
    shapes.append(
        f"LIB~{u1_x}~{u1_y}~package`HDR-2.54-3P-F`pre`U1`~0~0~gge{gid}~3#"
        f"@$PAD~OVAL~{u1_x}~{u1_y}~{mm2u(1.6)}~{mm2u(1.6)}~11~5V_RAW~1~2~~0~gge{gid+1}~~~Y#"
        f"@$PAD~OVAL~{u1_x+ps_hd}~{u1_y}~{mm2u(1.6)}~{mm2u(1.6)}~11~GND~2~2~~0~gge{gid+2}~~~Y#"
        f"@$PAD~OVAL~{u1_x+ps_hd*2}~{u1_y}~{mm2u(1.6)}~{mm2u(1.6)}~11~3.3V~3~2~~0~gge{gid+3}~~~Y#"
        f"@$RECT~{u1_x-mm2u(5.0)}~{u1_y-mm2u(8.0)}~{mm2u(22.0)}~{mm2u(16.0)}~3~gge{gid+4}#"
        f"@$T~L~{u1_x-mm2u(5.0)}~{u1_y-mm2u(11.0)}~1~0~none~3~~3~AMS1117-3.3V~~gge{gid+5}"
    ); gid += 6

    # C3: 10uF/10V
    shapes.append(
        f"LIB~{c3_x}~{c3_y}~package`CAP-DIP-6.3X11MM`pre`C3`~0~0~gge{gid}~2#"
        f"@$PAD~OVAL~{c3_x-mm2u(0.5)}~{c3_y}~{mm2u(2.5)}~{mm2u(2.5)}~11~3.3V~1~2~~0~gge{gid+1}~~~Y#"
        f"@$PAD~OVAL~{c3_x+mm2u(0.5)}~{c3_y}~{mm2u(2.5)}~{mm2u(2.5)}~11~GND~2~2~~0~gge{gid+2}~~~Y#"
        f"@$CIRCLE~{c3_x}~{c3_y}~{mm2u(4.0)}~1~3~gge{gid+3}#"
        f"@$T~L~{c3_x-mm2u(4.0)}~{c3_y-mm2u(8.0)}~1~0~none~3~~3~10uF/10V~~gge{gid+4}"
    ); gid += 5

    # J3: 3.3V
    shapes.append(
        f"LIB~{j3_x}~{j3_y}~package`HDR-2.54-2P`pre`J3`~0~0~gge{gid}~2#"
        f"@$PAD~OVAL~{j3_x}~{j3_y}~{mm2u(1.6)}~{mm2u(1.6)}~11~GND~1~2~~0~gge{gid+1}~~~Y#"
        f"@$PAD~OVAL~{j3_x+ps_hd}~{j3_y}~{mm2u(1.6)}~{mm2u(1.6)}~11~3.3V~2~2~~0~gge{gid+2}~~~Y#"
        f"@$RECT~{j3_x-mm2u(2.0)}~{j3_y-mm2u(4.0)}~{mm2u(8.0)}~{mm2u(8.0)}~3~gge{gid+3}#"
        f"@$T~L~{j3_x-mm2u(2.0)}~{j3_y-mm2u(7.0)}~1~0~none~3~~3~3.3V OUT~~gge{gid+4}"
    ); gid += 5

    # J6: 3.3V_SPARE
    shapes.append(
        f"LIB~{j6_x}~{j6_y}~package`HDR-2.54-2P`pre`J6`~0~0~gge{gid}~2#"
        f"@$PAD~OVAL~{j6_x}~{j6_y}~{mm2u(1.6)}~{mm2u(1.6)}~11~GND~1~2~~0~gge{gid+1}~~~Y#"
        f"@$PAD~OVAL~{j6_x+ps_hd}~{j6_y}~{mm2u(1.6)}~{mm2u(1.6)}~11~3.3V~2~2~~0~gge{gid+2}~~~Y#"
        f"@$RECT~{j6_x-mm2u(2.0)}~{j6_y-mm2u(4.0)}~{mm2u(8.0)}~{mm2u(8.0)}~3~gge{gid+3}#"
        f"@$T~L~{j6_x-mm2u(2.0)}~{j6_y-mm2u(7.0)}~1~0~none~3~~3~3.3V SPARE~~gge{gid+4}"
    ); gid += 5

    # C4: 0.1uF MLCC (near U1 IN)
    c4_x = mm2u(24.0); c4_y = mm2u(22.0)
    shapes.append(
        f"LIB~{c4_x}~{c4_y}~package`CAP-0805`pre`C4`~0~0~gge{gid}~2#"
        f"@$PAD~RECT~{c4_x-mm2u(0.5)}~{c4_y}~{mm2u(1.3)}~{mm2u(1.6)}~1~5V_RAW~1~0~~0~gge{gid+1}~~~Y#"
        f"@$PAD~RECT~{c4_x+mm2u(0.5)}~{c4_y}~{mm2u(1.3)}~{mm2u(1.6)}~1~GND~2~0~~0~gge{gid+2}~~~Y#"
        f"@$RECT~{c4_x-mm2u(2.0)}~{c4_y-mm2u(1.0)}~{mm2u(4.0)}~{mm2u(2.0)}~3~gge{gid+3}#"
        f"@$T~L~{c4_x-mm2u(2.0)}~{c4_y+mm2u(2.5)}~1~0~none~3~~3~0.1uF~~gge{gid+4}"
    ); gid += 5

    # C5: 0.1uF MLCC (near U1 OUT)
    c5_x = mm2u(38.0); c5_y = mm2u(22.0)
    shapes.append(
        f"LIB~{c5_x}~{c5_y}~package`CAP-0805`pre`C5`~0~0~gge{gid}~2#"
        f"@$PAD~RECT~{c5_x-mm2u(0.5)}~{c5_y}~{mm2u(1.3)}~{mm2u(1.6)}~1~3.3V~1~0~~0~gge{gid+1}~~~Y#"
        f"@$PAD~RECT~{c5_x+mm2u(0.5)}~{c5_y}~{mm2u(1.3)}~{mm2u(1.6)}~1~GND~2~0~~0~gge{gid+2}~~~Y#"
        f"@$RECT~{c5_x-mm2u(2.0)}~{c5_y-mm2u(1.0)}~{mm2u(4.0)}~{mm2u(2.0)}~3~gge{gid+3}#"
        f"@$T~L~{c5_x-mm2u(2.0)}~{c5_y+mm2u(2.5)}~1~0~none~3~~3~0.1uF~~gge{gid+4}"
    ); gid += 5

    # ==============================
    # Traces (Top Layer)
    # ==============================
    # 5V_RAW: J1(+) → L1.in
    shapes.append(
        f"TRACK~{T_S}~{L1_LAYER}~5V_RAW~"
        f"{j1_x+ps_kf} {j1_y} "
        f"{l1_x} {l1_y}~gge{gid}"
    ); gid += 1

    # 5V_RAW: J1(+) → J4(+)
    shapes.append(
        f"TRACK~{T_L}~{L1_LAYER}~5V_RAW~"
        f"{j1_x+ps_kf} {j1_y} "
        f"{j1_x+ps_kf} {j4_y+ps_hd} "
        f"{j4_x+ps_hd} {j4_y+ps_hd}~gge{gid}"
    ); gid += 1

    # 5V_RAW: → C2(+) → U1.IN
    mid_y = mm2u(24.0)
    shapes.append(
        f"TRACK~{T_L}~{L1_LAYER}~5V_RAW~"
        f"{j1_x+ps_kf} {j1_y} "
        f"{j1_x+ps_kf} {mid_y} "
        f"{c2_x} {mid_y} "
        f"{c2_x} {c2_y}~gge{gid}"
    ); gid += 1
    shapes.append(
        f"TRACK~{T_L}~{L1_LAYER}~5V_RAW~"
        f"{c2_x} {c2_y} "
        f"{u1_x} {c2_y} "
        f"{u1_x} {u1_y}~gge{gid}"
    ); gid += 1
    # U1.IN → C4
    shapes.append(
        f"TRACK~{T_L}~{L1_LAYER}~5V_RAW~"
        f"{u1_x} {u1_y} "
        f"{u1_x} {c4_y} "
        f"{c4_x} {c4_y}~gge{gid}"
    ); gid += 1

    # 5V_FILT: L1.out → C1(+) → J2(+) → J5(+)
    l1_out_x = l1_x + mm2u(2.0)
    shapes.append(
        f"TRACK~{T_S}~{L1_LAYER}~5V_FILT~"
        f"{l1_out_x} {l1_y} "
        f"{c1_x} {c1_y} "
        f"{j2_x+ps_kf} {j2_y} "
        f"{j5_x+ps_hd} {j5_y}~gge{gid}"
    ); gid += 1

    # 3.3V: U1.OUT → C3(+) → J3(+) → J6(+)
    u1_out_x = u1_x + ps_hd * 2
    shapes.append(
        f"TRACK~{T_3}~{L1_LAYER}~3.3V~"
        f"{u1_out_x} {u1_y} "
        f"{c3_x} {u1_y} "
        f"{c3_x} {c3_y}~gge{gid}"
    ); gid += 1
    shapes.append(
        f"TRACK~{T_3}~{L1_LAYER}~3.3V~"
        f"{c3_x} {c3_y} "
        f"{j3_x+ps_hd} {c3_y} "
        f"{j3_x+ps_hd} {j3_y} "
        f"{j6_x+ps_hd} {j6_y}~gge{gid}"
    ); gid += 1
    # → C5
    shapes.append(
        f"TRACK~{T_3}~{L1_LAYER}~3.3V~"
        f"{u1_out_x} {u1_y} "
        f"{c5_x} {u1_y} "
        f"{c5_x} {c5_y}~gge{gid}"
    ); gid += 1

    # --- Board Outline ---
    shapes.append(
        f"TRACK~1~10~~"
        f"{M} {M} "
        f"{M+BW} {M} "
        f"{M+BW} {M+BH} "
        f"{M} {M+BH} "
        f"{M} {M}~gge{gid}"
    ); gid += 1

    # --- Bottom Layer GND Copper Pour ---
    shapes.append(
        f"COPPERAREA~1~2~GND~"
        f"{M} {M} "
        f"{M+BW} {M} "
        f"{M+BW} {M+BH} "
        f"{M} {M+BH}~"
        f"2~solid~gge{gid}~spoke~yes~[]"
    ); gid += 1

    # --- Silkscreen ---
    silk_texts = [
        (mm2u(3), mm2u(3), "PWR v3.0 30xSG90 10A 2oz"),
        (mm2u(28), mm2u(46), "STAR GND - Bottom Cu Pour"),
    ]
    for tx, ty, txt in silk_texts:
        shapes.append(f"TEXT~L~{tx}~{ty}~1~0~none~3~~3~{txt}~~gge{gid}")
        gid += 1

    return {
        "head": "3~1.11.3~~",
        "canvas": f"CA~{BW+20}~{BH+20}~#000000~yes~#FFFFFF~10~{BW//2}~{BH//2}~line~1~mil~1~45~visible~0.5~6~6",
        "systemColor": "#000000~#FFFFFF~#FFFFFF~#000000~#FFFFFF",
        "layers": [
            "1~TopLayer~#FF0000~true~true~true",
            "2~BottomLayer~#0000FF~true~false~true",
            "3~TopSilkLayer~#FFCC00~true~false~true",
            "10~BoardOutline~#FF00FF~true~false~true",
            "11~Multi-Layer~#C0C0C0~true~false~true"
        ],
        "DRCRULE": {
            "trackWidth": mm2u(0.3),
            "track2Track": mm2u(0.3),
            "pad2Pad": mm2u(0.3),
            "track2Pad": mm2u(0.3),
            "hole2Hole": mm2u(0.5),
            "holeSize": mm2u(0.5)
        },
        "shape": shapes,
        "BBox": {"x": 0, "y": 0, "width": BW + M * 2, "height": BH + M * 2},
        "preference": {"hideNets": "", "hideFootprints": ""},
        "netColors": {}
    }


# ============================================================
# Main
# ============================================================

def main():
    print("=== Mechanical Clock Power Board v3.0 Generator ===\n")

    # Schematic
    sch = gen_schematic()
    sch_path = os.path.join(OUT_DIR, "机械时钟电源板_v3_原理图.json")
    with open(sch_path, 'w', encoding='utf-8') as f:
        json.dump(sch, f, ensure_ascii=False, indent=2)
    print(f"[OK] Schematic: {sch_path}")

    # PCB
    pcb = gen_pcb()
    pcb_path = os.path.join(OUT_DIR, "机械时钟电源板_v3_PCB.json")
    with open(pcb_path, 'w', encoding='utf-8') as f:
        json.dump(pcb, f, ensure_ascii=False, indent=2)
    print(f"[OK] PCB: {pcb_path}")

    # ZIP
    zip_path = os.path.join(OUT_DIR, "机械时钟电源板_v3_完整工程.zip")
    with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zf:
        zf.writestr("机械时钟电源板_v3_原理图.json",
                     json.dumps(sch, ensure_ascii=False, indent=2))
        zf.writestr("机械时钟电源板_v3_PCB.json",
                     json.dumps(pcb, ensure_ascii=False, indent=2))
    print(f"[OK] Project ZIP: {zip_path}")

    # Summary
    print(f"""
=== Design Summary ===
Board:      {BOARD_W} x {BOARD_H} mm
Copper:     {COPPER_OZ}oz
Servo track:{TRACK_SERVO}mm (>=12A @ 2oz)
Logic track:{TRACK_LOGIC}mm
3.3V track: {TRACK_3V3}mm
Input:      LRS-50-5 (5V/10A)
Outputs:    3x 5V (1x screw + 2x header), 2x 3.3V (header), 4x GND

=== Nets ===
5V_RAW:   J1(+) -> L1.in, J4(+), C2(+), U1.IN
5V_FILT:  L1.out -> C1(+), J2(+), J5(+)
3.3V:     U1.OUT -> C3(+), J3(+), J6(+)
GND:      All -> Bottom Cu Pour (Star Ground)

=== LCEDA Pro Import ===
File -> Import -> EasyEDA Standard ->
  {zip_path}
Check footprint mapping -> DRC -> Export Gerber -> JLC Order
""")

    return sch_path, pcb_path, zip_path


if __name__ == "__main__":
    main()
