
import OSS

print("-------------------Welcome OSS MicroPython--------------------")

print("-----OSS demo start-----")
OSS_ACCESS_KEY = "xxxx"
OSS_ACCESS_SECRET = "xxxx"
OSS_ENDPOINT = "xxxx"
OSS_BUCKET = "xxxx"
File_PATH = "/data/python-apps/oss/res/test.jpg"
File_CONTENT = "oss test content"

urlFile = OSS.uploadFile(OSS_ACCESS_KEY, OSS_ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET, File_PATH)
print("File url:")
print(urlFile)
urlContent = OSS.uploadContent(OSS_ACCESS_KEY, OSS_ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET, File_CONTENT)
print("Content url:")
print(urlContent)

print("-----OSS demo end-----")
