import logging

logging.basicConfig(level=logging.INFO)
log = logging.getLogger("test")
log.debug("Test message: %d(%s)", 100, "foobar")
log.info("Test message2: %d(%s)", 100, "foobar")
log.warning("Test message3: %d(%s)")
log.error("Test message4")
log.critical("Test message5")
logging.info("Test message6")

try:
    1 / 0
except:
    log.exception("Some trouble (%s)", "expected")


class MyHandler(logging.Handler):
    def emit(self, record):
        print("levelname=%(levelname)s name=%(name)s message=%(message)s" % record.__dict__)


logging.getLogger().addHandler(MyHandler())
logging.info("Test message7")
