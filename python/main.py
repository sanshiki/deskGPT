import azure.cognitiveservices.speech as speechsdk  
import os  
  
speech_config = speechsdk.SpeechConfig(subscription=os.environ.get('KEY'), region="eastasia")``audio_config = speechsdk.audio.AudioOutputConfig(use_default_speaker=True)`
