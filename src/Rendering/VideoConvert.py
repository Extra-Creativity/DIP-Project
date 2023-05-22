import cv2
from subprocess import Popen, PIPE
import os
import numpy as np

dir_path = os.path.dirname(os.path.realpath(__file__))
buffer_path = os.path.join(dir_path, "config", "DataTransfer")
video_path = os.path.join(dir_path, "data", "video")
pose_path = os.path.join(dir_path, "data", "poses")
print(dir_path, video_path, pose_path, sep = "\n")

poses = []

converter = Popen("build\\windows\\x64\\release\\RestoreFromVideo.exe", 
                  stdin=PIPE, stdout=PIPE, stderr=PIPE, encoding="UTF8")

pic_path = os.path.join(buffer_path, "buffer.png")
converter.stdin.write(pic_path + "\n")
converter.stdin.flush()  # important

response = converter.stdout.readline()

def GetVideoWriter(video):
    width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    fps = video.get(cv2.CAP_PROP_FPS)
    print(f"video width = {width}, height = {height}, fps = {fps}")
    
    fourcc = cv2.VideoWriter_fourcc(*"mp4v")
    videoWriter = cv2.VideoWriter(os.path.join(video_path, "converted.mp4"),
                                  fourcc, fps, (width, height))
    converter.stdin.write(f"{width} {height}\n")
    converter.stdin.flush()
    converter.stdout.readline()
    return videoWriter, width, height

i = 0
video = cv2.VideoCapture(os.path.join(video_path, "result.mp4"))
assert video.isOpened()

videoWriter, width, height = GetVideoWriter(video)
poses = np.loadtxt(os.path.join(pose_path, "poses1.txt")).reshape(-1, 9)

while video.isOpened():
    ret, frame = video.read()
    if ret:
        cv2.imwrite(pic_path, frame)
        pose = poses[i]
        for k in range(len(pose)):
            converter.stdin.write(f"{pose[k]} ")
        converter.stdin.write("\n")
        converter.stdin.flush()
        converter.stdout.readline()
        convertedFrame = cv2.imread(pic_path)
        videoWriter.write(convertedFrame)
        i += 1
    else:
        break

video.release()
videoWriter.release()
converter.stdin.write("Done.\n")
converter.stdin.flush()