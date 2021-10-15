import platform

if platform.system() == "Windows":
    import os
    pkg_dir = os.path.dirname(os.path.realpath(__file__))
    os.environ.setdefault("PATH","")
    os.environ["PATH"] = pkg_dir + os.pathsep + os.environ["PATH"]
