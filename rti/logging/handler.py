import rti.logging.distlog as distlog
import logging


class DistlogHandler(logging.Handler):
    _count = 0
    _levelMap = {
        logging.CRITICAL: distlog.LogLevel.SEVERE,
        logging.ERROR:    distlog.LogLevel.ERROR,
        logging.WARNING:  distlog.LogLevel.WARNING,
        logging.INFO:     distlog.LogLevel.INFO,
        logging.DEBUG:    distlog.LogLevel.DEBUG
    }

    def __init__(self, options=None):
        logging.Handler.__init__(self)
        DistlogHandler._count += 1

        if options is not None:
            distlog.Logger.options(options)

        self._instance = distlog.Logger.instance

    def close(self):
        self.acquire()
        if self._instance is None:
            raise RuntimeError('Attempted multiple closure of DistlogHandler')
        self._instance = None
        DistlogHandler._count -= 1
        if DistlogHandler._count == 0:
            distlog.Logger.finalize()
        self.release()
        logging.Handler.close(self)

    def emit(self, record):
        level = DistlogHandler._levelMap[record.levelno]
        try:
            self._instance.log(level, record.getMessage(), record.category)
        except:
            self._instance.log(level, record.getMessage())



