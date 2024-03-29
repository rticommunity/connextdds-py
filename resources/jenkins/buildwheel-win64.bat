python -m venv .venv

CALL .venv\Scripts\activate.bat
python -m pip install cibuildwheel pybind11==2.9.0

python -m cibuildwheel --platform windows --archs auto64 --config-file resources\jenkins\config.toml

exit %ERRORLEVEL%