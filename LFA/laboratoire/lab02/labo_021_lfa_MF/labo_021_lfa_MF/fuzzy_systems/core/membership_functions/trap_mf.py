import numpy as np
from fuzzy_systems.core.membership_functions.lin_piece_wise_mf import LinPWMF

class TrapMF(LinPWMF):
    def __init__(self, p0, p1, p2, p3):
        super(TrapMF, self).__init__([0,0],[p0,0],[p1,1],[p2,1],[p3,0],[p3+p0,0])
