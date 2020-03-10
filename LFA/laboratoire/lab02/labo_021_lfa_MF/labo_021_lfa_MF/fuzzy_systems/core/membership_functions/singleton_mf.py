import numpy as np

from fuzzy_systems.core.membership_functions.free_shape_mf import FreeShapeMF

class SingletonMF(FreeShapeMF):
    def __init__(self,y):
        self.y = y ; 
        in_values = [0,y,y,y,2*y]
        mf_values = [0,0,1,0,0]
        super().__init__(in_values, mf_values) 
    def fuzzify(self):
        return self.y; 
