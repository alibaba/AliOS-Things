c:\ffmpeg.exe -i img%%d.JPEG -f image2 final%%d.png
c:\ffmpeg.exe -framerate 30 -i final%%d.png -vf fps=30 -pix_fmt yuv420p output.mp4
del /s *.png