from .. import Tango_model

dll_path = r"C:\Users\Transfersystem User\Desktop\Repos\BachelorThesis\DLL Files\TangoDLL_64bit_V1399\Tango_DLL.dll"

tc = Tango_model.TangoController(dll_path)
# Raster move
steps = 50
tc.can_move_x = tc.can_move(steps, 0)
tc.can_move_y = tc.can_move(0, steps)
while(tc.can_move_x):
    while(tc.can_move_y):
        tc.can_move_y = tc.rel_move(0, steps)
    curr_x, curr_y = tc.get_pos()
    tc.abs_move(curr_x, 0)
    tc.can_move_x = tc.rel_move(steps, 0)
    tc.can_move_y = tc.can_move(0, steps)
