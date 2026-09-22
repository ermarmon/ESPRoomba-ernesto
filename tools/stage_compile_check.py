"""Stage a NON-DEPLOYABLE compile check; never reads real secrets or uploads."""
from pathlib import Path
import argparse, re, shutil, subprocess, tempfile, os
p=argparse.ArgumentParser();p.add_argument('--legacy',action='store_true');a=p.parse_args()
root=Path(__file__).resolve().parents[1]
dest=root/'build'/('legacy-check' if a.legacy else 'modern-check')
dest.mkdir(parents=True,exist_ok=True)
if a.legacy:
    config=subprocess.check_output(['git','show','roomba-working-2025.2.2:roomba.yaml'],cwd=root).decode('utf-8')
    header=subprocess.check_output(['git','show','roomba-working-2025.2.2:custom_components/Roomba.h'],cwd=root)
    (dest/'custom_components').mkdir(exist_ok=True)
    (dest/'custom_components'/'Roomba.h').write_bytes(header)
else:
    config=(root/'roomba.yaml').read_text(encoding='utf-8-sig')
    shutil.copytree(root/'custom_components',dest/'custom_components',dirs_exist_ok=True)
    shutil.copytree(root/'components',dest/'components',dirs_exist_ok=True)
# Dummy credentials belong ONLY to staging; they do not connect to the home network.
config=re.sub(r'(?m)^(\s*)key: !secret .+$',r'\1key: "AQIDBAUGBwgJCgsMDQ4PEBESExQVFhcYGRobHB0eHyA="',config)
config=re.sub(r'(?m)^(\s*)password: !secret .+$',r'\1password: "compile-check-only"',config)
config=re.sub(r'(?m)^(\s*)ssid: !secret .+$',r'\1ssid: "compile-check-only"',config)
if os.name=='nt':
    build_path=Path(tempfile.gettempdir())/('roomba-check-2025' if a.legacy else 'roomba-check-2026')
    config=config.replace('esphome:\n','esphome:\n  build_path: '+build_path.as_posix()+'\n',1)
(dest/'roomba.yaml').write_text(config,encoding='utf-8')
print(dest/'roomba.yaml')
