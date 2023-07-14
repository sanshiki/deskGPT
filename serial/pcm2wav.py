#pcm转wav程序

from moudule import fileIO

pcmFile = fileIO.pcmFileManager("test.pcm")
wavFile = fileIO.wavFileManager("test.wav")

pcmData = pcmFile.read()

header = wavFile.getWavHeader(8000, 16, 1)

wavFile.write(header + pcmData)
