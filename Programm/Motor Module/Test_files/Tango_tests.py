
# =====================================================================
# Example how to use Tango DLL in conjunction with Python version 3.7.3
# =====================================================================

import sys
# import ctypes (used to call DLL functions)
from ctypes import *
dll_path = r"C:\Users\Transfersystem User\Desktop\Repos\BachelorThesis\DLL Files\TangoDLL_64bit_V1399\Tango_DLL.dll"
# give location of dll (current directory)
m_Tango = cdll.LoadLibrary(dll_path)

if m_Tango == 0:
    print("Error: failed to load DLL")
    sys.exit(0)

# Tango_DLL.dll loaded successfully

if m_Tango.LSX_CreateLSID == 0:
    print("unexpected error. required DLL function CreateLSID() missing")
    sys.exit(0)
# continue only if required function exists

LSID = c_int()
error = int  # value is either DLL or Tango error number if not zero
error = m_Tango.LSX_CreateLSID(byref(LSID))
if error > 0:
    print("Error: " + str(error))
    sys.exit(0)

# OK: got communication ID from DLL (usually 1. may vary with multiple connections)
# keep this LSID in mind during the whole session

if m_Tango.LSX_ConnectSimple == 0:
    print("unexepcted error. required DLL function ConnectSimple() missing")
    sys.exit(0)
# continue only if required function exists

# error = m_Tango.LSX_ConnectSimple(LSID,2,"COM20",57600,0)
# following combination of -1,"" works only for USB but not for RS232 connections
error = m_Tango.LSX_ConnectSimple(LSID, -1, "", 57600, 0)
if error > 0:
    print("Error: LSX_ConnectSimple " + str(error))
    sys.exit(0)

print("TANGO is now successfully connected to DLL")


# some c-type variables (general purpose usage)
dx = c_double()
dy = c_double()
dz = c_double()
da = c_double()
dimX = c_int()
dimY = c_int()
dimZ = c_int()
dimA = c_int()

# query actual position (4 axes) (unit depends on GetDimensions)
error = m_Tango.LSX_GetPos(LSID, byref(dx), byref(dy), byref(dz), byref(da))
if error > 0:
    print("Error: GetPos " + str(error))
else:
    print("position = " + str(dx.value) + " " + str(dy.value) +
          " " + str(dz.value) + " " + str(da.value))


# Get the Dimensions
error = m_Tango.LSX_GetDimensions(LSID, byref(
    dimX), byref(dimY), byref(dimZ), byref(dimA))
if error > 0:
    print("Error: getDim " + str(error))
else:
    print("LSX_GetDimensions = " + str(dimX.value) + " " + str(dimY.value) +
          " " + str(dimZ.value) + " " + str(dimA.value))

# query the Limits
for i in range(1, 5):
    error = m_Tango.LSX_GetLimit(LSID, i, byref(dx), byref(dy))
    if error > 0:
        print("Error: getLimit " + str(error))
    else:
        print(dx.value, dy.value)

moveX = c_double(100)
moveY = c_double(100)
moveZ = c_double(0.0)
moveA = c_double(0.0)
vocal = c_bool(True)


error = m_Tango.LSX_MoveAbs(LSID, moveX, moveY, moveZ, moveA, vocal)
if error > 0:
    print("Error: getDim " + str(error))
else:
    print("Moving")

# query actual position (4 axes) (unit depends on GetDimensions)
error = m_Tango.LSX_GetPos(LSID, byref(dx), byref(dy), byref(dz), byref(da))
if error > 0:
    print("Error: GetPos " + str(error))
else:
    print("position = " + str(dx.value) + " " + str(dy.value) +
          " " + str(dz.value) + " " + str(da.value))
