
import sys
from ctypes import*
dll_path = r"C:\Users\Transfersystem User\Desktop\Repos\BachelorThesis\DLL Files\TangoDLL_64bit_V1399\Tango_DLL.dll"
# give location of dll (current directory)


class TangoController():
    def __init__(self, dll_path):
        self.dll_path = dll_path
        self.m_Tango = cdll.LoadLibrary(self.dll_path)
        self.max_x = 100
        self.max_y = 100

        if self.m_Tango == 0:
            print("Error: failed to load DLL")
            sys.exit(0)

        # Tango_DLL.dll loaded successfully

        if self.m_Tango.LSX_CreateLSID == 0:
            print("unexpected error. required DLL function CreateLSID() missing")
            sys.exit(0)
        # continue only if required function exists

        self.LSID = c_int()
        error = int  # value is either DLL or Tango error number if not zero
        error = self.m_Tango.LSX_CreateLSID(byref(self.LSID))
        if error > 0:
            print("Error: " + str(error))
            sys.exit(0)

        # OK: got communication ID from DLL (usually 1. may vary with multiple connections)
        # keep this LSID in mind during the whole session

        if self.m_Tango.LSX_ConnectSimple == 0:
            print("unexepcted error. required DLL function ConnectSimple() missing")
            sys.exit(0)
        # continue only if required function exists

        # error = self.m_Tango.LSX_ConnectSimple(self.LSID, 2, "COM3", 57600, 0)
        # following combination of -1,"" works only for USB but not for RS232 connections
        error = self.m_Tango.LSX_ConnectSimple(self.LSID, -1, "", 57600, 0)
        if error > 0:
            print("Error: LSX_ConnectSimple " + str(error))
            sys.exit(0)

        print("TANGO is now successfully connected to DLL")

        self.full_calibrate()

    def _calibrate(self):
        # calibrate all axes
        error = self.m_Tango.LSX_Calibrate(self.LSID)
        if error > 0:
            print("Error: Calibrate " + str(error))

    def _range_measure(self):
        error = self.m_Tango.LSX_RMeasure(self.LSID)
        if error > 0:
            print("Error: RM " + str(error))

    def check_limit(self, x, y):
        """
        Checks if the X and Y Coords are within the Software Endstops\n
        returns clipped x and y
        """
        if (x > self.max_x):
            x = self.max_x
            print("X bigger than Secure Stop, setting X to Max_x")
        if (y > self.max_y):
            y = self.max_y
            print("Y bigger than Secure Stop, setting Y to Max_y")
        return x, y

    def full_calibrate(self):
        """
        Calibrates the plant and sets the Software endstops
        """
        # calibrate all axes
        self._calibrate()
        self._range_measure()
        self.maxX, self.maxY = self.get_pos()
        self._calibrate()

    def get_pos(self):
        """
        Returns the Current Position
        """
        # query actual position (4 axes) (unit depends on GetDimensions)
        dx = c_double()
        dy = c_double()
        dz = c_double()
        da = c_double()
        error = self.m_Tango.LSX_GetPos(
            self.LSID, byref(dx), byref(dy), byref(dz), byref(da))
        if error > 0:
            print("Error: GetPos " + str(error))

        return (dx.value, dy.value)

    def get_max_xy(self):
        return self.max_x, self.max_y

    def abs_move(self, x, y):
        """
        moves to an absolute position, checks for max_y and max_y\n
        """
        x, y = self.check_limit(x, y)
        moveX = c_double(x)
        moveY = c_double(y)
        moveZ = c_double(0.0)
        moveA = c_double(0.0)
        vocal = c_bool(True)

        error = self.m_Tango.LSX_MoveAbs(
            self.LSID, moveX, moveY, moveZ, moveA, vocal)
        if error > 0:
            print("Error: abs_move " + str(error))
            sys.exit()
        else:
            print(f"Moved to {x}, {y} (Absolut)")

    def can_move(self, dx, dy):
        """
        Checks if it can move by dx and dy\n
        returns False if not possible\n
        return True if possible
        """
        curr_x, curr_y = self.get_pos()
        if (curr_x + dx > self.max_x):
            return False
        if (curr_y + dy > self.max_y):
            return False
        return True

    def rel_move(self, dx, dy):
        """
        moves relative to the Current position, checks for max_x and max_y\n
        returns False if the move was unsuccesful\n
        return True if successful
        """
        if not self.can_move(dx, dy):
            return False

        move_dx = c_double(dx)
        move_dy = c_double(dy)
        move_dz = c_double(0.0)
        move_da = c_double(0.0)
        vocal = c_bool(True)

        error = self.m_Tango.LSX_MoveRel(
            self.LSID, move_dx, move_dy, move_dz, move_da, vocal)
        if error > 0:
            print("Error: rel_move " + str(error))
            sys.exit()
        else:
            print(f"Moved by {dx}, {dy} (Rel)")
            return True


if __name__ == "__main__":
    tc = TangoController(dll_path)
    steps = 50
    can_move_x = tc.can_move(steps, 0)
    can_move_y = tc.can_move(0, steps)
    while(can_move_x):
        while(can_move_y):
            can_move_y = tc.rel_move(0, steps)
        curr_x, curr_y = tc.get_pos()
        tc.abs_move(curr_x, 0)
        can_move_x = tc.rel_move(steps, 0)
        can_move_y = True
