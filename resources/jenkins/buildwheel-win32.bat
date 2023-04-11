python -m venv .venv

CALL .venv\Scripts\activate.bat
python -m pip install cibuildwheel pybind11==2.9.0

python -m cibuildwheel --platform windows --archs auto32 --config-file resources\jenkins\config-win32.toml

exit %ERRORLEVEL%