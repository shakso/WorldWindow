import subprocess
import shlex
import json
import urllib.request
import os
import time
import paho.mqtt.client as mqtt

p=""

def LoadVideo(url, transform, croptop, cropbottom, paddleft, cropleft, audio):
	global p
	ytjson = subprocess.run(['youtube-dl','-j','--skip-download',url], stdout=subprocess.PIPE).stdout.decode('utf-8')

	master_manifest = json.loads(ytjson)['manifest_url']
	hd_manifest = ''

	with urllib.request.urlopen(master_manifest) as response:
	   html = response.read().decode('utf-8')
	   index = 0 
	   mani_lines = html.splitlines()
	   while index < len(mani_lines):
	    if ("1920x1080" in mani_lines[index]):
	      hd_manifest = mani_lines[index+1]
	    index += 1

	args = shlex.split('cvlc ' + audio + ' --swscale-mode=8 --sout-transcode-vfilter=transform:croppadd --transform-type='+ transform + ' --croppadd-croptop='+ croptop + ' --croppadd-cropbottom='+ cropbottom + ' --croppadd-paddright='+ paddleft + ' --croppadd-cropright=' + cropleft + ' --sout "#transcode{venc=x264{preset=fast},vcodec=h264,threads=1}:duplicate{dst=std{access=livehttp{seglen=5,delsegs=true,numsegs=5,index=/var/www/html/stream.m3u8,index-url=stream-########.ts},mux=ts{use-key-frames},dst=/var/www/html/stream-########.ts}}}" ' + hd_manifest)
	p = subprocess.Popen(args)


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("videowindow")

def on_message(client, userdata, msg):
    global p

    if "LOC" in str(msg.payload.decode('utf-8')):
        message = str(msg.payload.decode('utf-8')).replace('LOC=', '')

        if (hasattr(p, "terminate")):
            print ("killing")
            p.terminate()
            outs, errs = p.communicate()
    
        if (message == "AMS"):
            LoadVideo('https://www.youtube.com/watch?v=8dQbULXs0U4', '90', '200', '1100', '140', '0','') # AMS
        if (message == "VER"):
            LoadVideo('https://www.youtube.com/watch?v=yDKJMdZTEXQ', '90', '650', '650', '0', '0','') # VER
        if (message == "LAS"):
            LoadVideo('https://www.youtube.com/watch?v=CUyy8rBnuzY', '90', '650', '650', '140', '0','--no-sout-audio') # LAS
        if (message == "FLO"):
            LoadVideo('https://www.youtube.com/watch?v=Zv1fgmd1pr4', '90', '650', '650', '140', '0','--no-sout-audio') # FLO
        if (message == "JPN"):
            LoadVideo('https://www.youtube.com/watch?v=BQYKarjjWi0', '90', '0', '1300', '140', '60','')

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set(username="xxx", password="xxx")
client.connect("192.168.1.2", 1883, 60)

client.loop_forever()

