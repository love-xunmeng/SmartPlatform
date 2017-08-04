#coding:utf-8

import urllib2
import json
import base64
import unittest
from os.path import join, getsize
from PIL import Image

def test():
	image_path = "e:\\jingtian.jpg"
	file = open(image_path, 'rb')

	values = {}
	img = Image.open(image_path)
	image_width, image_height = img.size
	image_channels = 0
	if('RGB' == img.mode):
		image_channels = 3
	elif "RGBA" == img.mode:
		image_channels = 4
	base64_string = base64.b64encode(file.read())

	values['image_width'] = image_width
	values['image_height'] = image_height
	values['image_channels'] = image_channels
	values['image_type'] = "PNG"
	values['image_data'] = base64_string

	jdata = json.dumps(values)

	request = urllib2.Request("http://127.0.0.1:18080/CameraOcclusionServiceRequest", jdata, {'Content-Type': 'application/json'})
	response = urllib2.urlopen(request, timeout=30)
	response_data = response.read()

	print response_data

	file.close();

if __name__ == "__main__":
	test()