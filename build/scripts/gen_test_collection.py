import sys, os, glob

if len(sys.argv) == 3:
   file_dir = os.path.join(os.getcwd(), sys.argv[1])
   if not os.path.exists(file_dir):
       os.makedirs(file_dir)
   else:
       if os.path.exists("aos.mk"):
            os.remove("aos.mk")

   for f in os.listdir(sys.argv[1]):
       if "test_collection" in f:
            os.remove(sys.argv[1] + "/" + f)

   file_obj = open(sys.argv[2], "w")
   file_obj.write("")
   file_obj.close()
