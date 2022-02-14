import ctypes
import os

CUR_PATH = os.path.dirname(__file__)
dllPath = os.path.join(CUR_PATH, "backend.dll")
#print(dllPath)
pDll = ctypes.WinDLL(dllPath)
#print(pDll)

pResutl = pDll.sum(1, 4)
pResult2 = pDll.sub(1, 4)
print(pResutl)
print(pResult2)