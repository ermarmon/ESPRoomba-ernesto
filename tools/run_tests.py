"""Run host regression tests using CXX or the workspace Zig compiler."""
from pathlib import Path
import os, shutil, subprocess
root=Path(__file__).resolve().parents[1]
zig=root/'build/venv-2026/Lib/site-packages/ziglang/zig.exe'
compiler=os.environ.get('CXX') or shutil.which('g++') or shutil.which('clang++')
cmd=[compiler] if compiler else [str(zig),'c++']
if not compiler and not zig.exists(): raise SystemExit('Set CXX to a C++17 compiler, or install ziglang in build/venv-2026.')
(root/'build').mkdir(exist_ok=True)
exe=root/'build'/('test_roomba.exe' if os.name=='nt' else 'test_roomba')
with (root/'build/host-tests.log').open('w',encoding='utf-8') as log:
    subprocess.run(cmd+['-std=c++17','-Itests/stubs','tests/test_roomba.cpp','-o',str(exe)],cwd=root,stdout=log,stderr=subprocess.STDOUT,check=True)
subprocess.run([str(exe)],cwd=root,check=True)
