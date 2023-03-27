# TODO

#from pyaudio import PyAudio, paInt16
#import numpy as np
#import time
#
## define the sampling rate
#SAMPLING_RATE = 8000
## define the sampling time
#NUM_SAMPLES = 200
## define the frequency of the first tone
#FREQ1 = 900
## define the frequency of the second tone
#FREQ2 = 1100
##define the volume
#VOLUME = 500
#
## define the function to play the audio
#def play_tone(stream):
#    # generate the sine wave
#    sine_wave1 = [np.sin(2 * np.pi * FREQ1 * x / SAMPLING_RATE) for x in range(NUM_SAMPLES)]
#    sine_wave2 = [np.sin(2 * np.pi * FREQ2 * x / SAMPLING_RATE) for x in range(NUM_SAMPLES)]
#    # convert the sine wave to the audio stream
#    sine_wave = sine_wave1 + sine_wave2
#    sine_wave = np.array(sine_wave * VOLUME, dtype=np.int16)
#    # play the audio stream
#    stream.write(sine_wave)

try:
    print("starting")
    import pyaudio
    import numpy as np
    import time

    # set the frequency of the two sounds
    freq1 = 900 # in Hz
    freq2 = 1100 # in Hz

    # set the duration of each sound in seconds
    duration = 0.2

    # initialize PyAudio
    p = pyaudio.PyAudio()

    for i in range(p.get_device_count()):
        print("\n Index " + str(i) + " :")
        dev = p.get_device_info_by_index(i)
        print(dev.get('name'))
        print("------------------------------------------------------------")
    device = 14

    # create the two sounds
    samples1 = (np.sin(2 * np.pi * freq1 * np.arange(44100 * duration)) * 32767).astype(np.int16)
    samples2 = (np.sin(2 * np.pi * freq2 * np.arange(44100 * duration)) * 32767).astype(np.int16)

    # open the output stream
    stream = p.open(format=pyaudio.paInt16, channels=1, rate=44100, output=True)

    print("playing sequence")

    # play the sequence of sounds
    for i in range(13): 
        if i % 2 == 0:
            stream.write(samples1)
        else:
            stream.write(samples2)
        #time.sleep(duration)

    # close the output stream and terminate PyAudio
    #stream.stop_stream()
    stream.close()
    p.terminate()
    print("done")
except Exception as err:
    print("error: ",err)
